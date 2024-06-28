#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

int handle_client(client_socket_fd) {
  char request[MESSAGE_SIZE];
  int read_status = read(client_socket_fd, request, MESSAGE_SIZE);
  if (read_status == -1) {
    perror("Error at reading client");
    return 1;
  }
  printf("Request from client: %s\n", request);

  char response[MESSAGE_SIZE];
  strcpy(response, "Message got from client");

  send_message(response, client_socket_fd);
  close(client_socket_fd);
  exit(0);
}

void send_message(char message, socket_fd) {
  int write_status = write(socket_fd, message, MESSAGE_SIZE);
  if (write_status == -1) {
    perror("Error at writing response");
    return 1;
  }
}
