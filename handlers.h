#ifndef HANDLERS_H
#define HANDLERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024

// Function to extract IPv4 or IPv6 address from a struct sockaddr
void *get_in_addr(struct sockaddr *sa);

// Function to handle an HTTP request
void handle_http_request(int client_fd);

// Thread function to handle requests
void *threaded_handle_request(void *client_fd_ptr);

#endif