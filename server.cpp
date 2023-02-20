#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <vector>
#include <algorithm>

#define PORT 8888

std::vector<int> client_sockets;

void handle_authentication(int client_socket) {
    char buffer[1024] = {0};

    // wait for authentication message from client
    int valread = read(client_socket, buffer, 1024);
    char *auth_msg_prefix = "auth ";
    if (strncmp(buffer, auth_msg_prefix, strlen(auth_msg_prefix)) == 0) {
        // extract username and password from auth message
        char *username = strtok(buffer + strlen(auth_msg_prefix), " ");
        char *password = strtok(NULL, " ");
        if (strcmp(username, "admin") == 0 && strcmp(password, "password") == 0) {
            send(client_socket, "auth_success", strlen("auth_success"), 0);
            printf("Authentication successful for client %d\n", client_socket);
        } else {
            send(client_socket, "auth_failed", strlen("auth_failed"), 0);
            printf("Authentication failed for client %d\n", client_socket);
            close(client_socket);
            return;
        }
    } else {
        printf("Invalid authentication message from client %d\n", client_socket);
        close(client_socket);
        return;
    }

    // add the new client socket to the list of client sockets
    client_sockets.push_back(client_socket);
}

void handle_message(int client_socket) {
    char buffer[1024] = {0};
    int valread = read(client_socket, buffer, 1024);
    if (valread == 0) {
        printf("Client %d disconnected\n", client_socket);
        client_sockets.erase(std::remove(client_sockets.begin(), client_sockets.end(), client_socket), client_sockets.end());
        return;
    }
    printf("Client %d: %s\n", client_socket, buffer);

    // send the message to all other clients
    for (int i = 0; i < client_sockets.size(); i++) {
        if (client_sockets[i] != client_socket) {
            send(client_sockets[i], buffer, strlen(buffer), 0);
        }
    }
}

void handle_client(int client_socket) {
    handle_authentication(client_socket);
    while (1) {
        handle_message(client_socket);
    }
}

void handle_new_connection(int server_socket) {
    struct sockaddr_in client_address;
    socklen_t addrlen = sizeof(client_address);
    int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &addrlen);
    if (client_socket < 0) {
        printf("Accept failed\n");
        return;
    }
    // fork a child process to handle the new client
    int pid = fork();
    if (pid == 0) {
        // child process
        close(server_socket);
        handle_client(client_socket);
        exit(0);
    } else if (pid < 0) {
        printf("Fork failed\n");
        return;
    } else {
        // parent process
        close(client_socket);
    }
}

int main() {
    int server_socket = 0;
    struct sockaddr_in server_address;

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == 0) {
        printf("Socket creation error\n");
        return -1;
    }

    // Set server address to bind to
    server_address.sin_family = AF_INET;
server_address.sin_addr.s_addr = INADDR_ANY;
server_address.sin_port = htons(PORT);
// Bind socket to server address
if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
    printf("Bind failed\n");
    return -1;
}

// Listen for incoming connections
if (listen(server_socket, 5) < 0) {
    printf("Listen failed\n");
    return -1;
}

printf("Server listening on port %d\n", PORT);

// Handle incoming connections
while (1) {
    handle_new_connection(server_socket);
}

return 0;
}