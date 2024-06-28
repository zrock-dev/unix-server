#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include "client_handler.h"
#define MESSAGE_SIZE 128

void sigint_handler(threads_id, is_running) {
  printf("SIGINT handler - CTRL+C detected, exiting!!!\n");

  for (int i = 0; i < threads; i++) {
    int join_status = pthread_join(*threads_id[i], NULL);
    if (join_status == -1) {
      perror("Error joining thread.");
      return 1;
    }
  }

  is_running = 0;

  exit(0);
}

int socket_listen(int port) {
  int threads = 6;
  pthread_t threads_id[threads];
  int current = 0;
  int is_running = 1;

  signal(SIGINT, &sigint_handler(&threads_id, is_running));

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
  while (is_running == 1) {
    int client_socket_fd =
        accept(server_socket_fd, (struct sockaddr *)&client_addr,
               (socklen_t *)&size_client_addr);
    if (client_socket_fd == -1) {
      perror("Error at accepting client");
      return 1;
    }

    int t_status = pthread_create(&threads_id[current], NULL, &handle_client(client_socket_fd), NULL);
    if (t_status == -1) {
      perror("Error creating thread.");
      return 1;
    }
    current++;
  }

  close(server_socket_fd);
  exit(0);
}
