/**
 *  Establece la definición de la clase Socket para efectuar la comunicación
 *  de procesos que no comparten memoria, utilizando un esquema de memoria
 *  distribuida.  El desarrollo de esta clase se hará en varias etapas, primero
 *  los métodos necesarios para los clientes, en la otras etapas los métodos
 *para el servidor, manejo de IP-v6, conexiones seguras y otros
 *
 *  Universidad de Costa Rica
 *  ECCI
 *  CI0123 Proyecto integrador de redes y sistemas operativos
 *  2023-ii
 *  Grupos: 2
 *
 **/

#include <arpa/inet.h>  // ntohs
#include <sys/socket.h>
#include <unistd.h>  // close

#include <cstddef>
#include <cstdio>
#include <cstring>  // memset
#include <stdexcept>
// #include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>  // getaddrinfo, freeaddrinfo

#include <iostream>

#include "VSocket.h"

/**
 *  Class initializer
 *     use Unix socket system call
 *
 *  @param     char t: socket type to define
 *     's' for stream
 *     'd' for datagram
 *  @param     bool ipv6: if we need a IPv6 socket
 *
 **/
void VSocket::InitVSocket(char t, bool IPv6) {
  int domain = IPv6 ? AF_INET6 : AF_INET;
  int type = SOCK_STREAM;
  this->IPv6 = IPv6;

  // inicializar el socket
  if (t == 'd') {
    type = SOCK_DGRAM;
  }

  this->idSocket = socket(domain, type, 0);

  if (idSocket == -1) {
    throw std::runtime_error("VSocket::InitVSocket, error al crear el socket");
  }
}

void VSocket::SetTimeout(int seconds) {
  this->timeout = true;
  struct timeval timeout;
  timeout.tv_sec = seconds;
  timeout.tv_usec = 0;

  // Set the receive timeout
  if (setsockopt(this->idSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                 sizeof(timeout)) < 0) {
    perror("Error setting socket receive timeout");
  }

  // Set the send timeout
  if (setsockopt(this->idSocket, SOL_SOCKET, SO_SNDTIMEO, &timeout,
                 sizeof(timeout)) < 0) {
    perror("Error setting socket send timeout");
  }
}

/**
 *  Class initializer
 *
 *  @param     int descriptor: socket descriptor for an already opened socket
 *
 **/
void VSocket::InitVSocket(int descriptor) { this->idSocket = descriptor; }

/**
 * Class destructor
 *
 **/
VSocket::~VSocket() { this->Close(); }

/**
 * Close method
 *    use Unix close system call (once opened a socket is managed like a file in
 *Unix)
 *
 **/
void VSocket::Close() {
  int st;

  close(this->idSocket);

  if (-1 == st) {
    throw std::runtime_error("Socket::Close()");
  }
}

/**
 * DoConnect method
 *   use "connect" Unix system call
 *
 * @param      char * host: host address in dot notation, example "10.1.104.187"
 * @param      int port: process address, example 80
 *
 **/
int VSocket::DoConnect(const char *hostip, int port) {
  int st = 0, len;
  this->port = port;

  if (this->IPv6) {
    struct sockaddr_in6 host6;
    struct sockaddr *host_addr;
    struct hostent *server;

    server = gethostbyname2(hostip, AF_INET6);
    if (server == nullptr) {
      throw std::runtime_error("Socket::DoConnect: ERROR, invalid host");
    }

    memset(&host6, 0, sizeof(host6));
    host6.sin6_family = AF_INET6;
    host6.sin6_port = htons(port);
    memcpy(&host6.sin6_addr, server->h_addr, server->h_length);

    // st = inet_pton(AF_INET6, hostip, &host6.sin6_addr);

    // if (st == 0) { // 0 means invalid address, -1 means address error
    //    throw std::runtime_error("Socket::DoConnect: ERROR, invalid address");
    // } else if (st == -1) { // 0 means invalid address, -1 means address error
    //    throw std::runtime_error("Socket::DoConnect: ERROR, address error");
    // }

    host_addr = (struct sockaddr *)&host6;
    len = sizeof(host6);
    st = connect(this->idSocket, host_addr, len);

    if (st == -1) {
      throw std::runtime_error(
          "Socket::DoConnect( const char *, int ) [connect]");
    }

  } else {
    // conectar el socket al servidor:
    struct sockaddr_in host4;
    memset((char *)&host4, 0, sizeof(host4));
    host4.sin_family = AF_INET;
    st = inet_pton(AF_INET, hostip, &host4.sin_addr);

    if (-1 == st) {
      throw std::runtime_error("VSocket::DoConnect post inet_pton");
    }

    host4.sin_port = htons(port);
    std::cout << port << " <- port socket ->" << idSocket << std::endl;
    st = connect(this->idSocket, (struct sockaddr *)&host4, sizeof(host4));

    if (-1 == st) {
      throw std::runtime_error("VSocket::DoConnect post connect");
    }
  }

  return st;
}

/**
 * DoConnect method
 *   use "connect" Unix system call
 *
 * @param      char * host: host address in dns notation, example
 *"os.ecci.ucr.ac.cr"
 * @param      char * service: process address, example "http"
 *
 **/
int VSocket::DoConnect(const char *host, const char *service) {
  int st;
  struct addrinfo hints, *result, *rp;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;     /* Allow IPv4 or IPv6 */
  hints.ai_socktype = SOCK_STREAM; /* Stream socket */
  hints.ai_flags = 0;
  hints.ai_protocol = 0; /* Any protocol */

  st = getaddrinfo(host, service, &hints, &result);

  for (rp = result; rp != NULL; rp = rp->ai_next) {
    st = connect(idSocket, rp->ai_addr, rp->ai_addrlen);
    if (0 == st) break;
  }

  freeaddrinfo(result);
  return st;
}

