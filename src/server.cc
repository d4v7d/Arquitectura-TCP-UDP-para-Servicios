/**
 *   UCR-ECCI
 *   CI-0123 Proyecto integrador de redes y sistemas operativos
 *
 *   Socket client/server example
 *
 **/

#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>  // memset
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <csignal>  // Add this line to include the csignal header
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "./network/Socket.h"
#include "./network/VSocket.h"
#include "./objects/JSONDecoder.h"
#include "./objects/Lego.h"

#define TCP_PORT_LISTEN_INTERM 2560
#define UDP_PORT_LISTEN_INTERM 2561
#define RESPONSE_BUFF_SIZE 1024
#define BUFSIZE 10000

using namespace std;

struct shared_data_inter {
  std::string inter_ip;
  int test_case;
};

void send_broadcast(int port, char* message, char* subnet);

string formatRequest(const char* request) {
  // Buscar el inicio de la ruta después de "GET /"
  const char* start = std::strstr(request, "GET /");
  if (start == nullptr) {
    return request;  // Si no se encuentra "GET /", retornar una cadena vacía
  }
  start += 5;  // Avanzar 5 caracteres para omitir "GET /"

  // Buscar el espacio siguiente
  const char* end = std::strchr(start, ' ');
  if (end == nullptr) {
    return request;  // Si no se encuentra el espacio, retornar una cadena vacía
  }

  // Calcular la longitud de la parte de la ruta
  std::size_t length = end - start;

  // Crear un string a partir de la parte de la ruta
  std::string resource(start, length);

  return resource;
}

void responseHTML(Lego lego, char* a) {
  // Transformar la instancia a HTML
  string html = lego.generateHTML();

  // Guardar el HTML en el buffer a devolver
  std::size_t length = html.copy(a, BUFSIZE - 1);
  a[length] = '\0';
}

void errorHTML(char* a) {
  stringstream ss;
  ss << "HTTP/1.1 404 Not Found\n";
  ss << "Content-Type: text/html\r\n\r\n";

  ss << "<html>\n";
  ss << "<head>\n";
  ss << "<title>" << "Not Found!" << "</title>\n";
  ss << "</head>\n";
  ss << "<body>\n";
  ss << "<h1>" << "404 - Not Found" << "</h1>\n";
  ss << "<h2>" << "Lego no encontrado" << "</h2>\n";

  ss << "</body>\n";
  ss << "</html>\n";

  std::size_t length = ss.str().copy(a, BUFSIZE - 1);
  a[length] = '\0';
}

void send_broadcast(int port, char* message, char* subnet) {
  VSocket* udp_s;

  // Create a UDP socket
  udp_s = new Socket('d');
  udp_s->subnet = subnet;
  // udp_s->Bind(port);  // Port to access this mirror server

  udp_s->Broadcast(message, strlen(message));
  udp_s->Close();
  delete udp_s;
}

/*
 * Método para escuchar al servidor intermedio
 */

void* listen_intermediate(void* shared_data) {
  VSocket *tcp_s, *interm_s;

  // Create a TCP socket
  tcp_s = new Socket('s');
  // tcp_s->SetTimeout(5);
  std::cout << "Listening to pieces server TCP" << std::endl;

  tcp_s->Bind(TCP_PORT_LISTEN_INTERM);  // Port to access this mirror server
  tcp_s->Listen(5);

  for (;;) {
    char a[BUFSIZE];
    memset(a, 0, BUFSIZE);
    
    interm_s = tcp_s->Accept();  // Wait for a new conection, conection info is
                                 // in interm_s variable

    // tcp_s->Close();  // Close original socket "tcp_s" in child
    interm_s->Read(a, BUFSIZE);  // Read a string from client using new conection info
    std::cout << "Server received: " << a << std::endl;

    // LECTURA DEL ARCHIVO JSON
    std::string decoded_req = formatRequest(a);
    // JSONDecoder decoder("./" + formatRequest(a) + ".json");
    cout << "request: " << decoded_req << ".json" << endl;
    JSONDecoder decoder("../" + decoded_req + ".json");

    if (decoder.decode()) {
      // Crear el Lego a partir del Json
      Lego lego = decoder.getLegoFromJson();
      responseHTML(lego, a);
    } else {
      // Retornar html de error en 'a'
      errorHTML(a);
    }

    interm_s->Write(a);  // Write it back to client, this is the mirror function

    interm_s->Close();  // Close socket interm_s in parent, then go wait for a
                        // new conection
  }
}

void* listen_intermediate_broadcast(void* shared_data) {
  std::cout << "Listening to intermediate server UDP" << std::endl;
  char* ip_addr = (char*)"172.16.123.95";

  VSocket *udp_s, *interm_s;

  // Create a UDP socket
  udp_s = new Socket('d');
  udp_s->SetTimeout(5);
  udp_s->subnet = ip_addr;

  udp_s->Bind(UDP_PORT_LISTEN_INTERM);  // Port to access this mirror server
  
  for (;;) {
    // TODO: Cuando recibe
    char ps_buff[RESPONSE_BUFF_SIZE];
    char* addr_src;

    addr_src = udp_s->listenBroadcast(ps_buff, RESPONSE_BUFF_SIZE);

    if (strcmp(ps_buff, "T") == 0 &&
        ((shared_data_inter*)shared_data)->inter_ip != "") {
      send_broadcast(UDP_PORT_LISTEN_INTERM, (char*)"S",
                     ip_addr);
    }

    struct sockaddr* srcAddr = (struct sockaddr*)(addr_src);

    std::cout << "PS_BUFF_CONTENT_UDP: " << ps_buff << std::endl;  // DEBUG TEMP

    // TODO: Guarda el shared_data

    if (strcmp(addr_src, ip_addr) != 0) {
      ((shared_data_inter*)shared_data)->inter_ip = addr_src;
    }

    std::cout << "Share data: " << ((shared_data_inter*)shared_data)->inter_ip
              << std::endl;  // DEBUG TEMP

    // TODO: Responde

    delete[] addr_src;
  }
}

void enviarSignal(int signum) {
  // Envia mensaje de eutanasia
  std::cout << "signal, eutanacia" << std::endl;
  send_broadcast(UDP_PORT_LISTEN_INTERM, (char*)"E", (char*)"172.16.123.95");

  // Terminar el programa
  exit(signum);
}

int main(int argc, char** argv) {
  VSocket* udp_s;
  int childpid;
  signal(SIGINT, enviarSignal);

  // Create a UDP socket
  char* broadcast = (char*)"T";

  send_broadcast(UDP_PORT_LISTEN_INTERM, broadcast, (char*)"172.16.123.95");

  pthread_t thread_inter_udp;
  pthread_t thread_inter_tcp;

  shared_data_inter* shared_data = new shared_data_inter();
  shared_data->inter_ip = "";
  shared_data->test_case = 0;

  pthread_create(&thread_inter_udp, NULL, listen_intermediate_broadcast,
                 (void*)shared_data);

  pthread_create(&thread_inter_tcp, NULL, listen_intermediate,
                 (void*)shared_data);

  std::cout << "Termina el servidor de piezas: " << std::endl;

  for (;;) {
    // TODO: Acepta la conexión

    // TODO: Genera un hilo para atender la consulta
  }
}