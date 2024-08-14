#include "handlers.h"

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void handle_http_request(int client_fd) {
    char buffer[BUFFER_SIZE];
    int bytes_received;
    int total_bytes_received = 0;

    // Step 1: Read the request in chunks if larger than buffer size
    while ((bytes_received = recv(client_fd, buffer + total_bytes_received, BUFFER_SIZE - total_bytes_received - 1, 0)) > 0) {
        total_bytes_received += bytes_received;
        if (total_bytes_received >= BUFFER_SIZE - 1 || buffer[total_bytes_received - 1] == '\n') {
            break;
        }
    }

    if (bytes_received < 0) {
        perror("recv error");
        close(client_fd);
        return;
    }

    buffer[total_bytes_received] = '\0';  // Null-terminate the buffer

    // Step 2: Parse the Request Line
    char method[10], uri[100], http_version[10];
    sscanf(buffer, "%s %s %s", method, uri, http_version);

    // Step 3: Check if the method is GET
    if (strcmp(method, "GET") != 0) {
        const char *response = 
            "HTTP/1.1 405 Method Not Allowed\r\n"
            "Content-Length: 0\r\n"
            "\r\n";
        send(client_fd, response, strlen(response), 0);
        close(client_fd);
        return;
    }

    // Step 4: Construct the file path based on the URI
    char *base_dir = getenv("WEBSITE_ROOT");
    if (base_dir == NULL) {
        // Handle the case where the environment variable is not set
        fprintf(stderr, "Environment variable WEBSITE_ROOT not set.\n");
        close(client_fd);
        return;
    }

    char filepath[512];
    snprintf(filepath, sizeof(filepath), "%s", base_dir);  // Copy the base directory path

    if (strcmp(uri, "/") == 0) {
        strcat(filepath, "/index.html");  // Default to index.html if the root is requested
    } else {
        strcat(filepath, uri);
    }

    // Step 5: Security check to block access to sensitive files or directories
    if (strstr(uri, "/.git") != NULL || strstr(uri, "/.vscode") != NULL) {
        const char *response = 
            "HTTP/1.1 403 Forbidden\r\n"
            "Content-Length: 0\r\n"
            "\r\n";
        send(client_fd, response, strlen(response), 0);
        close(client_fd);
        return;
    }

    // Step 6: Check if the file exists and is accessible
    struct stat file_stat;
    if (stat(filepath, &file_stat) != 0 || S_ISDIR(file_stat.st_mode)) {
        const char *response = 
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Length: 0\r\n"
            "\r\n";
        send(client_fd, response, strlen(response), 0);
        close(client_fd);
        return;
    }

    // Step 7: Open the file and read its contents in chunks
    FILE *file = fopen(filepath, "r");
    if (!file) {
        const char *response = 
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Length: 0\r\n"
            "\r\n";
        send(client_fd, response, strlen(response), 0);
        close(client_fd);
        return;
    }

    // Step 8: Determine Content-Type based on file extension
    const char *content_type = "text/plain"; // Default content type
    if (strstr(filepath, ".html")) {
        content_type = "text/html";
    } else if (strstr(filepath, ".css")) {
        content_type = "text/css";
    } else if (strstr(filepath, ".js")) {
        content_type = "application/javascript";
    } else if (strstr(filepath, ".png")) {
        content_type = "image/png";
    } else if (strstr(filepath, ".jpg") || strstr(filepath, ".jpeg")) {
        content_type = "image/jpeg";
    } else if (strstr(filepath, ".gif")) {
        content_type = "image/gif";
    } else if (strstr(filepath, ".ico")) {
        content_type = "image/x-icon";
    }

    // Step 9: Generate the Date header
    time_t now = time(NULL);
    struct tm *gmt = gmtime(&now);
    char date_header[100];
    strftime(date_header, sizeof(date_header), "Date: %a, %d %b %Y %H:%M:%S GMT", gmt);

    // Step 10: Construct and send the response headers
    char response_headers[BUFFER_SIZE];
    snprintf(response_headers, sizeof(response_headers),
        "%s 200 OK\r\n"
        "%s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "\r\n",
        http_version, date_header, content_type, file_stat.st_size);

    send(client_fd, response_headers, strlen(response_headers), 0);

    // Step 11: Send the file content in chunks
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        if (send(client_fd, buffer, bytes_read, 0) == -1) {
            perror("send error");
            break;
        }
    }

    fclose(file);

    // Step 12: Close the client connection
    close(client_fd);
}

void *threaded_handle_request(void *client_fd_ptr) {
    int client_fd = *(int *)client_fd_ptr;
    handle_http_request(client_fd);
    close(client_fd);
    free(client_fd_ptr);
    return NULL;
}