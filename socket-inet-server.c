#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#define MESSAGE_SIZE 128

void sigint_handler() {
  // finishing other threads
  printf("SIGINT handler - CTRL+C detected, exiting!!!\n");
  exit(0);
}

int socket_listen(int port) {

  signal(SIGINT, &sigint_handler);

  printf("Listening on port %d\n", port);
  int server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket_fd == -1) {
    perror("Error at socket creation");
    return 1;
  }

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  int bind_status = bind(server_socket_fd, (struct sockaddr *)&server_addr,
                         sizeof(server_addr));
  if (bind_status == -1) {
    perror("Error at binding");
    return 1;
  }

  int listen_status = listen(server_socket_fd, 5);
  if (listen_status == -1) {
    perror("Error at listening");
    return 1;
  }

  struct sockaddr_in client_addr;
  int size_client_addr = sizeof(client_addr);
  int client_socket_fd =
      accept(server_socket_fd, (struct sockaddr *)&client_addr,
             (socklen_t *)&size_client_addr);
  if (client_socket_fd == -1) {
    perror("Error at accepting client");
    return 1;
  }

  char request[MESSAGE_SIZE];
  int read_status = read(client_socket_fd, request, MESSAGE_SIZE);
  if (read_status == -1) {
    perror("Error at reading client");
    return 1;
  }

  printf("Request from client: %s\n", request);

  char response[MESSAGE_SIZE];
  strcpy(response, "Message got from client");
  int write_status = write(client_socket_fd, response, MESSAGE_SIZE);
  if (write_status == -1) {
    perror("Error at writing response");
    return 1;
  }

  close(server_socket_fd);
  close(client_socket_fd);

  return 0;
}
