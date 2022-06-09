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
    CS_DNS_READ,
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
     * envia la respuesta de `request' al cliente.
     *
     * Intereses:
     *     - SELECTOR_OP_WRITE sobre client_fd
     *
     * Transiciones:
     * TODO: Work on this part of the FSM
     *
     *   - CS_ERROR      ante cualquier error (IO/parseo)
     */
    CS_REQUEST_WRITE,
    CS_ERROR,
    CS_DONE

} CONNECTION_STATE;

#endif //SOCKS5D_SOCKS5_CONNECTION_STATUS_H
