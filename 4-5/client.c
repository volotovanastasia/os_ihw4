#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

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

    if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        die("Failed to connect to the server");
    }

    if (send(server_socket, &energy, sizeof(energy), 0) != sizeof(energy)) {
        die("Failed to send energy value to the server");
    }

    printf("Sent energy value to the server: %d\n", energy);

    close(server_socket);

    return 0;
}
