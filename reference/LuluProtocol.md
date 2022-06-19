# LULU Protocol - Version 1
## 1. Introduccion

## 2. A short digression

## 3. Operaciones Básicas

Inicialmente, el servidor empieza el servicio LULU al escuchar en TCP al puerto 8080. Cuando un cliente quiere hacer uso del servicio, establece una conexión TCP con el servidor. Cuando la conexión es establecida, el servidor LULU envía un saludo. El cliente y servidor intercambian comandos y respuestas (respectivamente) hasta que la conexión sea terminada o aborte. 
Los comandos en LULU son case-sensitive, posiblemente seguidos por uno o más argumentos. Todos los comandos son terminados por el par CRLF. Las palabras clave y sus argumentos consisten de caracteres ASCII. Las palabras clave y los argumentos son separados por un único carácter “|”(PIPE). 
Las respuestas en LULU consisten en un indicador de estado seguida por información adicional. Todas las respuestas son terminadas por el par CRLF. Actualmente hay dos indicadores de estado: positivo (“+”) y negativo (“-”).
Una sesión LULU pasa por varios estados en su ciclo de vida. Una vez que la conexión TCP se ha abierto y el servidor LULU ha enviado el saludo, la sesión entra al estado de AUTORIZACIÓN. En este estado el cliente debe identificarse al servidor LULU. Una vez que el cliente ha hecho esto de forma exitosa, la sesión entra en el estado de TRANSACCIÓN. En este estado, el cliente solicita acciones de parte del servidor LULU. Cuando el cliente da el comando GOODBYE, el servidor LULU responde con goodbye. La conexión TCP luego es terminada.
El servidor DEBE responder a un comando no reconocido, no implementado, o invalido con un indicador negativo de estado. El servidor DEBE responder a un comando dado en un estado incorrecto respondiendo con un indicador de estado negativo. 
Un servidor LULU tiene un timer de inactividad en el que desloguea al cliente al haber pasado este tiempo. El mínimo de tiempo aceptado son 10 minutos. Cuando el tiempo de inactividad es alcanzado el servidor cierra la conexión TCP sin enviar ninguna respuesta al cliente.

## 4. Estado de AUTORIZACIÓN

Una vez que la conexión TCP ha sido abierta por el cliente de LULU, la sesión LULU ahora esta en el estado de AUTORIZACIÓN. El cliente debe ahora identificarse y autenticarse al servidor LULU. Para hacer esto se utiliza el comando HELLO el cual será explicado más adelante en este documento. Es posible que varios clientes se autentiquen como el mismo usuario. 
Si las credenciales son correctas el servidor responde con un indicador de estado positivo. La sesión LULU ahora entra al estado de TRANSACCIÓN. 
Si no son correctas entonces se responde con un indicador de estado negativo, y el cliente puede dar nuevamente el comando HELLO, o de lo contrario dar el comando GOODBYE.
Se mostrará un ejemplo del comando GOODBYE utilizándolo en el estado de AUTORIZACIÓN:
    
GOODBYE
            
    Argumentos: ninguno
            
    Restricciones: ninguna

    Descripción:
        Para simplificar el parseo, todos los servidores LULU utilizan un formato. La respuesta positiva consiste en un “+”, seguido por una palabra clave GOODBYE, terminado con el par CRLF. La sesión LULU será terminada cerrando la conexión TCP.
            
    Respuestas posibles:
        +GOODBYE
            
    Ejemplo:
        C:	GOODBYE
        S:	+GOODBYE

A continuación se mostrará el funcionamiento del comando HELLO

HELLO|username|password

    Argumentos: 
        .Username: Nombre de usuario con un máximo de 255 caracteres de longitud y un minimo de 1 caracter de longitud. No puede contener los siguientes caracteres: el caracter ASCII 214 (“|”); el carácter ASCII 13 (CR); el carácter ASCII 10(LF).
        .Password: contraseña de usuario con un máximo de 255 caracteres de longitud y un minimo de 1 caracter de longitud. No puede contener los siguientes caracteres: el caracter ASCII 214 (“|”);  el par CRLF consecutivos (ASCII 13 (CR), ASCII 10(LF)) .
            
    Restricciones:
        Solo puede ser dado en el estado de AUTORIZACIÓN.
            
    Descripción: 
        Para simplificar el parseo, todos los servidores LULU utilizan un formato. La respuesta positiva consiste en un “+”, seguido por un valor entero positivo mostrando la cantidad de conexiones históricas, luego “|”(pipe) seguido por un valor entero positivo representando la cantidad de conexiones concurrentes, luego “|” seguido por la cantidad de bytes transferidos, terminado con el par CRLF.
            
    Respuestas posibles:
        +OK
        -AUTHENTICATION ERROR

    Ejemplo:
        C:	HELLO|user1|pass
        S:	+OK
        …
        C:	HELLO|user1|pass1
        S:	-AUTHENTICATION ERROR

