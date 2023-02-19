#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define PORT 8889
using namespace std;

int main() {
    int server_socket = 0, client_socket = 0;
    struct sockaddr_in server_address, client_address;
    int addrlen = sizeof(server_address);

    char buffer[1024] = {0};
    char *auth_msg_prefix = "auth ";

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

    // Bind the socket to the address and port
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        printf("Bind failed\n");
        return -1;
    }

    // Listen for incoming connections
    if (listen(server_socket, 3) < 0) {
        printf("Listen failed\n");
        return -1;
    }

    // Accept incoming connection
    if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t*)&addrlen)) < 0) {
        printf("Accept failed\n");
        return -1;
    }

    // wait for authentication message from client
    int valread = read(client_socket, buffer, 1024);
    if (strncmp(buffer, auth_msg_prefix, strlen(auth_msg_prefix)) == 0) {
        // extract username and password from auth message
        char *username = strtok(buffer + strlen(auth_msg_prefix), " ");
        char *password = strtok(NULL, " ");
        if (strcmp(username, "admin") == 0 && strcmp(password, "password") == 0) {
            send(client_socket, "auth_success", strlen("auth_success"), 0);
            printf("Authentication successful\n");
        } else {
            send(client_socket, "auth_failed", strlen("auth_failed"), 0);
            printf("Authentication failed\n");
            return -1;
        }
    } else {
        printf("Invalid authentication message\n");
        return -1;
    }

    // continue with chat
    while (1) {
        valread = read(client_socket, buffer, 1024);
        printf("%s\n", buffer);
    }

    close(server_socket);
    return 0;
}