/**
 * Listen method
 *
 * @param      int queue: max pending connections to enqueue (server mode)
 *
 *  This method define how many elements can wait in queue
 *
 **/
int VSocket::Listen(int queue) {
  int st = listen(this->idSocket, queue);

  if (-1 == st) {
    throw std::runtime_error("VSocket::Listen( int )");
  }

  return st;
}

/**
 * Bind method
 *    use "bind" Unix system call (man 3 bind) (server mode)
 *
 * @param      int port: bind a unamed socket to a port defined in sockaddr
 *structure
 *
 *  Links the calling process to a service at port
 *
 **/
int VSocket::Bind(int port) {
  int st;

  if (this->IPv6) {
    struct sockaddr_in6 host6;
    memset(&host6, 0, sizeof(host6));
    host6.sin6_family = AF_INET6;
    host6.sin6_port = htons(port);
    host6.sin6_addr = in6addr_any;

    st = bind(this->idSocket, (struct sockaddr *)&host6, sizeof(host6));
  } else {
    struct sockaddr_in host4;
    memset(&host4, 0, sizeof(host4));
    host4.sin_family = AF_INET;
    host4.sin_addr.s_addr = htonl(INADDR_ANY);
    host4.sin_port = htons(port);

    st = bind(this->idSocket, (struct sockaddr *)&host4, sizeof(host4));
  }

  this->port = port;

  if (-1 == st) {
    throw std::runtime_error("Error al realizar el enlace");
  }

  return st;
}

/**
 * DoAccept method
 *    use "accept" Unix system call (man 3 accept) (server mode)
 *
 *  @returns   a new class instance
 *
 *  Waits for a new connection to service (TCP mode: stream)
 *
 **/
int VSocket::DoAccept() {
  struct sockaddr_in addr;
  addr.sin_family = this->ip_family;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  socklen_t srcaddrlenm = (socklen_t)sizeof(addr);

  int st = accept(this->idSocket, (struct sockaddr *)&addr, &srcaddrlenm);

  if (-1 == st) {
    throw std::runtime_error("VSocket::DoAccept()");
  }

  return st;
}

/**
 * Shutdown method
 *    use "shutdown" Unix system call (man 3 shutdown)
 *
 *  @param	int mode define how to cease socket operation
 *
 *  Partial close the connection (TCP mode)
 *
 **/
int VSocket::Shutdown(int mode) {
  int st = shutdown(this->idSocket, mode);

  if (-1 == st) {
    throw std::runtime_error("VSocket::Shutdown( int )");
  }

  return st;
}

/**
 *  sendTo method
 *
 *  @param	const void * buffer: data to send
 *  @param	size_t size data size to send
 *  @param	void * addr address to send data
 *
 *  Send data to another network point (addr) without connection (Datagram)
 *
 **/
size_t VSocket::sendTo(const void *buffer, size_t size, void *addr) {
  int st;

  if (this->IPv6) {
    struct sockaddr *destAddr = (struct sockaddr *)(addr);
    st = sendto(this->idSocket, buffer, size, 0, destAddr,
                sizeof(struct sockaddr_in6));
  } else {
    struct sockaddr *destAddr = (struct sockaddr *)(addr);
    st = sendto(this->idSocket, buffer, size, 0, destAddr, sizeof(*destAddr));
  }

  if (st == -1) {
    // printf("Error: %s (%d)\n", strerror(errno), errno);
    throw std::runtime_error("Error al enviar el mensaje");
  }

  return st;
}

/**
 *  recvFrom method
 *
 *  @param	const void * buffer: data to send
 *  @param	size_t size data size to send
 *  @param	void * addr address to receive from data
 *
 *  @return	size_t bytes received
 *
 *  Receive data from another network point (addr) without connection (Datagram)
 *
 **/
size_t VSocket::recvFrom(void *buffer, size_t size, void *addr) {
  int st;

  struct sockaddr *srcAddr = (struct sockaddr *)(addr);
  socklen_t srcAddrLen = (socklen_t)sizeof(*srcAddr);
  st = recvfrom(this->idSocket, buffer, size, 0, srcAddr, &srcAddrLen);

  if (st == -1) {
    throw std::runtime_error("Error al recibir el mensaje");
  }

  return st;
}

char* VSocket::listenBroadcast(void *buffer, size_t size) {

  size_t st = 0;

  struct sockaddr_in host4;
  memset((char *)&host4, 0, sizeof(host4));
  host4.sin_family = AF_INET;
  host4.sin_port = htons(this->port);
  host4.sin_addr.s_addr = inet_addr(this->subnet);

  socklen_t sock_len = (socklen_t)sizeof(struct sockaddr_in);

  st = recvfrom(this->idSocket, buffer, size, 0, (sockaddr *)&host4, &sock_len);

  if (0 > st) {
    throw std::runtime_error("Socket::listenBroadcast( void *, size_t ): recvfrom()");
  }

  //char senderIP[INET_ADDRSTRLEN];

  char * senderIP = new char[INET_ADDRSTRLEN];

  inet_ntop(AF_INET, &host4.sin_addr, senderIP, INET_ADDRSTRLEN);

  std::cout << "IP SENDER: " << senderIP << std::endl;

  return (char*)senderIP;
}
