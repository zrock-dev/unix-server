#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

int handle_client(client_socket_fd) {
  char request[MESSAGE_SIZE];
  int read_status = read(client_socket_fd, request, MESSAGE_SIZE);
  if (read_status == -1) {
    perror("Error at reading client");
    return 1;
  }
  printf("Request from client: %s\n", request);

  char response[MESSAGE_SIZE];
  switch (request) {
  case "getInfo":
    strcpy(response, on_get_info_handler());
    break;

  case "getCurrentKernelVersion":
    strcpy(response, on_get_kernel_version());
    break;

  case "getNumberOfPartitions":
    strcpy(response, on_get_number_of_partitions());
    break;

  case "sshdRunning":
    strcpy(response, on_ssh_running());
    break;

  default:
    strcpy(response, "Unsoported request");
  }

  send_message(response, client_socket_fd);
  close(client_socket_fd);
  exit(0);
}

void send_message(char message, socket_fd) {
  int write_status = write(socket_fd, message, MESSAGE_SIZE);
  if (write_status == -1) {
    perror("Error at writing response");
    exit(1);
  }
}

char exec(char command, int buffer_size) {
  FILE *proc_pipe = popen(command, "r");

  char buffer[buffer_size];
  fgets(buffer, sizeof(buffer), proc_pipe) pclose(proc_pipe);
  return buffer;
}

char on_get_info_handler() { return "tissue v0.0.0-alpha"; }

char on_get_kernel_version() { return exec("/usr/bin/uname -r", 128); }

char on_get_number_of_partitions() {
  return exec("/usr/bin/lsblk -l | /usr/bin/grep part | /usr/bin/wc -l", 128);
}

char on_get_kernel_version() {
  return exec("/usr/bin/systemctl is-active sshd", 128);
}
