#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 1024

int main() {
    int recv_fd = socket(PF_INET, SOCK_DGRAM, 0);
    if (recv_fd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in recv_addr;
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_port = htons(8080);
    recv_addr.sin_addr.s_addr = htonl("192.168.5.255");

    bind(recv_fd, (struct sockaddr *)&recv_addr, sizeof(recv_addr));

    char buffer[BUFFER_SIZE];
    ssize_t bytes_received = recvfrom(recv_fd, buffer, sizeof(buffer) - 1, 0, NULL, NULL);
    if (bytes_received < 0) {
        perror("Message receiving failed");
        return 1;
    }

    buffer[bytes_received] = '\0';
    printf("Received message: %s\n", buffer);

    close(recv_fd);
    return 0;
}

