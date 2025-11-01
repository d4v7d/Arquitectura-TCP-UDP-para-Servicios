/**
 *   CI0123 PIRO
 *   Clase para utilizar los "sockets" en Linux
 *
 **/

#include "Socket.h"

#include <arpa/inet.h>  // for inet_pton
#include <stdio.h>      // for perror
#include <stdlib.h>     // for exit
#include <string.h>     // for memset
#include <sys/socket.h>
#include <sys/types.h>  // for connect
#include <unistd.h>     // for close

#include <iostream>

/**
 *  Class constructor
 *     use Unix socket system call
 *
 *  @param	char type: socket type to define
 *     's' for stream
 *     'd' for datagram
 *  @param	bool ipv6: if we need a IPv6 socket
 *
 **/
Socket::Socket(char type, bool IPv6) { this->InitVSocket(type, IPv6); }

/**
 *  Class constructor
 *
 *  @param     int id: socket descriptor
 *
 **/
Socket::Socket(int id) { this->InitVSocket(id); }

/**
 * Class destructor
 *
 **/
Socket::~Socket() {
  // Close();
}

/**
 * Close method
 *    use Unix close system call (once opened a socket is managed like a file in
 *Unix)
 *
 **/
void Socket::Close() { close(this->idSocket); }

/**
 * Connect method
 *   use "connect" Unix system call
 *
 * @param	char * host: host address in dot notation, example
 *"10.1.104.187"
 * @param	int port: process address, example 80
 *
 **/
int Socket::Connect(const char *host, int port) {
  return this->DoConnect(host, port);
}

/**
 * Connect method
 *   use "connect" Unix system call
 *
 * @param	char * host: host address in dot notation, example
 *"10.1.104.187"
 * @param  char * prefix: http or https
 *
 **/
int Socket::Connect(const char *host, const char *prefix) {
  if (strcmp("http", prefix)) {
    this->port = 80;
  } else {
    if (strcmp("https", prefix)) {
      this->port = 443;
    }
  }

  return this->DoConnect(host, prefix);
}

/**
 * Read method
 *   use "read" Unix system call (man 3 read)
 *
 * @param	void * text: buffer to store data read from socket
 * @param	int size: buffer capacity, read will stop if buffer is full
 *
 **/
size_t Socket::Read(void *text, size_t size) {
  ssize_t st;

  st = read(this->idSocket, (void *)text, size);

  if (!this->timeout && -1 == st) {
    throw std::runtime_error("Socket::Read( const void *, size_t )");
  }

  return st;
}

/**
 * Write method
 *   use "write" Unix system call (man 3 write)
 *
 * @param	void * buffer: buffer to store data write to socket
 * @param	size_t size: buffer capacity, number of bytes to write
 *
 **/
size_t Socket::Write(const void *text, size_t size) {
  ssize_t st;

  st = write(this->idSocket, text, size);

  if (-1 == st) {
    throw std::runtime_error("Socket::Write( void *, size_t )");
  }

  return st;
}

/**
 * Write method
 *
 * @param	char * text: string to store data write to socket
 *
 *  This method write a string to socket, use strlen to determine how many bytes
 *
 **/
size_t Socket::Write(const char *text) {
  ssize_t st;

  st = write(this->idSocket, text, strlen(text));

  if (-1 == st) {
    throw std::runtime_error("Socket::Write( void *, size_t )");
  }

  // std::cout<<"\nwrite_st: "<<st<<std::endl;

  return st;
}

/**
 * Broadcast method
 *    use base class to send a message in a broadcast way
 *
 *  @returns   a operation status
 *
 *
 **/
size_t Socket::Broadcast(char *message, size_t len) {
  size_t st;

  std::cout << "port: " << this->port << std::endl;
  std::cout << "subnet: " << this->subnet << std::endl;

  struct sockaddr_in host4;
  memset((char *)&host4, 0, sizeof(host4));
  host4.sin_family = AF_INET;
  //host4.sin_port = htons(this->port);
  host4.sin_port = htons(2561);
  host4.sin_addr.s_addr = inet_addr(this->subnet);

  //char broadcast = '1';
  int broadcast = 1;

  st = setsockopt(this->idSocket, SOL_SOCKET, SO_BROADCAST, &broadcast,
                  sizeof(broadcast));

  if (0 > st) {
    throw std::runtime_error(
        "Socket::Broadcast( void *, size_t ): setsockopt()");
  }

  std::cout << "Mensaje: " << message << std::endl;
  std::cout << "Len: " << len << std::endl;

  // Envía el mensaje
  st = sendto(this->idSocket, message, len, 0, (sockaddr *)&host4, sizeof(host4));

  if (0 > st) {
    throw std::runtime_error("Socket::Broadcast( void *, size_t ): sendto()");
  }

  return st;
}

/**
 * Accept method
 *    use base class to accept connections
 *
 *  @returns   a new class instance
 *
 *  Waits for a new connection to service (TCP mode: stream)
 *
 **/
Socket *Socket::Accept() {
  int id;
  Socket *other;

  id = this->DoAccept();

  other = new Socket(id);

  return other;
}