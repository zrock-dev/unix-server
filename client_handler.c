#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MESSAGE_SIZE 1024

void send_message(char *message, int socket_fd);
char *on_get_info_handler(void);
char *on_get_number_of_partitions(void);
char *on_get_kernel_version(void);
char *on_ssh_running(void);

int handle_client(int client_socket_fd) {
  char request[MESSAGE_SIZE];
  int read_status = read(client_socket_fd, request, MESSAGE_SIZE);
  if (read_status == -1) {
    perror("Error at reading client");
    return 1;
  }
  printf("Request from client: %s\n", request);

  char response[MESSAGE_SIZE];
  if (strcmp(request, "getInfo") == 0) {
    strcpy(response, on_get_info_handler());
  } else if (strcmp(request, "getCurrentKernelVersion") == 0) {
    strcpy(response, on_get_kernel_version());
  } else if (strcmp(request, "getNumberOfPartitions") == 0) {
    strcpy(response, on_get_number_of_partitions());
  } else if (strcmp(request, "sshdRunning") == 0) {
    strcpy(response, on_ssh_running());
  } else {
    strcpy(response, "Unsupported request");
  }

  send_message(response, client_socket_fd);
  close(client_socket_fd);
  return 0;
}

void send_message(char *message, int socket_fd) {
  int write_status = write(socket_fd, message, strlen(message) + 1);
  if (write_status == -1) {
    perror("Error at writing response");
    exit(1);
  }
}

char *exec(char *command, int buffer_size) {
  FILE *proc_pipe = popen(command, "r");
  if (!proc_pipe) {
    perror("popen failed");
    exit(1);
  }

  static char buffer[128];
  if (fgets(buffer, sizeof(buffer), proc_pipe) == NULL) {
    pclose(proc_pipe);
    return NULL;
  }
  pclose(proc_pipe);
  return buffer;
}

char *on_get_info_handler() { return "tissue v0.0.0-alpha"; }

char *on_get_kernel_version() { return exec("/usr/bin/uname -r", 128); }

char *on_get_number_of_partitions() {
  return exec("/usr/bin/lsblk -l | /usr/bin/grep part | /usr/bin/wc -l", 128);
}

char *on_ssh_running() {
  return exec("/usr/bin/systemctl is-active sshd", 128);
}
