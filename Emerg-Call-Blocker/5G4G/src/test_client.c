#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "192.168.1.8"
#define PORT 5060
#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection to server failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server %s:%d\n", SERVER_IP, PORT);

    // // Create a string with 1024 characters
    // char message[BUFFER_SIZE];
    // memset(message, 'A', sizeof(message) - 1);
    // message[BUFFER_SIZE - 1] = '\0'; // Null-terminate the string
    char data[] = {0x49, 0x4e, 0x56, 0x49, 0x54, 0x45, 0x20, 0x75, 0x72, 0x6e, 0x3a, 0x73, 0x65, 0x72, 0x76, 0x69, 0x63, 0x65, 0x3a, 0x73, 0x6f, 0x73, 0x2e, 0x61, 0x6d, 0x62, 0x75, 0x6c, 0x61, 0x6e, 0x63, 0x65, 0x20, 0x53, 0x49, 0x50, 0x2f, 0x32, 0x2e, 0x30, 0x0d, 0x0a, 0x56, 0x69, 0x61, 0x3a, 0x20, 0x53, 0x49, 0x50, 0x2f, 0x32, 0x2e, 0x30, 0x2f, 0x54, 0x43, 0x50, 0x20, 0x31, 0x30, 0x2e, 0x31, 0x2e, 0x31, 0x39, 0x32, 0x2e, 0x31, 0x38, 0x3a, 0x37, 0x33, 0x30, 0x30, 0x3b, 0x62, 0x72, 0x61, 0x6e, 0x63, 0x68, 0x3d, 0x7a, 0x39, 0x68, 0x47, 0x34, 0x62, 0x4b, 0x2d, 0x35, 0x32, 0x34, 0x32, 0x38, 0x37, 0x2d, 0x31, 0x2d, 0x2d, 0x2d, 0x66, 0x64, 0x32, 0x61, 0x63, 0x66, 0x64, 0x66, 0x37, 0x30, 0x34, 0x35, 0x64, 0x33, 0x64, 0x31, 0x3b, 0x72, 0x70, 0x6f, 0x72, 0x74, 0x3b, 0x74, 0x72, 0x61, 0x6e, 0x73, 0x70, 0x6f, 0x72, 0x74, 0x3d, 0x54, 0x43, 0x50, 0x0d, 0x0a, 0x4d, 0x61, 0x78, 0x2d, 0x46, 0x6f, 0x72, 0x77, 0x61, 0x72, 0x64, 0x73, 0x3a, 0x20, 0x37, 0x30, 0x0d, 0x0a, 0x52, 0x6f, 0x75, 0x74, 0x65, 0x3a, 0x20, 0x3c, 0x73, 0x69, 0x70, 0x3a, 0x31, 0x30, 0x2e, 0x31, 0x35, 0x39, 0x2e, 0x32, 0x31, 0x32, 0x2e, 0x33, 0x33, 0x3a, 0x37, 0x37, 0x37, 0x37, 0x3b, 0x6c, 0x72, 0x3e, 0x0d, 0x0a, 0x52, 0x6f, 0x75, 0x74, 0x65, 0x3a, 0x20, 0x3c, 0x73, 0x69, 0x70, 0x3a, 0x31, 0x30, 0x2e, 0x31, 0x35, 0x39, 0x2e, 0x32, 0x31, 0x32, 0x2e, 0x33, 0x33, 0x3a, 0x37, 0x37, 0x37, 0x37, 0x3b, 0x74, 0x72, 0x61, 0x6e, 0x73, 0x70, 0x6f, 0x72, 0x74, 0x3d, 0x74, 0x63, 0x70, 0x3b, 0x6c, 0x72, 0x3e, 0x0d, 0x0a, 0x50, 0x72, 0x6f, 0x78, 0x79, 0x2d, 0x52, 0x65, 0x71, 0x75, 0x69, 0x72, 0x65, 0x3a, 0x20, 0x73, 0x65, 0x63, 0x2d, 0x61, 0x67, 0x72, 0x65, 0x65, 0x0d, 0x0a, 0x52, 0x65, 0x71, 0x75, 0x69, 0x72, 0x65, 0x3a, 0x20, 0x73, 0x65, 0x63, 0x2d, 0x61, 0x67, 0x72, 0x65, 0x65, 0x0d, 0x0a, 0x43, 0x6f, 0x6e, 0x74, 0x61, 0x63, 0x74, 0x3a, 0x20, 0x3c, 0x73, 0x69, 0x70, 0x3a, 0x2b, 0x38, 0x38, 0x36, 0x39, 0x36, 0x36, 0x36, 0x36, 0x39, 0x36, 0x32, 0x31, 0x40, 0x31, 0x30, 0x2e, 0x31, 0x2e, 0x31, 0x39, 0x32, 0x2e, 0x31, 0x38, 0x3a, 0x37, 0x33, 0x30, 0x30, 0x3e, 0x3b, 0x2b, 0x73, 0x69, 0x70, 0x2e, 0x69, 0x6e, 0x73, 0x74, 0x61, 0x6e, 0x63, 0x65, 0x3d, 0x22, 0x3c, 0x75, 0x72, 0x6e, 0x3a, 0x67, 0x73, 0x6d, 0x61, 0x3a, 0x69, 0x6d, 0x65, 0x69, 0x3a, 0x33, 0x35, 0x30, 0x35, 0x34, 0x38, 0x30, 0x35, 0x2d, 0x34, 0x34, 0x34, 0x31, 0x35, 0x37, 0x2d, 0x30, 0x3e, 0x22, 0x3b, 0x2b, 0x67, 0x2e, 0x33, 0x67, 0x70, 0x70, 0x2e, 0x69, 0x63, 0x73, 0x69, 0x2d, 0x72, 0x65, 0x66, 0x3d, 0x22, 0x75, 0x72, 0x6e, 0x25, 0x33, 0x41, 0x75, 0x72, 0x6e, 0x2d, 0x37, 0x25, 0x33, 0x41, 0x33, 0x67, 0x70, 0x70, 0x2d, 0x73, 0x65, 0x72, 0x76, 0x69, 0x63, 0x65, 0x2e, 0x69, 0x6d, 0x73, 0x2e, 0x69, 0x63, 0x73, 0x69, 0x2e, 0x6d, 0x6d, 0x74, 0x65, 0x6c, 0x22, 0x3b, 0x2b, 0x67, 0x2e, 0x33, 0x67, 0x70, 0x70, 0x2e, 0x6d, 0x69, 0x64, 0x2d, 0x63, 0x61, 0x6c, 0x6c, 0x3b, 0x2b, 0x67, 0x2e, 0x33, 0x67, 0x70, 0x70, 0x2e, 0x73, 0x72, 0x76, 0x63, 0x63, 0x2d, 0x61, 0x6c, 0x65, 0x72, 0x74, 0x69, 0x6e, 0x67, 0x3b, 0x2b, 0x67, 0x2e, 0x33, 0x67, 0x70, 0x70, 0x2e, 0x70, 0x73, 0x32, 0x63, 0x73, 0x2d, 0x73, 0x72, 0x76, 0x63, 0x63, 0x2d, 0x6f, 0x72, 0x69, 0x67, 0x2d, 0x70, 0x72, 0x65, 0x2d, 0x61, 0x6c, 0x65, 0x72, 0x74, 0x69, 0x6e, 0x67, 0x0d, 0x0a, 0x54, 0x6f, 0x3a, 0x20, 0x3c, 0x75, 0x72, 0x6e, 0x3a, 0x73, 0x65, 0x72, 0x76, 0x69, 0x63, 0x65, 0x3a, 0x73, 0x6f, 0x73, 0x2e, 0x61, 0x6d, 0x62, 0x75, 0x6c, 0x61, 0x6e, 0x63, 0x65, 0x3e, 0x0d, 0x0a, 0x46, 0x72, 0x6f, 0x6d, 0x3a, 0x20, 0x3c, 0x73, 0x69, 0x70, 0x3a, 0x2b, 0x38, 0x38, 0x36, 0x39, 0x36, 0x36, 0x36, 0x36, 0x39, 0x36, 0x32, 0x31, 0x40, 0x69, 0x6d, 0x73, 0x2e, 0x6d, 0x6e, 0x63, 0x30, 0x39, 0x32, 0x2e, 0x6d, 0x63, 0x63, 0x34, 0x36, 0x36, 0x2e, 0x33, 0x67, 0x70, 0x70, 0x6e, 0x65, 0x74, 0x77, 0x6f, 0x72, 0x6b, 0x2e, 0x6f, 0x72, 0x67, 0x3e, 0x3b, 0x74, 0x61, 0x67, 0x3d, 0x33, 0x35, 0x31, 0x64, 0x39, 0x66, 0x33, 0x66, 0x0d, 0x0a, 0x43, 0x61, 0x6c, 0x6c, 0x2d, 0x49, 0x44, 0x3a, 0x20, 0x53, 0x62, 0x63, 0x68, 0x39, 0x32, 0x7a, 0x62, 0x52, 0x51, 0x73, 0x4c, 0x43, 0x66, 0x73, 0x43, 0x48, 0x38, 0x36, 0x36, 0x45, 0x51, 0x2e, 0x2e, 0x40, 0x31, 0x30, 0x2e, 0x31, 0x2e, 0x31, 0x39, 0x32, 0x2e, 0x31, 0x38, 0x0d, 0x0a, 0x43, 0x53, 0x65, 0x71, 0x3a, 0x20, 0x31, 0x20, 0x49, 0x4e, 0x56, 0x49, 0x54, 0x45, 0x0d, 0x0a, 0x53, 0x65, 0x73, 0x73, 0x69, 0x6f, 0x6e, 0x2d, 0x45, 0x78, 0x70, 0x69, 0x72, 0x65, 0x73, 0x3a, 0x20, 0x31, 0x38, 0x30, 0x30, 0x0d, 0x0a, 0x41, 0x63, 0x63, 0x65, 0x70, 0x74, 0x3a, 0x20, 0x61, 0x70, 0x70, 0x6c, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x2f, 0x73, 0x64, 0x70, 0x2c, 0x20, 0x61, 0x70, 0x70, 0x6c, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x2f, 0x33, 0x67, 0x70, 0x70, 0x2d, 0x69, 0x6d, 0x73, 0x2b, 0x78, 0x6d, 0x6c, 0x0d, 0x0a, 0x41, 0x6c, 0x6c, 0x6f, 0x77, 0x3a, 0x20, 0x49, 0x4e, 0x56, 0x49, 0x54, 0x45, 0x2c, 0x20, 0x41, 0x43, 0x4b, 0x2c, 0x20, 0x4f, 0x50, 0x54, 0x49, 0x4f, 0x4e, 0x53, 0x2c, 0x20, 0x43, 0x41, 0x4e, 0x43, 0x45, 0x4c, 0x2c, 0x20, 0x42, 0x59, 0x45, 0x2c, 0x20, 0x55, 0x50, 0x44, 0x41, 0x54, 0x45, 0x2c, 0x20, 0x49, 0x4e, 0x46, 0x4f, 0x2c, 0x20, 0x52, 0x45, 0x46, 0x45, 0x52, 0x2c, 0x20, 0x4e, 0x4f, 0x54, 0x49, 0x46, 0x59, 0x2c, 0x20, 0x4d, 0x45, 0x53, 0x53, 0x41, 0x47, 0x45, 0x2c, 0x20, 0x50, 0x52, 0x41, 0x43, 0x4b, 0x0d, 0x0a, 0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 0x54, 0x79, 0x70, 0x65, 0x3a, 0x20, 0x61, 0x70, 0x70, 0x6c, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x2f, 0x73, 0x64, 0x70, 0x0d, 0x0a, 0x53, 0x75, 0x70, 0x70, 0x6f, 0x72, 0x74, 0x65, 0x64, 0x3a, 0x20, 0x74, 0x69, 0x6d, 0x65, 0x72, 0x2c, 0x20, 0x31, 0x30, 0x30, 0x72, 0x65, 0x6c, 0x2c, 0x20, 0x70, 0x72, 0x65, 0x63, 0x6f, 0x6e, 0x64, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x2c, 0x20, 0x73, 0x65, 0x63, 0x2d, 0x61, 0x67, 0x72, 0x65, 0x65, 0x0d, 0x0a, 0x55, 0x73, 0x65, 0x72, 0x2d, 0x41, 0x67, 0x65, 0x6e, 0x74, 0x3a, 0x20, 0x53, 0x61, 0x6d, 0x73, 0x75, 0x6e, 0x67, 0x20, 0x49, 0x4d, 0x53, 0x20, 0x36, 0x2e, 0x30, 0x20, 0x47, 0x39, 0x39, 0x31, 0x30, 0x5a, 0x48, 0x53, 0x32, 0x41, 0x55, 0x49, 0x4e, 0x0d, 0x0a, 0x53, 0x65, 0x63, 0x75, 0x72, 0x69, 0x74, 0x79, 0x2d, 0x56, 0x65, 0x72, 0x69, 0x66, 0x79, 0x3a, 0x20, 0x69, 0x70, 0x73, 0x65, 0x63, 0x2d, 0x33, 0x67, 0x70, 0x70, 0x3b, 0x71, 0x3d, 0x30, 0x2e, 0x35, 0x3b, 0x61, 0x6c, 0x67, 0x3d, 0x68, 0x6d, 0x61, 0x63, 0x2d, 0x73, 0x68, 0x61, 0x2d, 0x31, 0x2d, 0x39, 0x36, 0x3b, 0x70, 0x72, 0x6f, 0x74, 0x3d, 0x65, 0x73, 0x70, 0x3b, 0x6d, 0x6f, 0x64, 0x3d, 0x74, 0x72, 0x61, 0x6e, 0x73, 0x3b, 0x65, 0x61, 0x6c, 0x67, 0x3d, 0x6e, 0x75, 0x6c, 0x6c, 0x3b, 0x73, 0x70, 0x69, 0x2d, 0x63, 0x3d, 0x39, 0x37, 0x34, 0x33, 0x33, 0x38, 0x37, 0x35, 0x33, 0x3b, 0x73, 0x70, 0x69, 0x2d, 0x73, 0x3d, 0x35, 0x33, 0x30, 0x33, 0x36, 0x30, 0x35, 0x32, 0x32, 0x3b, 0x70, 0x6f, 0x72, 0x74, 0x2d, 0x63, 0x3d, 0x37, 0x38, 0x30, 0x37, 0x3b, 0x70, 0x6f, 0x72, 0x74, 0x2d, 0x73, 0x3d, 0x37, 0x37, 0x37, 0x37, 0x0d, 0x0a, 0x50, 0x2d, 0x50, 0x72, 0x65, 0x66, 0x65, 0x72, 0x72, 0x65, 0x64, 0x2d, 0x49, 0x64, 0x65, 0x6e, 0x74, 0x69, 0x74, 0x79, 0x3a, 0x20, 0x3c, 0x73, 0x69, 0x70, 0x3a, 0x2b, 0x38, 0x38, 0x36, 0x39, 0x36, 0x36, 0x36, 0x36, 0x39, 0x36, 0x32, 0x31, 0x40, 0x69, 0x6d, 0x73, 0x2e, 0x6d, 0x6e, 0x63, 0x30, 0x39, 0x32, 0x2e, 0x6d, 0x63, 0x63, 0x34, 0x36, 0x36, 0x2e, 0x33, 0x67, 0x70, 0x70, 0x6e, 0x65, 0x74, 0x77, 0x6f, 0x72, 0x6b, 0x2e, 0x6f, 0x72, 0x67, 0x3e, 0x0d, 0x0a, 0x41, 0x63, 0x63, 0x65, 0x70, 0x74, 0x2d, 0x43, 0x6f, 0x6e, 0x74, 0x61, 0x63, 0x74, 0x3a, 0x20, 0x2a, 0x3b, 0x2b, 0x67, 0x2e, 0x33, 0x67, 0x70, 0x70, 0x2e, 0x69, 0x63, 0x73, 0x69, 0x2d, 0x72, 0x65, 0x66, 0x3d, 0x22, 0x75, 0x72, 0x6e, 0x25, 0x33, 0x41, 0x75, 0x72, 0x6e, 0x2d, 0x37, 0x25, 0x33, 0x41, 0x33, 0x67, 0x70, 0x70, 0x2d, 0x73, 0x65, 0x72, 0x76, 0x69, 0x63, 0x65, 0x2e, 0x69, 0x6d, 0x73, 0x2e, 0x69, 0x63, 0x73, 0x69, 0x2e, 0x6d, 0x6d, 0x74, 0x65, 0x6c, 0x22, 0x0d, 0x0a, 0x50, 0x2d, 0x45, 0x61, 0x72, 0x6c, 0x79, 0x2d, 0x4d, 0x65, 0x64, 0x69, 0x61, 0x3a, 0x20, 0x73, 0x75, 0x70, 0x70, 0x6f, 0x72, 0x74, 0x65, 0x64, 0x0d, 0x0a, 0x50, 0x2d, 0x50, 0x72, 0x65, 0x66, 0x65, 0x72, 0x72, 0x65, 0x64, 0x2d, 0x53, 0x65, 0x72, 0x76, 0x69, 0x63, 0x65, 0x3a, 0x20, 0x75, 0x72, 0x6e, 0x3a, 0x75, 0x72, 0x6e, 0x2d, 0x37, 0x3a, 0x33, 0x67, 0x70, 0x70, 0x2d, 0x73, 0x65, 0x72, 0x76, 0x69, 0x63, 0x65, 0x2e, 0x69, 0x6d, 0x73, 0x2e, 0x69, 0x63, 0x73, 0x69, 0x2e, 0x6d, 0x6d, 0x74, 0x65, 0x6c, 0x0d, 0x0a, 0x50, 0x2d, 0x41, 0x63, 0x63, 0x65, 0x73, 0x73, 0x2d, 0x4e, 0x65, 0x74, 0x77, 0x6f, 0x72, 0x6b, 0x2d, 0x49, 0x6e, 0x66, 0x6f, 0x3a, 0x20, 0x49, 0x45, 0x45, 0x45, 0x2d, 0x38, 0x30, 0x32, 0x2e, 0x31, 0x31, 0x3b, 0x69, 0x2d, 0x77, 0x6c, 0x61, 0x6e, 0x2d, 0x6e, 0x6f, 0x64, 0x65, 0x2d, 0x69, 0x64, 0x3d, 0x62, 0x30, 0x62, 0x38, 0x36, 0x37, 0x66, 0x61, 0x35, 0x36, 0x34, 0x31, 0x0d, 0x0a, 0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 0x4c, 0x65, 0x6e, 0x67, 0x74, 0x68, 0x3a, 0x20, 0x38, 0x34, 0x33, 0x0d, 0x0a, 0x0d, 0x0a, 0x76, 0x3d, 0x30, 0x0d, 0x0a, 0x6f, 0x3d, 0x53, 0x41, 0x4d, 0x53, 0x55, 0x4e, 0x47, 0x2d, 0x49, 0x4d, 0x53, 0x2d, 0x55, 0x45, 0x20, 0x35, 0x36, 0x31, 0x37, 0x30, 0x36, 0x31, 0x30, 0x38, 0x39, 0x39, 0x20, 0x30, 0x20, 0x49, 0x4e, 0x20, 0x49, 0x50, 0x34, 0x20, 0x31, 0x30, 0x2e, 0x31, 0x2e, 0x31, 0x39, 0x32, 0x2e, 0x31, 0x38, 0x0d, 0x0a, 0x73, 0x3d, 0x53, 0x53, 0x20, 0x56, 0x4f, 0x49, 0x50, 0x0d, 0x0a, 0x63, 0x3d, 0x49, 0x4e, 0x20, 0x49, 0x50, 0x34, 0x20, 0x31, 0x30, 0x2e, 0x31, 0x2e, 0x31, 0x39, 0x32, 0x2e, 0x31, 0x38, 0x0d, 0x0a, 0x74, 0x3d, 0x30, 0x20, 0x30, 0x0d, 0x0a, 0x6d, 0x3d, 0x61, 0x75, 0x64, 0x69, 0x6f, 0x20, 0x31, 0x32, 0x33, 0x34, 0x20, 0x52, 0x54, 0x50, 0x2f, 0x41, 0x56, 0x50, 0x20, 0x31, 0x31, 0x32, 0x20, 0x31, 0x31, 0x36, 0x20, 0x31, 0x30, 0x37, 0x20, 0x31, 0x31, 0x38, 0x20, 0x39, 0x36, 0x20, 0x31, 0x31, 0x31, 0x20, 0x31, 0x31, 0x30, 0x0d, 0x0a, 0x62, 0x3d, 0x41, 0x53, 0x3a, 0x34, 0x32, 0x0d, 0x0a, 0x62, 0x3d, 0x52, 0x53, 0x3a, 0x30, 0x0d, 0x0a, 0x62, 0x3d, 0x52, 0x52, 0x3a, 0x32, 0x35, 0x30, 0x30, 0x0d, 0x0a, 0x61, 0x3d, 0x72, 0x74, 0x70, 0x6d, 0x61, 0x70, 0x3a, 0x31, 0x31, 0x32, 0x20, 0x45, 0x56, 0x53, 0x2f, 0x31, 0x36, 0x30, 0x30, 0x30, 0x0d, 0x0a, 0x61, 0x3d, 0x66, 0x6d, 0x74, 0x70, 0x3a, 0x31, 0x31, 0x32, 0x20, 0x62, 0x72, 0x3d, 0x39, 0x2e, 0x36, 0x2d, 0x32, 0x34, 0x2e, 0x34, 0x3b, 0x62, 0x77, 0x3d, 0x6e, 0x62, 0x2d, 0x73, 0x77, 0x62, 0x3b, 0x63, 0x68, 0x2d, 0x61, 0x77, 0x2d, 0x72, 0x65, 0x63, 0x76, 0x3d, 0x2d, 0x31, 0x0d, 0x0a, 0x61, 0x3d, 0x72, 0x74, 0x70, 0x6d, 0x61, 0x70, 0x3a, 0x31, 0x31, 0x36, 0x20, 0x41, 0x4d, 0x52, 0x2d, 0x57, 0x42, 0x2f, 0x31, 0x36, 0x30, 0x30, 0x30, 0x2f, 0x31, 0x0d, 0x0a, 0x61, 0x3d, 0x66, 0x6d, 0x74, 0x70, 0x3a, 0x31, 0x31, 0x36, 0x20, 0x6d, 0x6f, 0x64, 0x65, 0x2d, 0x63, 0x68, 0x61, 0x6e, 0x67, 0x65, 0x2d, 0x63, 0x61, 0x70, 0x61, 0x62, 0x69, 0x6c, 0x69, 0x74, 0x79, 0x3d, 0x32, 0x3b, 0x6d, 0x61, 0x78, 0x2d, 0x72, 0x65, 0x64, 0x3d, 0x32, 0x32, 0x30, 0x0d, 0x0a, 0x61, 0x3d, 0x72, 0x74, 0x70, 0x6d, 0x61, 0x70, 0x3a, 0x31, 0x30, 0x37, 0x20, 0x41, 0x4d, 0x52, 0x2d, 0x57, 0x42, 0x2f, 0x31, 0x36, 0x30, 0x30, 0x30, 0x2f, 0x31, 0x0d, 0x0a, 0x61, 0x3d, 0x66, 0x6d, 0x74, 0x70, 0x3a, 0x31, 0x30, 0x37, 0x20, 0x6f, 0x63, 0x74, 0x65, 0x74, 0x2d, 0x61, 0x6c, 0x69, 0x67, 0x6e, 0x3d, 0x31, 0x3b, 0x6d, 0x6f, 0x64, 0x65, 0x2d, 0x63, 0x68, 0x61, 0x6e, 0x67, 0x65, 0x2d, 0x63, 0x61, 0x70, 0x61, 0x62, 0x69, 0x6c, 0x69, 0x74, 0x79, 0x3d, 0x32, 0x3b, 0x6d, 0x61, 0x78, 0x2d, 0x72, 0x65, 0x64, 0x3d, 0x32, 0x32, 0x30, 0x0d, 0x0a, 0x61, 0x3d, 0x72, 0x74, 0x70, 0x6d, 0x61, 0x70, 0x3a, 0x31, 0x31, 0x38, 0x20, 0x41, 0x4d, 0x52, 0x2f, 0x38, 0x30, 0x30, 0x30, 0x2f, 0x31, 0x0d, 0x0a, 0x61, 0x3d, 0x66, 0x6d, 0x74, 0x70, 0x3a, 0x31, 0x31, 0x38, 0x20, 0x6d, 0x6f, 0x64, 0x65, 0x2d, 0x63, 0x68, 0x61, 0x6e, 0x67, 0x65, 0x2d, 0x63, 0x61, 0x70, 0x61, 0x62, 0x69, 0x6c, 0x69, 0x74, 0x79, 0x3d, 0x32, 0x3b, 0x6d, 0x61, 0x78, 0x2d, 0x72, 0x65, 0x64, 0x3d, 0x32, 0x32, 0x30, 0x0d, 0x0a, 0x61, 0x3d, 0x72, 0x74, 0x70, 0x6d, 0x61, 0x70, 0x3a, 0x39, 0x36, 0x20, 0x41, 0x4d, 0x52, 0x2f, 0x38, 0x30, 0x30, 0x30, 0x2f, 0x31, 0x0d, 0x0a, 0x61, 0x3d, 0x66, 0x6d, 0x74, 0x70, 0x3a, 0x39, 0x36, 0x20, 0x6f, 0x63, 0x74, 0x65, 0x74, 0x2d, 0x61, 0x6c, 0x69, 0x67, 0x6e, 0x3d, 0x31, 0x3b, 0x6d, 0x6f, 0x64, 0x65, 0x2d, 0x63, 0x68, 0x61, 0x6e, 0x67, 0x65, 0x2d, 0x63, 0x61, 0x70, 0x61, 0x62, 0x69, 0x6c, 0x69, 0x74, 0x79, 0x3d, 0x32, 0x3b, 0x6d, 0x61, 0x78, 0x2d, 0x72, 0x65, 0x64, 0x3d, 0x32, 0x32, 0x30, 0x0d, 0x0a, 0x61, 0x3d, 0x72, 0x74, 0x70, 0x6d, 0x61, 0x70, 0x3a, 0x31, 0x31, 0x31, 0x20, 0x74, 0x65, 0x6c, 0x65, 0x70, 0x68, 0x6f, 0x6e, 0x65, 0x2d, 0x65, 0x76, 0x65, 0x6e, 0x74, 0x2f, 0x31, 0x36, 0x30, 0x30, 0x30, 0x0d, 0x0a, 0x61, 0x3d, 0x66, 0x6d, 0x74, 0x70, 0x3a, 0x31, 0x31, 0x31, 0x20, 0x30, 0x2d, 0x31, 0x35, 0x0d, 0x0a, 0x61, 0x3d, 0x72, 0x74, 0x70, 0x6d, 0x61, 0x70, 0x3a, 0x31, 0x31, 0x30, 0x20, 0x74, 0x65, 0x6c, 0x65, 0x70, 0x68, 0x6f, 0x6e, 0x65, 0x2d, 0x65, 0x76, 0x65, 0x6e, 0x74, 0x2f, 0x38, 0x30, 0x30, 0x30, 0x0d, 0x0a, 0x61, 0x3d, 0x66, 0x6d, 0x74, 0x70, 0x3a, 0x31, 0x31, 0x30, 0x20, 0x30, 0x2d, 0x31, 0x35, 0x0d, 0x0a, 0x61, 0x3d, 0x63, 0x75, 0x72, 0x72, 0x3a, 0x71, 0x6f, 0x73, 0x20, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x20, 0x73, 0x65, 0x6e, 0x64, 0x72, 0x65, 0x63, 0x76, 0x0d, 0x0a, 0x61, 0x3d, 0x63, 0x75, 0x72, 0x72, 0x3a, 0x71, 0x6f, 0x73, 0x20, 0x72, 0x65, 0x6d, 0x6f, 0x74, 0x65, 0x20, 0x6e, 0x6f, 0x6e, 0x65, 0x0d, 0x0a, 0x61, 0x3d, 0x64, 0x65, 0x73, 0x3a, 0x71, 0x6f, 0x73, 0x20, 0x6d, 0x61, 0x6e, 0x64, 0x61, 0x74, 0x6f, 0x72, 0x79, 0x20, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x20, 0x73, 0x65, 0x6e, 0x64, 0x72, 0x65, 0x63, 0x76, 0x0d, 0x0a, 0x61, 0x3d, 0x64, 0x65, 0x73, 0x3a, 0x71, 0x6f, 0x73, 0x20, 0x6f, 0x70, 0x74, 0x69, 0x6f, 0x6e, 0x61, 0x6c, 0x20, 0x72, 0x65, 0x6d, 0x6f, 0x74, 0x65, 0x20, 0x73, 0x65, 0x6e, 0x64, 0x72, 0x65, 0x63, 0x76, 0x0d, 0x0a, 0x61, 0x3d, 0x73, 0x65, 0x6e, 0x64, 0x72, 0x65, 0x63, 0x76, 0x0d, 0x0a, 0x61, 0x3d, 0x70, 0x74, 0x69, 0x6d, 0x65, 0x3a, 0x32, 0x30, 0x0d, 0x0a, 0x61, 0x3d, 0x6d, 0x61, 0x78, 0x70, 0x74, 0x69, 0x6d, 0x65, 0x3a, 0x32, 0x34, 0x30, 0x0d, 0x0a};

    
        // sleep(1);
        // Send the message to the server
    if (send(client_socket, data, strlen(data), 0) == -1) {
        perror("Send failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Message sent to the server\n");

    // Receive response from the server
    memset(buffer, 0, sizeof(buffer));
    if (recv(client_socket, buffer, sizeof(buffer), 0) == -1) {
        perror("Receive failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Received response from server: %s\n", buffer);

    

    // Close the socket
    close(client_socket);

    return 0;
}
