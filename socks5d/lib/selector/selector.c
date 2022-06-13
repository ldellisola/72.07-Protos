//
// Created by Lucas Dell'Isola on 09/06/2022.
//
/**
 * selector.c - un muliplexor de entrada salida
 */
#include <stdio.h>  // perror
#include <stdlib.h> // malloc
#include <string.h> // memset
#include <assert.h> // :)
#include <errno.h>  // :)
#include <pthread.h>

#include <stdint.h> // SIZE_MAX
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/signal.h>
#include <signal.h>
#include "selector/selector.h"


#define N(x) (sizeof(x)/sizeof((x)[0]))

#define ERROR_DEFAULT_MSG "something failed"

/** retorna una descripción humana del fallo */
const char * SelectorError(SelectorStatus status) {
    const char *msg;
    switch(status) {
        case SELECTOR_STATUS_SUCCESS:
            msg = "Success";
            break;
        case SELECTOR_STATUS_ENOMEM:
            msg = "Not enough memory";
            break;
        case SELECTOR_STATUS_MAXFD:
            msg = "Can't handle any more file descriptors";
            break;
        case SELECTOR_STATUS_IARGS:
            msg = "Illegal argument";
            break;
        case SELECTOR_STATUS_IO:
            msg = "I/O error";
            break;
        default:
            msg = ERROR_DEFAULT_MSG;
    }
    return msg;
}


static void WakeHandler(const int signal) {
    // nada que hacer. está solo para interrumpir el select
}

// señal a usar para las notificaciones de resolución
SelectorOptions conf;
static sigset_t emptyset, blockset;

SelectorStatus SelectorInit(const SelectorOptions  *c) {
    memcpy(&conf, c, sizeof(conf));

    // inicializamos el sistema de comunicación entre threads y el selector
    // principal. La técnica se encuentra descripta en
    // "The new pselect() system call" <https://lwn.net/Articles/176911/>
    //  March 24, 2006
    SelectorStatus   ret = SELECTOR_STATUS_SUCCESS;
    struct sigaction act = {
            .sa_handler = WakeHandler,
    };

    // 0. calculamos mascara para evitar que se interrumpa antes de llegar al
    //    select
    sigemptyset(&blockset);
    sigaddset  (&blockset, conf.Signal);
    if(-1 == sigprocmask(SIG_BLOCK, &blockset, NULL)) {
        ret = SELECTOR_STATUS_IO;
        goto finally;
    }

    // 1. Registramos una función que atenderá la señal de interrupción
    //    del selector.
    //    Esta interrupción es útil en entornos multi-hilos.

    if (sigaction(conf.Signal, &act, 0)) {
        ret = SELECTOR_STATUS_IO;
        goto finally;
    }
    sigemptyset(&emptyset);

    finally:
    return ret;
}

SelectorStatus SelectorClose(void) {
    // Nada para liberar.
    // TODO(juan): podriamos reestablecer el Handler de la señal.
    return SELECTOR_STATUS_SUCCESS;
}

// estructuras internas
typedef struct {
    int                 Fd;
    FdInterest         Interest;
    const FdHandler   *Handler;
    void *              Data;
} Item;

/* tarea bloqueante */
typedef struct blocking_job {
    /** selector dueño de la resolucion */
    fd_selector  S;
    /** file descriptor dueño de la resolucion */
    int Fd;

    /** datos del trabajo provisto por el usuario */
    void *Data;

    /** el siguiente en la lista */
    struct blocking_job *Next;
} BlockingJob;

/** marca para usar en Item->Fd para saber que no está en uso */
static const int FD_UNUSED = -1;

/** verifica si el Item está usado */
#define ITEM_USED(i) ( ( FD_UNUSED != (i)->Fd) )

struct fdselector {
    // almacenamos en una jump table donde la entrada es el file descriptor.
    // Asumimos que el espacio de file descriptors no va a ser esparso; pero
    // esto podría mejorarse utilizando otra estructura de datos
    Item    *Fds;
    size_t          FdSize;  // cantidad de elementos posibles de Fds

    /** Fd maximo para usar en select() */
    int MaxFd;  // max(.Fds[].Fd)

    /** descriptores prototipicos ser usados en select */
    fd_set MasterRead, MasterWrite;
    /** para ser usado en el select() (recordar que select cambia el valor) */
    fd_set  SlaveRead,  SlaveWrite;

