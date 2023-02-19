#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define PORT 8889

int main() {
    int client_socket = 0;
    struct sockaddr_in server_address;

    char buffer[1024] = {0};
    char username[1024], password[1024];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        printf("\n Invalid address/ Address not supported \n");
        return -1;
    }

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        printf("\n Connection Failed \n");
        return -1;
    }

    // send authentication message to server
    char auth_msg[1024] = "auth ";
    strcat(auth_msg, username);
    strcat(auth_msg, " ");
    strcat(auth_msg, password);
    send(client_socket, auth_msg, strlen(auth_msg), 0);

    // wait for response from server
    int valread = read(client_socket, buffer, 1024);
    if (strcmp(buffer, "auth_success") == 0) {
        printf("Authentication successful\n");
        // continue with chat
        while (1) {
            char message[1024];
            printf("Enter message: ");
            fgets(message, 1024, stdin);
            send(client_socket, message, strlen(message), 0);
        }
    } else {
        printf("Authentication failed\n");
        return -1;
    }

    close(client_socket);
    return 0;
}

