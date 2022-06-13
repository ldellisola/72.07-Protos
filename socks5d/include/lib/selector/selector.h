//
// Created by Lucas Dell'Isola on 09/06/2022.
//

#ifndef SOCKS5D_SELECTOR_H
#define SOCKS5D_SELECTOR_H
#include <sys/time.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * selector.c - un muliplexor de entrada salida
 *
 * Un selector permite manejar en un único hilo de ejecución la entrada salida
 * de file descriptors de forma no bloqueante.
 *
 * Esconde la implementación final (select(2) / poll(2) / epoll(2) / ..)
 *
 * El usuario registra para un file descriptor especificando:
 *  1. un Handler: provee funciones callback que manejarán los eventos de
 *     entrada/salida
 *  2. un interés: que especifica si interesa leer o escribir.
 *
 * Es importante que los handlers no ejecute tareas bloqueantes ya que demorará
 * el procesamiento del resto de los descriptores.
 *
 * Si el Handler requiere bloquearse por alguna razón (por ejemplo realizar
 * una resolución de DNS utilizando getaddrinfo(3)), tiene la posiblidad de
 * descargar el trabajo en un hilo notificará al selector que el resultado del
 * trabajo está disponible y se le presentará a los handlers durante
 * la iteración normal. Los handlers no se tienen que preocupar por la
 * concurrencia.
 *
 * Dicha señalización se realiza mediante señales, y es por eso que al
 * iniciar la librería `SelectorInit' se debe configurar una señal a utilizar.
 *
 * Todos métodos retornan su estado (éxito / error) de forma uniforme.
 * Puede utilizar `SelectorError' para obtener una representación human
 * del estado. Si el valor es `SELECTOR_STATUS_IO' puede obtener información adicional
 * en errno(3).
 *
 * El flujo de utilización de la librería es:
 *  - iniciar la libreria `SelectorInit'
 *  - crear un selector: `SelectorNew'
 *  - registrar un file descriptor: `selector_register_fd'
 *  - esperar algún evento: `selector_iteratate'
 *  - destruir los recursos de la librería `SelectorClose'
 */
typedef struct fdselector * fd_selector;


/** valores de retorno. */
typedef enum {
    /** llamada exitosa */
    SELECTOR_STATUS_SUCCESS  = 0,
    /** no pudimos alocar memoria */
    SELECTOR_STATUS_ENOMEM   = 1,
    /** llegamos al límite de descriptores que la plataforma puede manejar */
    SELECTOR_STATUS_MAXFD    = 2,
    /** argumento ilegal */
    SELECTOR_STATUS_IARGS    = 3,
    /** descriptor ya está en uso */
    SELECTOR_STATUS_FDINUSE  = 4,
    /** I/O error check errno */
    SELECTOR_STATUS_IO       = 5,
} SelectorStatus;

/** retorna una descripción humana del fallo */
const char * SelectorError(SelectorStatus status);

/** opciones de inicialización del selector */
typedef struct{
    /** señal a utilizar para notificaciones internas */
    const int Signal;

    /** tiempo máximo de bloqueo durante `selector_iteratate' */
    struct timespec SelectTimeout;
}SelectorOptions;

/** inicializa la librería */
SelectorStatus SelectorInit(const SelectorOptions *c);

/** deshace la incialización de la librería */
SelectorStatus SelectorClose(void);

/* instancia un nuevo selector. returna NULL si no puede instanciar  */
fd_selector SelectorNew(size_t initial_elements);

/** destruye un selector creado por _new. Tolera NULLs */
void SelectorDestroy(fd_selector s);

/**
 * Intereses sobre un file descriptor (quiero leer, quiero escribir, …)
 *
 * Son potencias de 2, por lo que se puede requerir una conjunción usando el OR
 * de bits.
 *
 * SELECTOR_OP_NOOP es útil para cuando no se tiene ningún interés.
 */
typedef enum {
    SELECTOR_OP_NOOP    = 0,
    SELECTOR_OP_READ    = 1 << 0,
    SELECTOR_OP_WRITE   = 1 << 2,
} FdInterest ;

/**
 * Quita un interés de una lista de intereses
 */
#define INTEREST_OFF(FLAG, MASK)  ( (FLAG) & ~(MASK) )

/**
 * Argumento de todas las funciones callback del Handler
 */
typedef struct {
    /** el selector que dispara el evento */
    fd_selector Selector;
    /** el file descriptor en cuestión */
    int         Fd;
    /** dato provisto por el usuario */
    void *      Data;
}SelectorKey;

/**
 * Manejador de los diferentes eventos..
 */
typedef struct {
    void (*handle_read)      (SelectorKey *key);
    void (*handle_write)     (SelectorKey *key);
    void (*handle_block)     (SelectorKey *key);

    /**
     * llamado cuando se se desregistra el Fd
     * Seguramente deba liberar los recusos alocados en Data.
     */
    void (*handle_close)     (SelectorKey *key);

} FdHandler;

/**
 * registra en el selector `Selector' un nuevo file descriptor `Fd'.
 *
 * Se especifica un `Interest' inicial, y se pasa Handler que manejará
 * los diferentes eventos. `Data' es un adjunto que se pasa a todos
 * los manejadores de eventos.
 *
 * No se puede registrar dos veces un mismo Fd.
 *
 * @return 0 si fue exitoso el registro.
 */
SelectorStatus SelectorRegister(
        fd_selector        s,
        int          fd,
        const FdHandler  *handler,
        FdInterest  interest,
        void *data);

/**
 * desregistra un file descriptor del selector
 */
SelectorStatus SelectorUnregisterFd(fd_selector s, int fd);

/** permite cambiar los intereses para un file descriptor */
SelectorStatus SelectorSetInterest(fd_selector s, int fd, FdInterest i);

/** permite cambiar los intereses para un file descriptor */
SelectorStatus SelectorSetInterestKey(SelectorKey *key, FdInterest i);


/**
 * se bloquea hasta que hay eventos disponible y los despacha.
 * Retorna luego de cada iteración, o al llegar al timeout.
 */
SelectorStatus SelectorSelect(fd_selector s);

/**
 * Método de utilidad que activa O_NONBLOCK en un Fd.
 *
 * retorna -1 ante error, y deja detalles en errno.
 */
int SelectorFdSetNio(const int fd);

/** notifica que un trabajo bloqueante terminó */
SelectorStatus SelectorNotifyBlock(fd_selector s, const int   fd);


#endif //SOCKS5D_SELECTOR_H