    /** timeout prototipico para usar en select() */
    struct timespec MasterTimeout;
    /** tambien select() puede cambiar el valor */
    struct timespec SlaveTimeout;

    // notificaciónes entre blocking jobs y el selector
    volatile pthread_t      SelectorThread;
    /** protege el acceso a resolutions jobs */
    pthread_mutex_t         ResolutionMutex;
    /**
     * lista de trabajos blockeantes que finalizaron y que pueden ser
     * notificados.
     */
    BlockingJob   *ResolutionJobs;
};

/** cantidad máxima de file descriptors que la plataforma puede manejar */
#define ITEMS_MAX_SIZE      FD_SETSIZE

// en esta implementación el máximo está dado por el límite natural de select(2).

/**
 * determina el tamaño a crecer, generando algo de slack para no tener
 * que realocar constantemente.
 */
static size_t next_capacity(const size_t n) {
    unsigned bits = 0;
    size_t tmp = n;
    while(tmp != 0) {
        tmp >>= 1;
        bits++;
    }
    tmp = 1UL << bits;

    assert(tmp >= n);
    if(tmp > ITEMS_MAX_SIZE) {
        tmp = ITEMS_MAX_SIZE;
    }

    return tmp + 1;
}

static inline void item_init(Item *item) {
    item->Fd = FD_UNUSED;
}

/**
 * inicializa los nuevos items. `last' es el indice anterior.
 * asume que ya está blanqueada la memoria.
 */
static void items_init(fd_selector s, const size_t last) {
    assert(last <= s->FdSize);
    for(size_t i = last; i < s->FdSize; i++) {
        item_init(s->Fds + i);
    }
}

/**
 * calcula el Fd maximo para ser utilizado en select()
 */
static int items_max_fd(fd_selector s) {
    int max = 0;
    for(int i = 0; i <= s->MaxFd; i++) {
        Item *item = s->Fds + i;
        if(ITEM_USED(item)) {
            if(item->Fd > max) {
                max = item->Fd;
            }
        }
    }
    return max;
}

static void items_update_fdset_for_fd(fd_selector s, const Item * item) {
    FD_CLR(item->Fd, &s->MasterRead);
    FD_CLR(item->Fd, &s->MasterWrite);

    if(ITEM_USED(item)) {
        if(item->Interest & SELECTOR_OP_READ) {
            FD_SET(item->Fd, &(s->MasterRead));
        }

        if(item->Interest & SELECTOR_OP_WRITE) {
            FD_SET(item->Fd, &(s->MasterWrite));
        }
    }
}

/**
 * garantizar cierta cantidad de elemenos en `Fds'.
 * Se asegura de que `n' sea un número que la plataforma donde corremos lo
 * soporta
 */
static SelectorStatus ensure_capacity(fd_selector s, const size_t n) {
    SelectorStatus ret = SELECTOR_STATUS_SUCCESS;

    const size_t element_size = sizeof(*s->Fds);
    if(n < s->FdSize) {
        // nada para hacer, entra...
        ret = SELECTOR_STATUS_SUCCESS;
    } else if(n > ITEMS_MAX_SIZE) {
        // me estás pidiendo más de lo que se puede.
        ret = SELECTOR_STATUS_MAXFD;
    } else if(NULL == s->Fds) {
        // primera vez.. alocamos
        const size_t new_size = next_capacity(n);

        s->Fds = calloc(new_size, element_size);
        if(NULL == s->Fds) {
            ret = SELECTOR_STATUS_ENOMEM;
        } else {
            s->FdSize = new_size;
            items_init(s, 0);
        }
    } else {
        // hay que agrandar...
        const size_t new_size = next_capacity(n);
        if (new_size > SIZE_MAX/element_size) { // ver MEM07-C
            ret = SELECTOR_STATUS_ENOMEM;
        } else {
            Item *tmp = realloc(s->Fds, new_size * element_size);
            if(NULL == tmp) {
                ret = SELECTOR_STATUS_ENOMEM;
            } else {
                s->Fds     = tmp;
                const size_t old_size = s->FdSize;
                s->FdSize = new_size;

                items_init(s, old_size);
            }
        }
    }

    return ret;
}

