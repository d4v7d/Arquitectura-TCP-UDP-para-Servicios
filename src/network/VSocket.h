/**
 *  Universidad de Costa Rica
 *  ECCI
 *  CI0123 Proyecto integrador de redes y sistemas operativos
 *  2023-ii
 *  Grupos: 2 y 3
 *
 * (versión Fedora)
 *
 **/

#ifndef VSocket_h
#define VSocket_h

#include <cstddef>

class VSocket {
 public:
  void InitVSocket(char, bool = false);
  void InitVSocket(int);
  ~VSocket();

  void Close();
  int DoConnect(const char *, int);
  int DoConnect(const char *, const char *);
  virtual int Connect(const char *, int) = 0;
  virtual int Connect(const char *, const char *) = 0;

  virtual size_t Read(void *, size_t) = 0;
  virtual size_t Write(const void *, size_t) = 0;
  virtual size_t Write(const char *) = 0;
  virtual size_t Broadcast(char *, size_t) = 0;
  void SetTimeout(int seconds);

  int Listen(int);
  int Bind(int);
  int DoAccept();
  virtual VSocket *Accept() = 0;

  int Shutdown(int);

  size_t sendTo(const void *, size_t, void *);
  size_t recvFrom(void *, size_t, void *);
  char* listenBroadcast(void *, size_t);

  char *subnet;

 protected:
  int idSocket;
  bool IPv6;
  int ip_family;
  int port;
  bool timeout;
};

#endif  // VSocket_h
