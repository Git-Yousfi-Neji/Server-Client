#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1" 
#define SERVER_PORT 8889
#define MESSAGE_SIZE 1024
#define MAX_PENDING_CONNECTIONS 5

void handle_client(int client_socket) {
    char message[MESSAGE_SIZE];
    int message_length;

    // Start the message loop
    while (1) {
        // Receive a message from the client
        message_length = recv(client_socket, message, MESSAGE_SIZE, 0);
        if (message_length == -1) {
            fprintf(stderr, "Error receiving message from client\n");
            break;
        } else if (message_length == 0) {
            printf("Client disconnected\n");
            break;
        } else {
            message[message_length] = '\0';
            printf("Client says: %s\n", message);
        }

        // Send a response message to the client
        char response_message[MESSAGE_SIZE];
        snprintf(response_message, MESSAGE_SIZE, "Received message: %s", message);
        if (send(client_socket, response_message, strlen(response_message), 0) == -1) {
            fprintf(stderr, "Error sending message to client\n");
            break;
        }
    }

    // Close the client socket
    close(client_socket);
}

int main() {
    // Create the server socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        fprintf(stderr, "Error creating server socket\n");
        return -1;
    }

    // Set up the server address
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(SERVER_PORT);

    // Bind the socket to the server address
    if (bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == -1) {
        fprintf(stderr, "Error binding server socket\n");
        return -1;
    }

    // Listen for incoming connections
    if (listen(server_socket, MAX_PENDING_CONNECTIONS) == -1) {
        fprintf(stderr, "Error listening for incoming connections\n");
        return -1;
    }

    printf("Server listening on port %d...\n", SERVER_PORT);

    // Start accepting incoming connections
    while (1) {
        // Accept an incoming connection
        struct sockaddr_in client_address;
        socklen_t client_address_length = sizeof(client_address);
        int client_socket = accept(server_socket, (struct sockaddr*) &client_address, &client_address_length);
        if (client_socket == -1) {
            fprintf(stderr, "Error accepting incoming connection\n");
            break;
        }

        printf("Client connected: %s\n", inet_ntoa(client_address.sin_addr));

        // Handle the client connection in a separate thread
        handle_client(client_socket);
    }

    // Close the server socket
    close(server_socket);

    return 0;
}
