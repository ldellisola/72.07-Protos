//
// Created by tluci on 19/6/2022.
//

#ifndef SOCKS5D_LULU_CONNECTION_STATUS_H
#define SOCKS5D_LULU_CONNECTION_STATUS_H

typedef enum {

    /**
     * recibe el mensaje del `auth' del cliente.
     *
     * Intereses:
     *     - SELECTOR_OP_READ sobre client_fd
     *
     * Transiciones:
     *   - CS_AUTH_READ  mientras queden bytes por leer
     *   - CS_AUTH_WRITE cuando se leyeron todos los bytes
     *   - CS_ERROR      ante cualquier error (IO/parseo)
     */
    LULU_CS_AUTH_READ,
    /**
     * envia la respuesta de `auth' al cliente.
     *
     * Intereses:
     *     - SELECTOR_OP_WRITE sobre client_fd
     *
     * Transiciones:
     *   - CS_AUTH_WRITE  mientras queden bytes por escribir
     *   - CS_REQUEST_READ cuando se manden todos los bytes
     *   - CS_ERROR      ante cualquier error (IO/parseo)
     */
    LULU_CS_AUTH_WRITE,
    /**
     * recibe el mensaje del `request' del cliente.
     *
     * Intereses:
     *     - SELECTOR_OP_READ sobre client_fd
     *
     * Transiciones:
     *   - CS_REQUEST_READ  mientras queden bytes por leer
     *   - CS_ERROR      ante cualquier error (IO/parseo)
     */
    LULU_CS_TRANSACTION_READ,
    /**
     * envia la respuesta de `request' al cliente.
     *
     * Intereses:
     *     - SELECTOR_OP_WRITE sobre client_fd
     *
     * Transiciones:
     *   - CS_DONE      Si respondo con un comando invalido
     *   - CS_COPY      Si salio bien
     *   - CS_ERROR      ante cualquier error (IO/parseo)
     */
    LULU_CS_TRANSACTION_WRITE,
    LULU_CS_DONE,
    LULU_CS_ERROR

} CONNECTION_STATE;

#endif //SOCKS5D_LULU_CONNECTION_STATUS_H
