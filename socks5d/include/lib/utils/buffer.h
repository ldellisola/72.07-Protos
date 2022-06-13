//
// Created by Lucas Dell'Isola on 09/06/2022.
//

#ifndef SOCKS5D_BUFFER_H
#define SOCKS5D_BUFFER_H

#include <stdbool.h>
#include <unistd.h>  // size_t, ssize_t
#include "utils.h"

/**
 * buffer.c - buffer con acceso directo (útil para I/O) que mantiene
 *            mantiene puntero de lectura y de escritura.
 *
 *
 * Para esto se mantienen dos punteros, uno de lectura
 * y otro de escritura, y se provee funciones para
 * obtener puntero base y capacidad disponibles.
 *
 * R=0
 * ↓
 * +---+---+---+---+---+---+
 * |   |   |   |   |   |   |
 * +---+---+---+---+---+---+
 * ↑                       ↑
 * W=0                     Size=6
 *
 * Invariantes:
 *    R <= W <= Size
 *
 * Se quiere escribir en el bufer cuatro bytes.
 *
 * ptr + 0 <- BufferWritePtr(b, &wbytes), wbytes=6
 * n = ReadHead(fd, ptr, wbytes)
 * BufferWriteAdv(b, n = 4)
 *
 * R=0
 * ↓
 * +---+---+---+---+---+---+
 * | H | O | L | A |   |   |
 * +---+---+---+---+---+---+
 *                 ↑       ↑
 *                W=4      Size=6
 *
 * Quiero leer 3 del buffer
 * ptr + 0 <- BufferReadPtr, wbytes=4
 * BufferReadAdv(b, 3);
 *
 *            R=3
 *             ↓
 * +---+---+---+---+---+---+
 * | H | O | L | A |   |   |
 * +---+---+---+---+---+---+
 *                 ↑       ↑
 *                W=4      Size=6
 *
 * Quiero escribir 2 bytes mas
 * ptr + 4 <- BufferWritePtr(b, &wbytes=2);
 * BufferWriteAdv(b, 2)
 *
 *            R=3
 *             ↓
 * +---+---+---+---+---+---+
 * | H | O | L | A |   | M |
 * +---+---+---+---+---+---+
 *                         ↑
 *                         Size=6
 *                         W=4
 * Compactación a demanda
 * R=0
 * ↓
 * +---+---+---+---+---+---+
 * | A |   | M |   |   |   |
 * +---+---+---+---+---+---+
 *             ↑           ↑
 *            W=3          Size=6
 *
 * Leo los tres bytes, como R == W, se auto compacta.
 *
 * R=0
 * ↓
 * +---+---+---+---+---+---+
 * |   |   |   |   |   |   |
 * +---+---+---+---+---+---+
 * ↑                       ↑
 * W=0                     Size=6
 */

typedef struct  {
    byte *Data;
    /** límite superior del buffer. inmutable */
    uint8_t *Size;
    /** puntero de lectura */
    byte * ReadHead;
    /** puntero de escritura */
    byte * WriteHead;
} ArrayBuffer;

/**
 * inicializa el buffer sin utilizar el heap
 */
void BufferInit(ArrayBuffer *b, size_t n, byte *data);

/**
 * It creates a small buffer to write to and it tells us how many bytes we can write to it
 * @param buffer internal buffer
 * @param n number of byte I can write to this buffer
 * @return It returns a pointer where I can write up to N bytes
 */
byte * BufferWritePtr(ArrayBuffer *buffer, size_t *n);

/**
 * It moves the buffer head n bytes
 * @param b the buffer
 * @param bytes the number of bytes to move the head
 */
void BufferWriteAdv(ArrayBuffer *b, ssize_t bytes);


byte * BufferReadPtr(ArrayBuffer *b, size_t *nbyte);
void BufferReadAdv(ArrayBuffer *b, ssize_t bytes);

/**
 * obtiene un byte
 */
byte BufferReadByte(ArrayBuffer *b);

/** escribe un byte */
void BufferWriteByte(ArrayBuffer *b, byte c);

/**
 * compacta el buffer
 */
void BufferCompact(ArrayBuffer *b);

/**
 * Reinicia todos los punteros
 */
void BufferReset(ArrayBuffer *b);

/** retorna true si hay bytes para leer del buffer */
bool BufferCanRead(ArrayBuffer *b);

/** retorna true si se pueden escribir bytes en el buffer */
bool BufferCanWrite(ArrayBuffer *b);


#endif //SOCKS5D_BUFFER_H
