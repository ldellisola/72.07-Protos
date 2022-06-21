# SOCKS5D

En este repositorio se encuentra el TPE de la materia Protocolos de Comunicación realizado por el grupo 11.

Los autores son:

- **Torrusio, Lucia** 59489
- **Dell’Isola, Lucas** 58025

Este trabajo practico consiste en la implementación de un servidor Socks5 junto al diseño e implementación de un protocolo propio para poder manejar a dicho servidor.

## Estructura del Repositorio

```
.
├── README.md
├── reference
│   ├── Lulu.txt
│   ├── rfc1928.pdf
│   ├── rfc1929.pdf
│   ├── client.8
│   ├── socks5d.8
│   └── informe.pdf
├── scripts
│   ├── Dockerfile
│   └── StressTest
└── socks5d
    ├── cmake_scripts
    ├── include
    ├── lib
    ├── src
    │   ├── server
    │   └── lulu_client
    └── tests
```

Donde cada carpeta contiene:

- **reference**: Se guardan los documentos importantes del proyecto, como la definición de nuestro protocolo *“Lulu”*, los RFCs necesarios para implementar el protocolo Socks5 y la autenticación por usuario y contraseña, y por ultimo el informe del trabajo practico.
- **scripts**: Esta carpeta tiene una serie de scripts y programas auxiliares que fueron utiles a la hora de desarrollar y probar nuestra implementación, pero no son necesarios para utilizar el servidor o compilarlo.
  - <u>Dockerfile</u>: Es la definición de una imagen de Docker basada en Ubuntu. Debido a que los miembros del grupo tienen computadoras con distintas plataformas (Windows y MacOS), tuvimos que hacer todo nuestro desarrollo dentro de un container con esta imagen, con la ayuda de Clion.
  - <u>StressTest</u>: Es un programa auxiliar utilizado a la hora de hacer un stress test sobre nuestra implementación del protocolo Socks5. Luego de probar alternativas como JMeter y K6 y no lograr que funcionen correctamente utilizando un proxy Socks5, decidimos implementarlo nosotros mismos. [Acá](scripts/StressTest/README.md) se puede encontrar más información sobre este programa.
- **socks5d**: Aquí se encuentra nuestra implementación tanto del servidor como de nuestro cliente para el protocolo Lulu.
  - <u>cmake_scripts</u>: Almacena una serie de scripts utiles a la hora de correr `CMAKE`. Entre ellos es uno que nos permite encontrar las librerías de `Check` para poder ejecutar los tests unitarios.
  - <u>include</u>: Tiene todos los archivos `.h` del proyecto, ordenados en la misma estructura que sus correspondientes archivos `.c`.
  - <u>lib</u>: Contiene todos los módulos de creación propia necesarios para el funcionamiento de los ejecutables. Entre estos encontramos nuestra capa de abstracción sobre `TCP` y `SOCKS5`.  
  - <u>src</u>: Contiene dos carpetas con el código necesario para generar los ejecutables del servidor y del cliente.
  - <u>tests</u>: Contiene todos los tests unitarios, junto a sus runners.

## Compilación

Para compilar tanto el cliente como el servidor es necesario tener instalado `CMAKE`:

```bash
$ apt-get install cmake
```

Para poder compilar el programa hay que seguir los siguientes pasos:

1. Clono al repositorio y entro a la carpeta:

   ```bash
   $ git clone https://github.com/ldellisola/socks5d.git
   $ cd socks5d
   ```

2. Entro a la carpeta del codigo fuente e inicializo el proyecto CMake

   ```bash
   $ cd socks5d
   $ cmake .
   ```

3. Ahora puedo compilar ambos programas:

   - Para compilar el servidor:

     ```bash
     $ cd src/server
     $ cmake --build .
     ```

   - Para compilar el cliente:

     ```bash
     $ cd src/lulu_client
     $ cmake --build .
     ```

## Ejecución  

Para conocer los comandos del servidor, se puede utilizar al archivo man `socks5d.8`. Junto a los comandos pedidos por la cátedra, agregamos dos más:

- `-t`: Indica el timeout en segundos
- `-b`: Indica el tamaño del buffer

Un ejemplo de ejecución seria:

```bash
$ ./socks5d -t 50 -b 500 -p 9000
```

Ademas, con la variable en environment `SOCKS5D_LOG_LEVEL` se puede elegir el nivel de log:

- `DEBUG`: Logs necesarios para desarrollar el programa e identificar problemas
- `INFO`: Logs orientados al usuario, con información util del servidor
- `WARN`: Logs que indican posibles errores si estos no son manejados correctamente.
- `ERROR`: Logs qué indican errores certeros que finalizan una conexión.
- `FATAL`: Logs qué indican errores certeros finalizan la ejecución del programa.

## Tests

### Tests Unitarios

Si se quieren ejecutar los tests, se deberá instalar la librería `check`:

```bash
$ apt-get install check
```

Aunque la ultima version de check funciona sin problemas, nosotros desarrollamos nuestros tests con la version `10.0.0`. 

Debido a las limitaciones de pampero, no se puede instalar esta librería, por lo que por default los tests están desactivados. Para poder activarlos se necesita descomentar las siguientes lineas en el archivo `socks5d/CMakeLists.txt`:

```cmake
# enable_testing()
# add_subdirectory(tests)
```

### Tests de Stress

Los tests de stress se realizan con otro programa diseñado por nosotros.  [Acá](scripts/StressTest/README.md) se puede encontrar más información sobre este programa.

### Tests de Velocidad de Descarga

Para probar los efectos de nuestro servidor socks5 sobre la velocidad de descarga, decidimos utilizar al comando `curl` para descargar una [ISO de Ubuntu 22.04](https://ubuntu.zero.com.ar/ubuntu-releases/jammy/ubuntu-22.04-desktop-amd64.iso):

```bash
$ time curl https://ubuntu.zero.com.ar/ubuntu-releases/jammy/ubuntu-22.04-desktop-amd64.iso \
 -x socks5h://localhost:1080 \
 --output ubuntu.iso
```

Así también sobre un archivo en un servidor http local, para evitar cualquier interferencia externa sobre nuestro test.