fd_selector SelectorNew(size_t initial_elements) {
    size_t size = sizeof(struct fdselector);
    fd_selector ret = malloc(size);
    if(ret != NULL) {
        memset(ret, 0x00, size);
        ret->MasterTimeout.tv_sec  = conf.SelectTimeout.tv_sec;
        ret->MasterTimeout.tv_nsec = conf.SelectTimeout.tv_nsec;
        assert(ret->MaxFd == 0);
        ret->ResolutionJobs  = 0;
        pthread_mutex_init(&ret->ResolutionMutex, 0);
        if(0 != ensure_capacity(ret, initial_elements)) {
            SelectorDestroy(ret);
            ret = NULL;
        }
    }
    return ret;
}

void SelectorDestroy(fd_selector s) {
    // lean ya que se llama desde los casos fallidos de _new.
    if(s != NULL) {
        if(s->Fds != NULL) {
            for(size_t i = 0; i < s->FdSize ; i++) {
                if(ITEM_USED(s->Fds + i)) {
                    SelectorUnregisterFd(s, i);
                }
            }
            pthread_mutex_destroy(&s->ResolutionMutex);
            for(BlockingJob *j = s->ResolutionJobs; j != NULL;
                j = j->Next) {
                free(j);
            }
            free(s->Fds);
            s->Fds     = NULL;
            s->FdSize = 0;
        }
        free(s);
    }
}

#define INVALID_FD(fd)  ((fd) < 0 || (fd) >= ITEMS_MAX_SIZE)

SelectorStatus SelectorRegister(
        fd_selector        s,
        int          fd,
        const FdHandler  *handler,
        FdInterest interest,
        void *data)
        {
    SelectorStatus ret = SELECTOR_STATUS_SUCCESS;
    // 0. validación de argumentos
    if(s == NULL || INVALID_FD(fd) || handler == NULL) {
        ret = SELECTOR_STATUS_IARGS;
        goto finally;
    }
    // 1. tenemos espacio?
    size_t ufd = (size_t)fd;
    if(ufd > s->FdSize) {
        ret = ensure_capacity(s, ufd);
        if(SELECTOR_STATUS_SUCCESS != ret) {
            goto finally;
        }
    }

    // 2. registración
    Item * item = s->Fds + ufd;
    if(ITEM_USED(item)) {
        ret = SELECTOR_STATUS_FDINUSE;
        goto finally;
    } else {
        item->Fd       = fd;
        item->Handler  = handler;
        item->Interest = interest;
        item->Data     = data;

        // actualizo colaterales
        if(fd > s->MaxFd) {
            s->MaxFd = fd;
        }
        items_update_fdset_for_fd(s, item);
    }

    finally:
    return ret;
}

SelectorStatus SelectorUnregisterFd(fd_selector s, int fd) {
    SelectorStatus ret = SELECTOR_STATUS_SUCCESS;

    if(NULL == s || INVALID_FD(fd)) {
        ret = SELECTOR_STATUS_IARGS;
        goto finally;
    }

    Item *item = s->Fds + fd;
    if(!ITEM_USED(item)) {
        ret = SELECTOR_STATUS_IARGS;
        goto finally;
    }

    if(item->Handler->handle_close != NULL) {
        SelectorKey key = {
                .Selector    = s,
                .Fd   = item->Fd,
                .Data = item->Data,
        };
        item->Handler->handle_close(&key);
    }

    item->Interest = SELECTOR_OP_NOOP;
    items_update_fdset_for_fd(s, item);

    memset(item, 0x00, sizeof(*item));
    item_init(item);
    s->MaxFd = items_max_fd(s);

    finally:
    return ret;
}

SelectorStatus SelectorSetInterest(fd_selector s, int fd, FdInterest i) {
    SelectorStatus ret = SELECTOR_STATUS_SUCCESS;

    if(NULL == s || INVALID_FD(fd)) {
        ret = SELECTOR_STATUS_IARGS;
        goto finally;
    }
    Item *item = s->Fds + fd;
    if(!ITEM_USED(item)) {
        ret = SELECTOR_STATUS_IARGS;
        goto finally;
    }
    item->Interest = i;
    items_update_fdset_for_fd(s, item);
    finally:
    return ret;
}

SelectorStatus SelectorSetInterestKey(SelectorKey *key, FdInterest i) {
    SelectorStatus ret;

    if(NULL == key || NULL == key->Selector || INVALID_FD(key->Fd)) {
        ret = SELECTOR_STATUS_IARGS;
    } else {
        ret = SelectorSetInterest(key->Selector, key->Fd, i);
    }

    return ret;
}

/**
 * se encarga de manejar los resultados del select.
 * se encuentra separado para facilitar el testing
 */
