/**
 *   UCR-ECCI
 *   CI-0123 Proyecto integrador de redes y sistemas operativos
 *
 *   Socket client/server example
 *
 **/

#include <stdio.h>

#include <iostream>

#include "./network/Socket.h"

#define PORT 4657
#define BUFSIZE 512

int main(int argc, char** argv) {
  VSocket* s = new Socket('s');  // Crea un socket de IPv4, tipo "stream"
  s->SetTimeout(5);              // Set timeout to 5 seconds
  char* request = (char*)"GET /";
  char buffer[BUFSIZE];

  // s.Connect( "172.16.123.80", PORT );
  s->Connect((char*)"172.16.123.84",
             PORT);  // Same port as server // TODO: CHANGE TEST_SUBNET

  if (argc > 1) {
    sprintf(request, "GET /%s HTTP/1.1\r\n\r\n", argv[1]);
    s->Write(argv[1]);  // Send first program argument to server
  } else {
    s->Write("GET /camello HTTP/1.1\r\n\r\n");
  }

  int st = s->Read(buffer, BUFSIZE);  // Read answer sent back from server
  if (st == -1) {
    std::cout << "Client timeout" << std::endl;
  }

  printf("%s", buffer);  // Print received string
}
