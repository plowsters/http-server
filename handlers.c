#include "handlers.h"

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void handle_http_request(int client_fd) {
    char buffer[BUFFER_SIZE]; // Buffer to store the incoming request
    int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0'; // Null-terminate the received data
        printf("Received request:\n%s\n", buffer);

        // Construct a simple HTTP response
        const char *response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 51\r\n"
            "\r\n"
            "<html><body><h1>Hello, World!</h1></body></html>";

        // Send the response back to the client
        send(client_fd, response, strlen(response), 0);
    }
    // Close the client connection
    close(client_fd);
}