static void handle_iteration(fd_selector s) {
    int n = s->MaxFd;
    SelectorKey key = {
            .Selector = s,
    };

    for (int i = 0; i <= n; i++) {
        Item *item = s->Fds + i;
        if(ITEM_USED(item)) {
            key.Fd   = item->Fd;
            key.Data = item->Data;
            if(FD_ISSET(item->Fd, &s->SlaveRead)) {
                if(SELECTOR_OP_READ & item->Interest) {
                    if(0 == item->Handler->handle_read) {
                        assert(("SELECTOR_OP_READ arrived but no Handler. bug!" == 0));
                    } else {
                        item->Handler->handle_read(&key);
                    }
                }
            }
            if(FD_ISSET(i, &s->SlaveWrite)) {
                if(SELECTOR_OP_WRITE & item->Interest) {
                    if(0 == item->Handler->handle_write) {
                        assert(("SELECTOR_OP_WRITE arrived but no Handler. bug!" == 0));
                    } else {
                        item->Handler->handle_write(&key);
                    }
                }
            }
        }
    }
}

static void handle_block_notifications(fd_selector s) {
    SelectorKey key = {
            .Selector = s,
    };
    pthread_mutex_lock(&s->ResolutionMutex);
    for(BlockingJob *j = s->ResolutionJobs;
        j != NULL ;
        j  = j->Next) {

        Item *item = s->Fds + j->Fd;
        if(ITEM_USED(item)) {
            key.Fd   = item->Fd;
            key.Data = item->Data;
            item->Handler->handle_block(&key);
        }

        free(j);
    }
    s->ResolutionJobs = 0;
    pthread_mutex_unlock(&s->ResolutionMutex);
}


SelectorStatus SelectorNotifyBlock(fd_selector  s,
                                   const int    fd) {
    SelectorStatus ret = SELECTOR_STATUS_SUCCESS;

    // TODO(juan): usar un pool
    BlockingJob *job = malloc(sizeof(*job));
    if(job == NULL) {
        ret = SELECTOR_STATUS_ENOMEM;
        goto finally;
    }
    job->S  = s;
    job->Fd = fd;

    // encolamos en el selector los resultados
    pthread_mutex_lock(&s->ResolutionMutex);
    job->Next = s->ResolutionJobs;
    s->ResolutionJobs = job;
    pthread_mutex_unlock(&s->ResolutionMutex);

    // notificamos al hilo principal
    pthread_kill(s->SelectorThread, conf.Signal);

    finally:
    return ret;
}

SelectorStatus SelectorSelect(fd_selector s) {
    SelectorStatus ret = SELECTOR_STATUS_SUCCESS;

    memcpy(&s->SlaveRead, &s->MasterRead, sizeof(s->SlaveRead));
    memcpy(&s->SlaveWrite, &s->MasterWrite, sizeof(s->SlaveWrite));
    memcpy(&s->SlaveTimeout, &s->MasterTimeout, sizeof(s->SlaveTimeout));

    s->SelectorThread = pthread_self();

    int fds = pselect(s->MaxFd + 1, &s->SlaveRead, &s->SlaveWrite, 0, &s->SlaveTimeout,
                      &emptyset);
    if(-1 == fds) {
        switch(errno) {
            case EAGAIN:
            case EINTR:
                // si una señal nos interrumpio. ok!
                break;
            case EBADF:
                // ayuda a encontrar casos donde se cierran los Fd pero no
                // se desregistraron
                for(int i = 0 ; i < s->MaxFd; i++) {
                    if(FD_ISSET(i, &s->MasterRead) || FD_ISSET(i, &s->MasterWrite)) {
                        if(-1 == fcntl(i, F_GETFD, 0)) {
                            fprintf(stderr, "Bad descriptor detected: %d\n", i);
                        }
                    }
                }
                ret = SELECTOR_STATUS_IO;
                break;
            default:
                ret = SELECTOR_STATUS_IO;
                goto finally;

        }
    } else {
        handle_iteration(s);
    }
    if(ret == SELECTOR_STATUS_SUCCESS) {
        handle_block_notifications(s);
    }
    finally:
    return ret;
}

int SelectorFdSetNio(const int fd) {
    int ret = 0;
    int flags = fcntl(fd, F_GETFD, 0);
    if(flags == -1) {
        ret = -1;
    } else {
        if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
            ret = -1;
        }
    }
    return ret;
}