## 5. Estado de TRANSACCIÓN
Una vez que el cliente se ha identificado de manera correcta al servidor LULU, y este ha podido abrir de forma satisfactoria su sesión, LULU entra en estado de TRANSACCIÓN. El cliente ahora puede dar los siguientes comandos de LULU de forma repetida. Después de cada comando, el servidor LULU da una respuesta. Eventualmente el cliente da el comando GOODBYE y la sesión LULU será terminada cerrando la conexión TCP.
A continuacion estan los comandos de LULU válidos en el estado de TRANSACCIÓN:
        
SET|TIMEOUT|valor

    Argumentos: 
        .valor: Valor en segundos de TIMEOUT el cual ocupa 10 bytes. Este valor representa el tiempo en el que se cerrará la sesión después de x tiempo de inactividad. Solo se permiten números enteros positivos mayores a 600 (10 minutos’).

    Restricciones:
        Solo puede ser dado en el estado de TRANSACCIÓN
            
    Descripción: 

    Respuestas posibles:
        +OK

    Ejemplo:
        C:	SET|TIMEOUT|10
        S:	+OK 


GET|TIMEOUT
            
    Argumentos: ninguno
            
    Restricciones:
        Solo puede ser dado en el estado de TRANSACCIÓN
            
    Descripción: 
        El servidor LULU da una respuesta positiva con una línea conteniendo el valor del tiempo en segundos utilizado para cerrar una sesión en caso de este tiempo de inactividad.
        Para simplificar el parseo, todos los servidores LULU utilizan un formato. La respuesta positiva consiste en un “+OK” seguido por un único “|” (pipe), seguido por el valor de segundos de timeout, con un máximo de tamaño de 10 caracteres, terminado con el par CRLF.
            
    Respuestas posibles:
        +seconds

    Ejemplo:
        C:	GET|TIMEOUT
        S:	+600

        
SET|BUFFERSIZE|valor

    Argumentos: 
        El tamaño del buffer que quiere ser utilizado en el server LULU para recibir mensajes.Solo se permiten números enteros positivos mayores a cero, con un máximo de 10 caracteres.
            
    Restricciones:
        Solo puede ser dado en el estado de TRANSACCIÓN
            
    Descripción: 
        Para simplificar el parseo, todos los servidores LULU utilizan un formato. La respuesta positiva consiste en un “+OK”. La respuesta negativa consiste en un “-INVALID BUFFERSIZE”, terminado con el par CRLF.
            
    Respuestas posibles:
        +OK
        -INVALID BUFFER SIZE

    Ejemplo:
        C:	SET|BUFFERSIZE|100
        S:	+OK
        …
        C:	SET|BUFFERSIZE|0
        S:	-INVALID BUFFER SIZE
        

GET|BUFFERSIZE

    Argumentos: ninguno
            
    Restricciones:
        Solo puede ser dado en el estado de TRANSACCIÓN
            
    Descripción: 
        Para simplificar el parseo, todos los servidores LULU utilizan un formato. La respuesta positiva consiste en un “+”, seguido de una línea con un máximo de 10 caracteres mostrando el tamaño del buffer utilizado en el servidor LULU, terminado con el par CRLF.
            
    Respuestas posibles:
        +buffersize

    Ejemplo:
        C:	GET|BUFFERSIZE
        S:	+100

        
GET|METRICS

    Argumentos: ninguno
            
    Restricciones:
        Solo puede ser dado en el estado de TRANSACCIÓN
            
    Descripción: 
        Para simplificar el parseo, todos los servidores LULU utilizan un formato. La respuesta positiva consiste en un “+”, seguido por un valor entero positivo mostrando la cantidad de conexiones históricas, luego “|”(pipe) seguido por un valor entero positivo representando la cantidad de conexiones concurrentes, luego “|” seguido por la cantidad de bytes transferidos, terminado con el par CRLF.
            
    Respuestas posibles:
        +conexiones_historicas|conexiones_concurrentes|bytes_transferidos

    Ejemplo:
        C:	GET|METRICS
        S:	+45|2|200

