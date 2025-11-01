/**
 * @file simulacion-protocolo.cc
 * @brief Simulación de protocolo propuesto para la comunicación Cliente -
 * Servidor Intermedio - Servidor final. En el proyecto final se trabajará con
 * información de piezas de lego, sin embargo, para esta propuesta se trabajará
 * con un arreglo de nombres de animales y otro de descripciones para mayor
 * simplicidad.
 */

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <pthread.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#define DATA_BUFFER 512

// Estructura que sirve para pasar los mensajes
struct Message {
  // Estado de la solicitud: Si el animal existe para el servidor final, es un
  // 1, si no, es un 0.
  int st;

  // Contenido del mensaje
  char data[DATA_BUFFER];
};

// Estructura de los animales (que serán piezas de lego pero no entiendo aún
// cómo representar las piezas)
struct Animals {

  // Nombres de animales
  std::vector<std::string> names;

  // Descripciones
  std::vector<std::string> descriptions;
};

/**
 * Finds the description associated with a given name in the provided vectors.
 *
 * @param names The vector of names to search in.
 * @param descriptions The vector of descriptions corresponding to the names.
 * @param to_find The name for which the description needs to be found.
 * @return The description associated with the given name, or an empty string if
 * not found.
 */
int find_description(const std::vector<std::string> names,
    const std::vector<std::string> descriptions, std::string to_find);

/**
 * @brief Function that represents the final server in the protocol simulation.
 *
 * This function is responsible for handling the final server's behavior in the
 * protocol simulation. It takes a void pointer as an argument.
 *
 * @param arg A void pointer representing the argument passed to the function.
 * @return void* A void pointer representing the return value of the function.
 */
void* final_server(void* arg);

/**
 * @brief Function that represents the middle server in the protocol simulation.
 *
 * @param arg A pointer to the argument passed to the function.
 * @return void* A pointer to the result of the function.
 */
void* middle_server(void* arg);

/**
 * @brief Function that represents a client in the simulation.
 *
 * @param arg A pointer to the argument passed to the client function.
 * @return void* The return value of the client function.
 */
void* client(void* arg);

// Función para el hilo del servidor final
void* final_server(void* arg) {

  // Definición de animales que tiene el servidor
  Animals animals;

  animals.names = { "Goat", "Horse", "Camel" };
  animals.descriptions = { "Goat Desc.", "Horse Desc.", "Camel Desc." };

  // Creación del mensaje
  Message msg;

  int sockfd, newsockfd;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(6666);

  if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("final_server: bind()");
    pthread_exit(NULL);
  }

  // Escucha al intermedio
  listen(sockfd, 5);
  clilen = sizeof(cli_addr);
  newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);

  if (newsockfd < 0) {
    perror("final_server: accept()");
  }

  read(newsockfd, &msg, sizeof(msg));
  printf("5. El servidor final recibe el mensaje\n");

  std::string str(msg.data);

  printf("6. El servidor final busca si tiene al animal\n");
  int animal_index = find_description(animals.names, animals.descriptions, str);

  // Si fue encontrado el animal
  if (animal_index >= 0) {

    msg.st = 1;
    strcpy(msg.data, animals.descriptions[animal_index].c_str());
  } else {

    msg.st = 0;
  }

  printf("7. El servidor final le envía la respuesta al servidor del medio\n");
  write(newsockfd, &msg, sizeof(msg));

  close(newsockfd);
  close(sockfd);

  pthread_exit(NULL);
}

int find_description(const std::vector<std::string> names,
    const std::vector<std::string> descriptions, std::string to_find) {

  int was_found = -1;

  for (size_t animal = 0; animal < names.size(); ++animal) {

    if (names[animal] == to_find) {
      return animal;
    }
  }

  return was_found;
}

// Función para el hilo del servidor intermedio
void* middle_server(void* arg) {

  int sockfd, newsockfd;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;
  Message msg;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("middle_server: socket()");
    pthread_exit(NULL);
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(5555);

  if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("middle_server: bind()");
    pthread_exit(NULL);
  }

  listen(sockfd, 5);

  clilen = sizeof(cli_addr);
  newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);

  if (newsockfd < 0) {
    perror("middle_server: accept()");
    pthread_exit(NULL);
  }

  read(newsockfd, &msg, sizeof(msg));
  printf("3. El servidor del medio recibe la solicitud del cliente\n");

  int final_server_sfd = socket(AF_INET, SOCK_STREAM, 0);

  if (final_server_sfd < 0) {
    perror("middle_server: socket(): final");
    pthread_exit(NULL);
  }

  struct sockaddr_in final_server_addr;
  final_server_addr.sin_family = AF_INET;
  final_server_addr.sin_port = htons(6666);  // Puerto del servidor final
  inet_pton(AF_INET, "127.0.0.1",
      &final_server_addr.sin_addr);  // Dirección del servidor final

  if (connect(final_server_sfd, (struct sockaddr*)&final_server_addr,
          sizeof(final_server_addr))
      < 0) {
    perror("middle_server: connect()");
    pthread_exit(NULL);
  }

  // Enviar solicitud al servidor final
  printf("4. El servidor del medio hizo la conexión y le manda el mensaje al "
         "servidor final\n");
  write(final_server_sfd, &msg, sizeof(msg));

  // Leer respuesta del servidor final
  read(final_server_sfd, &msg, sizeof(msg));
  printf("8. El servidor del medio recibe la respuesta del servidor final\n");

  // Enviar respuesta al cliente
  printf("9. El servidor del medio le pasa la respuesta al cliente\n");
  write(newsockfd, &msg, sizeof(msg));

  close(final_server_sfd);

  close(newsockfd);
  close(sockfd);

  pthread_exit(NULL);
}

// Función para el hilo del cliente
void* client(void* arg) {

  Message msg;

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0) {
    perror("client: socket()");
    pthread_exit(NULL);
  }

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(5555);
  inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

  if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("client: connect()");
    pthread_exit(NULL);
  } else {

    printf("1. El cliente hace la conexión al servidor intermedio\n");
  }

  const char* animal_to_ask = "Camel";
  strcpy(msg.data, animal_to_ask);

  printf("2. El cliente envía la solicitud de %s\n", animal_to_ask);
  write(sockfd, &msg, sizeof(msg));

  // Leer respuesta del servidor intermedio
  read(sockfd, &msg, sizeof(msg));
  printf("10. El cliente recibe la respuesta\n");

  // El cliente recibe la respuesta del servidor intermedio
  if (msg.st == 1) {

    // Se encontró el animal
    printf("Animal description found:");
    printf(msg.data);
    printf("\n");
  } else {
    // No se encontró el animal
    printf("Animal not found\n");
  }

  close(sockfd);

  pthread_exit(NULL);
}

int main() {

  pthread_t thread_server_final, thread_server_intermedio, thread_client;

  // Crear el hilo del servidor final
  pthread_create(&thread_server_final, NULL, final_server, NULL);

  // Crear el hilo del servidor intermedio
  pthread_create(&thread_server_intermedio, NULL, middle_server, NULL);

  // Crear el hilo del cliente
  pthread_create(&thread_client, NULL, client, NULL);

  // Esperar a que todos los hilos terminen
  pthread_join(thread_server_final, NULL);
  pthread_join(thread_server_intermedio, NULL);
  pthread_join(thread_client, NULL);

  return 0;
}
