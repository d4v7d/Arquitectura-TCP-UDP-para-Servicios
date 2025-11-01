/**
 *   UCR-ECCI
 *   CI-0123 Proyecto integrador de redes y sistemas operativos
 *
 *   Intermediate server
 *
 **/
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "./network/Socket.h"

#define REQUEST_BUFF_SIZE 64
#define RESPONSE_BUFF_SIZE 1024
#define PORT_LISTEN_PS 2560
#define PORT_LISTEN_CLIENT 4657
#define UDP_PS_PORT 2561
#define LISTEN_CLIENT_QUEUE 50
#define LISTEN_PS_QUEUE 5
#define PORT_LISTEN_INTERMEDIATE 2432

struct shared_data_ps {
  std::string ps_answer;
  std::string ps_menu;
  std::string ps_ip;
  std::vector<std::string> intermediates_ip;
  int test_case;
  std::string request;
};

bool is_valid_request(char* request);
void request_ps(void* shared_data);
void send_broadcast(int port, char* message, char* subnet);

void* listen_client(void* shared_data) {
  std::cout << "Listening to clients" << std::endl;

  // Crea un socket
  VSocket* server_socket;
  VSocket* client_socket;

  server_socket = new Socket('s');

  // Escucha en el puerto PORT_LISTEN_PS
  server_socket->Bind(PORT_LISTEN_CLIENT);  // Port to access this mirror server
  server_socket->Listen(LISTEN_CLIENT_QUEUE);

  for (;;) {
    char request_buff[REQUEST_BUFF_SIZE];
    client_socket = server_socket->Accept();
    client_socket->Read(request_buff, REQUEST_BUFF_SIZE);

    std::cout << "REQUEST_BUFF_CONTENT: " << request_buff
              << std::endl;  // DEBUG TEMP

    if (((shared_data_ps*)shared_data)->test_case == 1) {
      sleep(10);
      continue;
    }

    // Si la petición es inválida, responde con un mensaje de error
    if (strlen(request_buff) == 0 || !is_valid_request(request_buff)) {
      char response_buff[RESPONSE_BUFF_SIZE];
      strcpy(response_buff, "Invalid request :(\\0");
      client_socket->Write(response_buff);
      client_socket->Close();
      continue;
    }

    // Si no hay un servidor de piezas, responde con un mensaje de error
    if (((shared_data_ps*)shared_data)->ps_ip == "" &&
        ((shared_data_ps*)shared_data)->intermediates_ip.size() == 0) {
      char response_buff[RESPONSE_BUFF_SIZE];
      strcpy(response_buff, "ERROR: 500 :(\\0");
      client_socket->Write(response_buff);
      client_socket->Close();
      continue;
    }

    // Si el response buff solo viene el get /, entonces se manda el menú
    if (strcmp(request_buff, "GET /") == 0) {
      char response_buff[RESPONSE_BUFF_SIZE];
      strcpy(response_buff, ((shared_data_ps*)shared_data)->ps_menu.c_str());
      client_socket->Write(response_buff);
      client_socket->Close();
      continue;
    }

    ((shared_data_ps*)shared_data)->request = request_buff;

    // TODO: create piece request

    request_ps(shared_data);

    char response_buff[RESPONSE_BUFF_SIZE];
    strcpy(response_buff, ((shared_data_ps*)shared_data)->ps_answer.c_str());
    client_socket->Write(response_buff);
    client_socket->Close();
  }
}

bool is_valid_request(char* request) {
  if (request[0] != 'G' || request[1] != 'E' || request[2] != 'T' ||
      request[3] != ' ' || request[4] != '/') {
    return false;
    void request_ps(void* shared_data);
  }

  return true;
}

void request_ps(void* shared_data) {
  std::cout << "Listening to pieces server" << std::endl;
  int st;

  // Crea un socket
  VSocket* server_socket;
  VSocket* s_ps;

  s_ps = new Socket('s');
  s_ps->SetTimeout(5);

  server_socket = new Socket('s');
  server_socket->SetTimeout(5);

  s_ps->Connect((char*)"172.16.123.83", PORT_LISTEN_PS);

  // Escucha en el puerto PORT_LISTEN_PS
  //server_socket->Bind(PORT_LISTEN_PS);     // Port to access this mirror server
  //server_socket->Listen(LISTEN_PS_QUEUE);  // Set backlog queue to 1 conections

  char ps_buff[RESPONSE_BUFF_SIZE];

  st = s_ps->Write(((shared_data_ps*)shared_data)->request.c_str());
  if (st == -1) {
    std::cout << "Pieces server timeout" << std::endl;
    strcpy(ps_buff, "ERROR: 504 :(\\0");
  }

  st = s_ps->Read(ps_buff, RESPONSE_BUFF_SIZE);
  if (st == -1) {
    std::cout << "Pieces server timeout" << std::endl;
    strcpy(ps_buff, "ERROR: 504 :(\\0");
  }

  std::cout << "PS_BUFF_CONTENT: " << ps_buff << std::endl;  // DEBUG TEMP

  ((shared_data_ps*)shared_data)->ps_answer = ps_buff;

  // TODO: Responde
  s_ps->Close();
}

