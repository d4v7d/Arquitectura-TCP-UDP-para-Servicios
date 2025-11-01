# Proyecto Integrador de Redes y Sistemas

## Descripción

Este repositorio contiene una implementación educativa de un sistema distribuido de ejemplo diseñado para practicar conceptos de redes y sistemas. El proyecto está dividido en tres roles principales que cooperan mediante protocolos de red:

- Cliente: componente que genera consultas y recibe respuestas (`src/client.cc`).
- Servidor intermedio: recibe peticiones de clientes, las enrutaa o consulta a servidores finales (`src/intermediate.cc`).
- Servidor final / servidor de piezas: almacena datos y responde consultas del intermedio o clientes (`src/server.cc`).

El propósito es demostrar cómo diseñar y probar intercambio de mensajes usando tanto TCP (conexiones fiables orientadas a flujo) como UDP (datagramas y difusión para descubrimiento y señalización).

## Componentes y partes

- Código de aplicación:
    - `src/client.cc` — cliente que inicia peticiones.
    - `src/intermediate.cc` — servidor intermedio que re-distribuye y coordina.
    - `src/server.cc` — servidor de datos/piezas.
- Núcleo de red y utilidades:
    - `src/network/` — implementaciones de `Socket`, `VSocket`, `SSLSocket` y utilidades de lectura/escritura.
    - `objects/` — decodificadores JSON y estructuras de datos auxiliares (`JSONDecoder.cc`, `Lego.cc`).
- Herramientas y ejemplos:
    - `src/miscellaneous/` — clientes y simulación de protocolo (`simulacion-protocolo.cc`, `tcp-cli.cc`, `ipv4-ssl-cli.cc`, etc.).
- Recursos y archivos de apoyo:
    - `files/` — configuraciones de ejemplo, diagramas y formatos de base de datos (`database-*.json`, `diagrama-simulacion-protocolo.drawio`, `isla_5_config.pkt`).
- Scripts de construcción:
    - `Makefile` — targets para compilar y ejecutar los binarios.

## Uso de TCP vs UDP (énfasis)

Este proyecto usa ambos modelos de comunicación según la necesidad:

- TCP (stream, orientado a conexión):
    - Se emplea para intercambio fiable de peticiones y respuestas donde el orden y la integridad de los datos son críticos.
    - Ejemplos de uso:
        - `Cliente ↔ Intermedio`: las peticiones del cliente al servidor intermedio usan TCP para garantizar entrega y reintentos a nivel de conexión. (Ver `src/client.cc`, `src/intermediate.cc`.)
        - `Intermedio ↔ Servidor final`: las consultas entre intermedios y servidores de piezas usan TCP para obtener respuestas completas y consistentes.
    - Ventajas usadas: control de congestión, retransmisiones automáticas, entrega ordenada.

- UDP (datagram, sin conexión) y difusión/broadcast:
    - Se usa para descubrimiento, anuncios y notificaciones ligeras donde no se requiere conexión persistente.
    - Ejemplos de uso:
        - Descubrimiento de servidores o anuncios de presencia mediante broadcast/multicast (rutinas en `src/intermediate.cc` y `src/network/Socket.cc`).
        - Señalización rápida donde la pérdida ocasional de paquetes es tolerable.
    - Ventajas usadas: baja latencia, menor overhead, útil para mensajes cortos y multicast/broadcast.

## Cómo compilar y ejecutar

Este proyecto usa un `Makefile` en la raíz. Desde la carpeta del repositorio se pueden usar los targets definidos allí. Ejemplos:

```powershell
# Compilar todo
make

# Ejecutar binarios de ejemplo (dependen de los targets presentes en el Makefile)
make run-server
make run-intermediate
make run-client
```

Si su entorno no dispone de `make` en Windows, se puede usar WSL o herramientas compatibles con Make, o compilar manualmente con `g++` apuntando a los archivos en `src/`.

## Referencias rápidas en el código

- Broadcast y descubrimiento: funciones en `src/intermediate.cc`, implementación de `Socket::Broadcast` en `src/network/Socket.cc`.
- Conexión y lectura TCP: `VSocket::DoConnect` y `Socket::Read` en `src/network/`.
- Ejemplo de flujo cliente–intermedio–final: `src/miscellaneous/simulacion-protocolo.cc`.

## Mantenimiento y siguientes pasos

- Añadir pruebas automatizadas y scripts de integración en `src/miscellaneous/`.
- Documentar las APIs internas y el formato de mensajes (por ejemplo, formatos JSON usados en `files/database-*.json`).
- Mejorar los ejemplos de despliegue y añadir instrucciones para entornos Windows/WSL.

---

### Integrantes

    David Gonzalez Villanueva - C13388
    Sebastian Bolaños Serrano - C11142
    Julián Gonzalez Betancur - B22877

**Notas:** el proyecto puede necesitar `-lssl -lcrypto` para compilaciones que utilicen `SSLSocket`.
