#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

typedef struct {
    int socket;
    int energy;
    int client_number;
} ClientData;

void die(const char *message) {
    perror(message);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <port> <num_clients>\n", argv[0]);
        exit(1);
    }

    int server_port = atoi(argv[1]);
    int num_clients = atoi(argv[2]);

    int server_socket;
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        die("Failed to create socket");
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(server_port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        die("Failed to bind the server socket");
    }

    printf("Server started. Waiting for clients to connect...\n");

    ClientData clients[num_clients];
    memset(clients, 0, sizeof(clients));

    int connected_clients = 0;

    while (connected_clients < num_clients) {
        int client_socket;
        struct sockaddr_in client_addr;
        memset(&client_addr, 0, sizeof(client_addr));
        socklen_t client_addr_len = sizeof(client_addr);

        int received_energy;

        if (recvfrom(server_socket, &received_energy, sizeof(received_energy), 0, (struct sockaddr *)&client_addr, &client_addr_len) < 0) {
            die("Failed to receive energy value from a client");
        }

        clients[connected_clients].socket = client_socket;
        clients[connected_clients].energy = received_energy;
        clients[connected_clients].client_number = connected_clients + 1;

        printf("Connected client %d. Energy: %d\n", clients[connected_clients].client_number, clients[connected_clients].energy);

        connected_clients++;
    }

    printf("All clients are connected. Starting the competition...\n");

    int current_round[num_clients];
    for (int i = 0; i < num_clients; i++) {
        current_round[i] = clients[i].client_number;
    }
    srand(time(NULL));
    int new_num_clients = num_clients / 2 + num_clients % 2;
    while (num_clients > 1) {
        int next_round[new_num_clients];
        for (int i = 0; i < new_num_clients; i++) {
            if (i == new_num_clients - 1 && num_clients % 2 == 1) {
                next_round[i] = current_round[i * 2];
                clients[current_round[i * 2]].energy *= 2;
            } else {
                int random = rand() % 2;
                next_round[i] = current_round[i * 2 + random];
                clients[current_round[i * 2 + random]].energy = clients[current_round[i * 2 + (random + 1) % 2]].energy;
            }
        }
        for (int i = 0; i < new_num_clients; i++) {
            current_round[i] = next_round[i];
        }

        num_clients = new_num_clients;
        new_num_clients = new_num_clients / 2 + new_num_clients % 2;

        printf("The round is over. The number of participants has been reduced to %d\n", num_clients);
    }

    printf("Competition is finished. Winner: Client %d, Energy: %d\n", clients[current_round[0]].client_number, clients[current_round[0]].energy);

    close(server_socket);

    return 0;
}
