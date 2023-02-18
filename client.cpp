#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1" // Change this to the server's IP address
#define SERVER_PORT 8889
#define MESSAGE_SIZE 1024

int main() {
    // Create the client socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        fprintf(stderr, "Error creating client socket\n");
        return -1;
    }

    // Set up the server address
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_address.sin_port = htons(SERVER_PORT);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == -1) {
        fprintf(stderr, "Error connecting to server\n");
        return -1;
    }

    // Start the message loop
    char message[MESSAGE_SIZE];
    while (1) {
        // Get input from the user
        printf("Enter a message: ");
        fgets(message, MESSAGE_SIZE, stdin);
        message[strcspn(message, "\n")] = 0; // Remove newline character from input

        // Send the message to the server
        if (send(client_socket, message, strlen(message), 0) == -1) {
            fprintf(stderr, "Error sending message to server\n");
            return -1;
        }

        // Receive a message from the server
        char server_message[MESSAGE_SIZE];
        int message_length = recv(client_socket, server_message, MESSAGE_SIZE, 0);
        if (message_length == -1) {
            fprintf(stderr, "Error receiving message from server\n");
            return -1;
        } else if (message_length == 0) {
            printf("Server disconnected\n");
            break;
        } else {
            server_message[message_length] = '\0';
            printf("Server says: %s\n", server_message);
        }
    }

    // Close the socket
    close(client_socket);

    return 0;
}
