#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024

void die(const char *message) {
    perror(message);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <server_ip> <server_port> <energy>\n", argv[0]);
        exit(1);
    }

    char *server_ip = argv[1];
    int server_port = atoi(argv[2]);
    int energy = atoi(argv[3]);

    int server_socket;
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        die("Failed to create socket");
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_ip, &(server_addr.sin_addr)) <= 0) {
        die("Invalid server address");
    }

    if (sendto(server_socket, &energy, sizeof(energy), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) !=
        sizeof(energy)) {
        die("Failed to send energy value to the server");
    }

    printf("Sent energy value to the server: %d\n", energy);

    char buffer[MAX_BUFFER_SIZE];
    ssize_t num_bytes;
    socklen_t server_addr_len = sizeof(server_addr);

    while ((num_bytes = recvfrom(server_socket, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&server_addr,
                                 &server_addr_len)) > 0) {
        buffer[num_bytes] = '\0';
        printf("Received message: %s\n", buffer);

        if (strncmp(buffer, "Competition is finished", 23) == 0 ||
            strncmp(buffer, "Congratulations!!! You ", 23) == 0) {
            break;
        }
    }

    if (num_bytes < 0) {
        die("Failed to receive message from the server");
    }

    close(server_socket);

    return 0;
}
