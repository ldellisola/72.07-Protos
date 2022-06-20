//
// Created by Lucas Dell'Isola on 19/06/2022.
//

#include <assert.h>
#include "utils/object_pool.h"
#include "utils/logger.h"
#include "utils/utils.h"

typedef enum {
    PooledObjectEmpty,
    PooledObjectInUse
} PooledObjectStatus;


typedef struct PooledObject_ {
    void * Object;
    PooledObjectStatus Status;
    struct PooledObject_ *Next;
} PooledObject;


void InitObjectPool(ObjectPool *pool, ObjectPoolHandlers *handlers, size_t poolSize, size_t objectSize) {
    Debug("Initializing socks5Pool");
    if (poolSize < 1) {
        LogDebug("Invalid initial socks5Pool size %d, using default value 1", poolSize);
        poolSize = 1;
    }
    pool->ObjectSize = objectSize;
    pool->Handlers = handlers;


    PooledObject * o = calloc(1, sizeof(PooledObject));
    if (null == o)
        FatalWithReason("Cannot allocate memory");
    pool->Pool = o;

    o->Object = calloc(1,objectSize);
    o->Status = PooledObjectEmpty;
    PooledObject *current = o;

    while (--poolSize > 0) {
        current->Next = calloc(1, sizeof(PooledObject));
        current = current->Next;
        current->Object = calloc(1,objectSize);
        current->Status = PooledObjectEmpty;
    }
}

void * GetObjectFromPool(ObjectPool *pool) {
    if (null == pool || null == pool->Handlers) {
        Warning( "Object socks5Pool was not initialized");
        return null;
    }
    PooledObject * temp;

    for ( temp = pool->Pool; temp != null ; temp = temp->Next) {
        if (PooledObjectEmpty == temp->Status) {
            temp->Status = PooledObjectInUse;

            if (null != pool->Handlers->OnCreate)
                pool->Handlers->OnCreate(temp->Object);

            return temp->Object;
        }

        if (null == temp->Next)
            break;
    }

    temp->Next = calloc(1, sizeof(PooledObject));
    if (null == temp->Next)
        FatalWithReason("Cannot allocate memory");

    temp = temp->Next;
    temp->Object = calloc(1,pool->ObjectSize);

    if (null == temp->Object)
        FatalWithReason("Cannot allocate memory");

    temp->Status = PooledObjectInUse;

    if (null != pool->Handlers->OnCreate)
        pool->Handlers->OnCreate(temp->Object);

    return temp->Object;
}

void DestroyObject(ObjectPool *pool, void *object) {
    if (null == pool || null == pool->Handlers) {
        Error( "Object socks5Pool was not initialized");
        return;
    }

    if (null != pool->Handlers->OnDispose)
        pool->Handlers->OnDispose(object);
    PooledObject * temp;

    for (temp = pool->Pool; temp != null && temp->Object != object ; temp = temp->Next);

    if (null == temp)
    {
        Error("Error while destroying connection!");
        return;
    }

    assert(temp->Object == object);
    temp->Status = PooledObjectEmpty;

}

void CleanObjectPool(ObjectPool *pool) {
    Debug("Cleaning object socks5Pool");
    if (null == pool || null == pool->Handlers) {
        Error("Object socks5Pool was not initialized");
        return;
    }

    PooledObject * next;
    for (PooledObject * obj = pool->Pool; obj != null ; obj = next) {
        next = obj->Next;
        if ( PooledObjectInUse == obj->Status && null != pool->Handlers->OnDispose){
            pool->Handlers->OnDispose(obj);
        }
        free(obj->Object);
        free(obj);
    }
}

void ExecuteOnExistingElements(ObjectPool *pool, void (* callback)(void *, void *), void * data) {
    if (null == pool || null == pool->Handlers) {
        Error("Object socks5Pool was not initialized");
        return;
    }

    for (PooledObject * temp = pool->Pool; temp != null ; temp = temp->Next){
        if (PooledObjectEmpty == temp->Status)
            continue;
        callback(temp->Object,data);
    }
}













