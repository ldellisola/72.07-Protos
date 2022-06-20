//
// Created by Lucas Dell'Isola on 19/06/2022.
//

#ifndef SOCKS5D_SOCKS5_BUFFER_H
#define SOCKS5D_SOCKS5_BUFFER_H


#include <stddef.h>
#include "utils/buffer.h"

/**
 * It changes the size of the Socks5Buffers. It will eventually force all
 * buffers to be updated.
 * @param bufferSize
 */
void SetSocks5BufferSize(size_t bufferSize);

/**
 * It returns the size of the Socks5Buffers.
 */
size_t GetSocks5BufferSize();

/**
 * It initializes a Socks5Buffer
 * @param buffer
 */
void InitSocks5Buffer(ArrayBuffer * buffer);

/**
 * It checks if we should resize the selected buffer
 * @param buffer
 * @return
 */
bool HasToResizeSocks5Buffer(ArrayBuffer * buffer);

/**
 * It resizes the buffer to the new selected size
 * @param buffer
 */
void ResizeSocks5Buffer(ArrayBuffer * buffer);

/**
 * It safely disposes all the memory used by the buffer
 * @param buffer
 */
void DisposeSocks5Buffer(ArrayBuffer * buffer);

#endif //SOCKS5D_SOCKS5_BUFFER_H