void* listen_ps_broadcast(void* shared_data) {
  std::cout << "Listening to pieces server BROADCAST" << std::endl;

  // Crea un socket
  VSocket* server_socket;

  server_socket = new Socket('d');
  server_socket->subnet = (char*)"172.16.123.95";

  // Escucha en el puerto PORTBROADCASTListening t_LISTEN_PS
  server_socket->Bind(UDP_PS_PORT);  // Port to access this mirror server

  for (;;) {
    // TODO: Cuando recibe
    char ps_buff[RESPONSE_BUFF_SIZE];
    char* addr_src;

    addr_src = server_socket->listenBroadcast(ps_buff, RESPONSE_BUFF_SIZE);

    // si ps_buff == T

    if (strcmp(ps_buff, "T") == 0 &&
        ((shared_data_ps*)shared_data)->ps_ip == "") {
      send_broadcast(UDP_PS_PORT, (char*)"S", (char*)"172.16.123.95");
      //break;
    }

    struct sockaddr* srcAddr = (struct sockaddr*)(addr_src);

    std::cout << "PS_BUFF_CONTENT_UDP: " << ps_buff << std::endl;  // DEBUG TEMP

    // TODO: Guarda el shared_data

    ((shared_data_ps*)shared_data)->ps_ip = addr_src;

    std::cout << "Share data: " << ((shared_data_ps*)shared_data)->ps_ip
              << std::endl;  // DEBUG TEMP

    // TODO: Responde

    delete[] addr_src;
  }
}


void* listen_intermediates(void* shared_data) {
  std::cout << "Listening to intermediates" << std::endl;

  // Crea un socket
  VSocket* server_socket;
  VSocket* intermediate_socket;

  server_socket = new Socket('s');

  // Escucha en el puerto PORT_LISTEN_PS
  server_socket->Bind(PORT_LISTEN_INTERMEDIATE);  // Port to access this mirror server
  server_socket->Listen(LISTEN_CLIENT_QUEUE);

  for (;;) {
    char request_buff[REQUEST_BUFF_SIZE];
    intermediate_socket = server_socket->Accept();
    intermediate_socket->Read(request_buff, REQUEST_BUFF_SIZE);

    std::cout << "REQUEST_BUFF_CONTENT: " << request_buff
              << std::endl;  // DEBUG TEMP

    if (((shared_data_ps*)shared_data)->test_case == 1) {
      sleep(10);
      continue;
    }

    
    ((shared_data_ps*)shared_data)->request = request_buff;

    // TODO: create piece request

    request_ps(shared_data);

    char response_buff[RESPONSE_BUFF_SIZE];
    strcpy(response_buff, ((shared_data_ps*)shared_data)->ps_answer.c_str());
    intermediate_socket->Write(response_buff);
    intermediate_socket->Close();
  }
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

void enviarSignal(int signum) {
  // Envia mensaje de eutanasia
  std::cout << "signal, eutanacia" << std::endl;
  send_broadcast(UDP_PS_PORT, (char*)"E", (char*)"172.16.123.95");

  // Terminar el programa
  exit(signum);
}

int main(int argc, char* argv[]) {
  signal(SIGINT, enviarSignal);

  int test_case = 0;

  if (argc > 1) {
    test_case = std::stoi(argv[1]);
  }

  shared_data_ps* shared_data = new shared_data_ps();
  shared_data->ps_menu = "";
  shared_data->ps_ip = "";
  shared_data->intermediates_ip = {};
  shared_data->test_case = test_case;
  shared_data->request = "";

  // Mandar broadcast al servidor de piezas

  char* broadcast = (char*)"T";

  send_broadcast(UDP_PS_PORT, broadcast, (char*)"172.16.123.95");

  // TODO: Mandar broadcast a los servidores intermedios (Esto todavía no)

  // TODO: Genera un hilo para escuchar intermedios(Esto todavía no)

  // Genera un hilo para escuchar a los clientes
  pthread_t thread_client;
  pthread_create(&thread_client, NULL, listen_client, (void*)shared_data);

  // Genera un hilo para escuchar al servidores intermediarios
  //pthread_t thread_intermediates;
  //pthread_create(&thread_intermediates, NULL, listen_intermediates, (void*)shared_data);

  // Genera un hilo para escuchar al servidor de piezas
  pthread_t thread_ps_udp;
  pthread_create(&thread_ps_udp, NULL, listen_ps_broadcast, (void*)shared_data);

  // Queda esperando las consultas
  for (;;) {
  }

  delete shared_data;
  return 0;
}
