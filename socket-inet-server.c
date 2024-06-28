
#include "client_handler.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MESSAGE_SIZE 128
#define THREAD_COUNT 6

pthread_t threads_id[THREAD_COUNT];
int is_running = 1;

void sigint_handler(int signum) {
  printf("SIGINT handler - CTRL+C detected, exiting!!!\n");


  for (int i = 0; i < THREAD_COUNT; i++) {
    int join_status = pthread_join(threads_id[i], NULL);
    if (join_status != 0) {
      perror("Error joining thread.");
    }
  }

  is_running = 0;
  exit(0);
}


void *handle_client_thread(void *arg) {
  int client_socket_fd = *(int *)arg;
  free(arg);
  handle_client(client_socket_fd);
  pthread_exit(NULL);
}

int socket_listen(int port) {
  int current = 0;


  signal(SIGINT, sigint_handler);

  printf("Listening on port %d\n", port);
  int server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket_fd == -1) {
    perror("Error at socket creation");
    exit(1);
  }

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  int bind_status = bind(server_socket_fd, (struct sockaddr *)&server_addr,
                         sizeof(server_addr));
  if (bind_status == -1) {
    perror("Error at binding");
    close(server_socket_fd);
    exit(1);
  }

  int listen_status = listen(server_socket_fd, 5);
  if (listen_status == -1) {
    perror("Error at listening");
    close(server_socket_fd);
    exit(1);
  }

  struct sockaddr_in client_addr;
  socklen_t size_client_addr = sizeof(client_addr);
  while (is_running) {
    int client_socket_fd = accept(
        server_socket_fd, (struct sockaddr *)&client_addr, &size_client_addr);
    if (client_socket_fd == -1) {
      if (is_running) {
        perror("Error at accepting client");
      }
      continue;
    }


    int *client_fd_ptr = malloc(sizeof(int));
    if (client_fd_ptr == NULL) {
      perror("Failed to allocate memory");
      close(client_socket_fd);
      continue;
    }
    *client_fd_ptr = client_socket_fd;

    int t_status = pthread_create(&threads_id[current], NULL,
                                  handle_client_thread, client_fd_ptr);
    if (t_status != 0) {
      perror("Error creating thread.");
      close(client_socket_fd);
      free(client_fd_ptr);
      continue;
    }

    current = (current + 1) % THREAD_COUNT;
  }

  close(server_socket_fd);
  exit(0);
}