SET|USER|username|password
            
    Argumentos: 
        .Username: Nombre de usuario con un máximo de 255 caracteres de longitud y un minimo de 1 caracter de longitud. No puede contener los siguientes caracteres: el caracter ASCII 214 (“|”); el par CRLF contiguos ( ASCII 13 seguido por ASCII 10).
        .Password: contraseña de usuario con un máximo de 255 caracteres de longitud  y un mínimo de 1 carácter de longitud. No puede contener los siguientes caracteres: el caracter ASCII 214 (“|”); el par CRLF contiguos ( ASCII 13 seguido por ASCII 10).
            
    Restricciones:
        Solo puede ser dado en el estado de TRANSACCIÓN
            
    Descripción: 
        Para simplificar el parseo, todos los servidores LULU utilizan un formato. La respuesta positiva consiste en un “+”, seguido por un valor entero positivo mostrando la cantidad de conexiones históricas, luego “|”(pipe) seguido por un valor entero positivo representando la cantidad de conexiones concurrentes, luego “|” seguido por la cantidad de bytes transferidos, terminado con el par CRLF.
            
    Respuestas posibles:
        +OK
        -USER EXISTS

    Ejemplo:
        C:	SET|USER|user1|pass
        S:	+OK
        …
        C:	SET|USER|user1|pass
        S:	-USER EXISTS


LIST|USERS

    Argumentos: ninguno
            
    Restricciones:
        Solo puede ser dado en el estado de TRANSACCIÓN
            
    Descripción: 
        Para simplificar el parseo, todos los servidores LULU utilizan un formato. La respuesta positiva consiste en un “+” seguido por el nombre de todos los usuarios activos separados por “|” (pipe), terminando con el par CRLF.
            
    Respuestas posibles:
        +user1|user2|...|userN

    Ejemplo:
        C:	LIST|USERS
        S:	+user1|user2|...|lucia|lucas


DEL|USER|username
    
    Argumentos: 
        Username: Nombre de usuario el cual se quiere remover con un máximo de 255 caracteres de longitud y un mínimo de 1 caracter de longitud. No puede contener los siguientes caracteres: el caracter ASCII 214 (“|”); el par CRLF consecutivos (ASCII 13 (CR), ASCII 10(LF)) .
            
    Restricciones:
         Solo puede ser dado en el estado de TRANSACCIÓN
            
    Descripción: 
        Para simplificar el parseo, todos los servidores LULU utilizan un formato. La respuesta positiva consiste en un “+OK”, terminado con el par CRLF. Si el nombre de usuario no existe entonces el servidor responde con una respuesta negativa que consiste en “-USER DOESNT EXIST”
            
    Respuestas posibles:
        +OK
        -USER DOESNT EXIST

    Ejemplo:
        C:	DEL|USER|user1
        S:	+OK
        …
        C:	DEL|USER|user11
        S:	-USER DOESNT EXIST

### Resumen de comandos LULU
#### Válidos en el estado de AUTORIZACIÓN
    GOODBYE			
    HELLO|username|password

#### Válidos en el estado de TRANSACCIÓN
    GOODBYE				
    SET|TIMEOUT|valor
    GET|TIMEOUT
    SET|BUFFERSIZE|valor
    GET|BUFFERSIZE
    GET|METRICS
    SET|USER|username|password
    LIST|USERS
    DEL|USER|username
### Respuesta LULU:
    +
    -

Notar que a excepcion de GET|TIMEOUT, GET|BUFFERSIZE, GET|METRICS y LIST|USERS la respuesta dada por el servidor LULU a cualquier comando solo es realmente importante hasta “-” y “+”. Todo texto que sigue luego de esta respuesta puede ser ignorado por el cliente.

### Ejemplo de sesión en LULU
    S:	<waiting for connection on TCP port 8080>
    C:	<open connection>
    C:	HELLO|lucia|luciapass
    S:	+OK
    C:	GET|TIMEOUT
    S:	+600
    C:	SET|TIMEOUT|1800
    S:	+OK
    C:	LIST|USERS
    S:	+lucia|lucas|juan
    C:	DEL|USER|juan
    S:	+OK
    C:	GOODBYE
    S:	+GOODBYE
    C:	<close connection>
    S:	<wait for next connection>





