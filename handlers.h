#ifndef HANDLERS_H
#define HANDLERS_H
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> //socket API functions and data types
#include <sys/types.h> //Data type definitions for <sys/socket.h>
#include <unistd.h> // POSIX API for syscalls
#include <netdb.h> //Network database operations (e.g. getaddrinfo)
#include <arpa/inet.h> //IP address operations
#define BUFFER_SIZE 1024

/*
Helper function to get the IPv4 or IPv6 address from a struct sockaddr
Parameters: struct sockaddr*
Returns: Pointer to IPv4 or IPv6 addr (void * allows us to point to any data type)
*/
void *get_in_addr(struct sockaddr *sa);

/*
Function to handle an HTTP request and send a response
Parameters: socket file descriptor
Returns: A handled request
*/
void handle_http_request(int client_fd);

#endif