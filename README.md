# HOLA

En este repositorio se encuentra el TPE de la materia Protocolos de Comunicación realizado por el grupo 11.

Los autores son:

- **Torrusio, Lucia**
- **Dell’Isola, Lucas** 58025

Este trabajo practico consiste en la implementación de un servidor Socks5 junto al diseño e implementación de un protocolo propio para poder manejar a dicho servidor.

## Estructura del Repositorio

```
.
├── README.md
├── reference
│   ├── Lulu Protocol.pdf
│   ├── rfc1928.pdf
│   ├── rfc1929.pdf
│   └── Report.pdf
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





















