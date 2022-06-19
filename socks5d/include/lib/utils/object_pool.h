//
// Created by Lucas Dell'Isola on 19/06/2022.
//

#ifndef SOCKS5D_OBJECT_POOL_H
#define SOCKS5D_OBJECT_POOL_H

#include <stddef.h>

typedef struct {
    /**
     * Function to be called after retrieving a new element
     */
    void (* OnCreate) (void *);
    /**
     * Function to be called when disposing an element to be later reused.
     * It must free all allocated memory and resources, as well as clear the
     * object for reuse.
     */
    void (* OnDispose) (void * );
}ObjectPoolHandlers;


typedef struct{
    ObjectPoolHandlers * Handlers;
    void * Pool;
    size_t ObjectSize;
}ObjectPool;

/**
 * It initializes an object socks5Pool
 * @param pool Pointer to the socks5Pool to initialize
 * @param handlers Callbacks needed to destroy and initialize elements
 * @param poolSize Initial size of the socks5Pool
 * @param objectSize Size of the objects stored on the socks5Pool
 */
void InitObjectPool(ObjectPool *pool, ObjectPoolHandlers *handlers, size_t poolSize, size_t objectSize);

/**
 * It retrieves an element from the socks5Pool. If the socks5Pool is full, it will create it
 * @param pool Pointer to the socks5Pool
 * @return Element
 */
void * GetObjectFromPool(ObjectPool * pool);

/**
 * It destroys a pooled object and leaves it ready to use
 * @param pool Pointer to the socks5Pool
 * @param object Object to destroy
 */
void DestroyObject(ObjectPool * pool, void * object);

/**
 * It frees all the allocated memory by the socks5Pool
 * @param pool Pointer to the socks5Pool
 */
void CleanObjectPool(ObjectPool * pool);

/**
 * It executes a function on all objects currently in use
 * @param pool Pointer to socks5Pool
 * @param callback Function to be called
 * @param data extra parameter to be pased into the callback function
 */
void ExecuteOnExistingElements(ObjectPool * pool, void (* callback)(void *, void *), void * data);



#endif //SOCKS5D_OBJECT_POOL_H
