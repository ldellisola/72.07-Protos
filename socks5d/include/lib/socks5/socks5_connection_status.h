//
// Created by Lucas Dell'Isola on 09/06/2022.
//

#ifndef SOCKS5D_SOCKS5_CONNECTION_STATUS_H
#define SOCKS5D_SOCKS5_CONNECTION_STATUS_H

typedef enum {
    /**
     * recibe el mensaje `hello` del cliente, y lo procesa
     *
     * Intereses:
     *     - SELECTOR_OP_READ sobre client_fd
     *
     * Transiciones:
     *   - CS_HELLO_READ  mientras el mensaje no esté completo
     *   - CS_HELLO_WRITE cuando está completo
     *   - CS_ERROR       ante cualquier error (IO/parseo)
     */
    CS_HELLO_READ,
    /**
     * envía la respuesta del `hello' al cliente.
     *
     * Intereses:
     *     - SELECTOR_OP_WRITE sobre client_fd
     *
     * Transiciones:
     *   - CS_HELLO_WRITE  mientras queden bytes por enviar
     *   - CS_AUTH_READ cuando se enviaron todos los bytes
     *   - CS_REQUEST_READ cuando se enviaron todos los bytes
     *   - CS_ERROR        ante cualquier error (IO/parseo)
     */
    CS_HELLO_WRITE,
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
    CS_AUTH_READ,
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
    CS_AUTH_WRITE,
    /**
     * recibe el mensaje del `request' del cliente.
     *
     * Intereses:
     *     - SELECTOR_OP_READ sobre client_fd
     *
     * Transiciones:
     *   - CS_REQUEST_READ  mientras queden bytes por leer
     *   - CS_ESTABLISH_CONNECTION cuando se leyeron todos los bytes
     *   - CS_DNS_READ si hay que resolver un FQDN
     *   - CS_ERROR      ante cualquier error (IO/parseo)
     */
    CS_REQUEST_READ,
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
    CS_REQUEST_WRITE,
    /**
     * Espero conectarrme al servidor
     *
     * Intereses:
     *     - SELECTOR_OP_READ sobre el fd del servidor
     *
     * Transiciones:
     *   - CS_ESTABLISH_CONNECTION  mientras queden bytes por leer
     *   - CS_REQUEST_ESTABLISH_CONNECTION cuando se leyeron todos los bytes
     *   - CS_ERROR      ante cualquier error (IO/parseo)
     */
    CS_ESTABLISH_CONNECTION,
    /**
     * La conexion fue realizada y estoy esperando a que el cliente me envie datos para el servidor remoto
     *
     * Intereses:
     *      - SELECTOR_OP_READ sobre el fd del cliente y del servidor remoto
     * Transiciones:
     *   - CS_CONNECTED  mientras transfiera
     *   - CS_DONE       Si termina
     *   - CS_ERROR      ante cualquier error (IO/parseo)
     */
    CS_CONNECTED,
    /**
     * el cliente intento de mandarle informacion al servidor remoto
     *
     * Intereses:
     *      - SELECTOR_OP_WRITE sobre el servidor remoto
     * Transiciones:
     *   - CS_CLIENT_READ  mientras transfiera
     *   - CS_REMOTE_WRITE Para escribirle al servidor remoto
     *   - CS_ERROR      ante cualquier error (IO/parseo)
     */
    CS_CLIENT_READ,
    /**
     * el cliente intento de mandarle informacion al servidor remoto
     *
     * Intereses:
     *      - SELECTOR_OP_WRITE sobre el servidor remoto
     * Transiciones:
     *   - CS_REMOTE_WRITE  mientras transfiera
     *   - CS_CONNECTED  Para volver a escuchar en los FDs
     *   - CS_ERROR      ante cualquier error (IO/parseo)
     */
    CS_REMOTE_WRITE,
    /**
     * el servidor remote intento de mandarle informacion al cliente
     *
     * Intereses:
     *      - SELECTOR_OP_WRITE sobre el servidor remoto
     * Transiciones:
     *   - CS_REMOTE_READ   mientras transfiera
     *   - CS_CLIENT_WRITE  Para escribirle al cliente
     *   - CS_ERROR         ante cualquier error (IO/parseo)
     */
    CS_REMOTE_READ,
    /**
     * el cliente intento de mandarle informacion al servidor remoto
     *
     * Intereses:
     *      - SELECTOR_OP_WRITE sobre el cliente
     * Transiciones:
     *   - CS_REMOTE_WRITE  mientras transfiera
     *   - CS_CONNECTED  Para volver a escuchar en los FDs
     *   - CS_ERROR      ante cualquier error (IO/parseo)
     */
    CS_CLIENT_WRITE,
    /**
     * recibe el mensaje del dns
     *
     * Intereses:
     *     - SELECTOR_OP_READ sobre el fd del dns???
     *
     * Transiciones:
     *   - CS_DNS_READ  mientras queden bytes por leer
     *   - CS_ESTABLISH_CONNECTION cuando se leyeron todos los bytes
     *   - CS_ERROR      ante cualquier error (IO/parseo)
     */
    //CS_DNS_READ,
    CS_DONE,
    CS_ERROR

} CONNECTION_STATE;

#endif //SOCKS5D_SOCKS5_CONNECTION_STATUS_H
