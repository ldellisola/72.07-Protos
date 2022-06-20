//
// Created by Lucas Dell'Isola on 19/06/2022.
//

#include "socks5/socks5_buffer.h"
#include "utils/logger.h"

static size_t targetBufferSize = 1000;

void SetSocks5BufferSize(size_t bufferSize) {
    if (bufferSize <=0)
        LogError("Cannot create a buffer of size %ld",bufferSize);
    else
        targetBufferSize = bufferSize;
}

size_t GetSocks5BufferSize() {
    return targetBufferSize;
}

void InitSocks5Buffer(ArrayBuffer *buffer) {
    if (null == buffer)
    {
        Error("Socks5 buffer cannot be null");
        return;
    }

    byte * data = calloc(targetBufferSize, sizeof(byte));

    if (null == data)
        FatalWithReason("Cannot allocate memory for buffer");

    BufferInit(buffer,targetBufferSize,data);
}

bool HasToResizeSocks5Buffer(ArrayBuffer *buffer) {
    if (null == buffer)
    {
        Error("Socks5 buffer cannot be null");
        return false;
    }

    return (size_t)(buffer->Size - buffer->Data) != targetBufferSize;
}

void ResizeSocks5Buffer(ArrayBuffer *buffer) {
    if (null == buffer)
    {
        Error("Socks5 buffer cannot be null");
        return;
    }

    DisposeSocks5Buffer(buffer);
    InitSocks5Buffer(buffer);
}

void DisposeSocks5Buffer(ArrayBuffer *buffer) {
    if (null == buffer)
    {
        Error("Socks5 buffer cannot be null");
        return;
    }

    if (null != buffer->Data)
        free(buffer->Data);
    buffer->Data = null;
}
