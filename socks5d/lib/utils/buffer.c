
/**
 * buffer.c - buffer con acceso directo (útil para I/O) que mantiene
 *            mantiene puntero de lectura y de escritura.
 */
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "utils/buffer.h"

inline void BufferReset(ArrayBuffer *b) {
    b->ReadHead = b->Data;
    b->WriteHead = b->Data;
}

void BufferInit(ArrayBuffer *b, size_t n, byte *data) {
    b->Data = data;
    BufferReset(b);
    b->Size = (b->Data + n);
}


inline bool BufferCanWrite(ArrayBuffer *b) {
    return b->Size - b->WriteHead > 0;
}

inline byte *BufferWritePtr(ArrayBuffer *buffer, size_t *nbyte) {
    assert(buffer->WriteHead <= buffer->Size);
    *nbyte = buffer->Size - buffer->WriteHead;
    return buffer->WriteHead;
}

inline bool BufferCanRead(ArrayBuffer *b) {
    return b->WriteHead - b->ReadHead > 0;
}

inline byte *BufferReadPtr(ArrayBuffer *b, size_t *nbyte) {
    assert(b->ReadHead <= b->WriteHead);
    *nbyte = b->WriteHead - b->ReadHead;
    return b->ReadHead;
}

inline void BufferWriteAdv(ArrayBuffer *b, ssize_t bytes) {
    if (bytes > -1) {
        b->WriteHead += (size_t) bytes;
        assert(b->WriteHead <= b->Size);
    }
}

inline void BufferReadAdv(ArrayBuffer *b, ssize_t bytes) {
    if (bytes > -1) {
        b->ReadHead += (size_t) bytes;
        assert(b->ReadHead <= b->WriteHead);

        if (b->ReadHead == b->WriteHead) {
            // compactacion poco costosa
            BufferCompact(b);
        }
    }
}

inline byte BufferReadByte(ArrayBuffer *b) {
    byte ret;
    if (BufferCanRead(b)) {
        ret = *b->ReadHead;
        BufferReadAdv(b, 1);
    } else {
        ret = 0;
    }
    return ret;
}

inline void BufferWriteByte(ArrayBuffer *b, byte c) {
    if (BufferCanWrite(b)) {
        *b->WriteHead = c;
        BufferWriteAdv(b, 1);
    }
}

void BufferCompact(ArrayBuffer *b) {
    if (b->Data == b->ReadHead) {
        // nada por hacer
    } else if (b->ReadHead == b->WriteHead) {
        b->ReadHead = b->Data;
        b->WriteHead = b->Data;
    } else {
        const size_t n = b->WriteHead - b->ReadHead;
        memmove(b->Data, b->ReadHead, n);
        b->ReadHead = b->Data;
        b->WriteHead = b->Data + n;
    }
}

