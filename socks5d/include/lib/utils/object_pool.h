//
// Created by Lucas Dell'Isola on 19/06/2022.
//

#ifndef SOCKS5D_OBJECT_POOL_H
#define SOCKS5D_OBJECT_POOL_H

#include <stddef.h>

typedef struct {
    void (* OnCreate) (void *);
    void (* OnDispose) (void * );
}ObjectPoolHandlers;


typedef struct{
    ObjectPoolHandlers * Handlers;
    void * Pool;
    size_t ObjectSize;
}ObjectPool;


void InitObjectPool(ObjectPool *pool, ObjectPoolHandlers *handlers, size_t poolSize, size_t objectSize);

void * GetObjectFromPool(ObjectPool * pool);

void DestroyObject(ObjectPool * pool, void * object);

void CleanObjectPool(ObjectPool * pool);



#endif //SOCKS5D_OBJECT_POOL_H
