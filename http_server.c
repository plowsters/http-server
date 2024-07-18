/*
 What do I need to do to have a functional HTTP Server?
-Sit and wait for HTTP reuest
-Process HTTP request
-Evaluate HTTP request and send important information back to 
client

-Determine edge cases where certain traffic must be sent in a 
different way
-Determine the most secure way to send information while balancing 
usabilty/resource usage
-Do not do more than is necessary

------------------------------------------------------------------------------------------

OSI Model Layers Overview

    Physical Layer (Layer 1)
        Function: Deals with the physical connection between devices.
        Examples: Ethernet, Wi-Fi, cables.

    Data Link Layer (Layer 2)
        Function: Responsible for node-to-node data transfer and error detection.
        Examples: MAC addresses, switches, ARP.

    Network Layer (Layer 3)
        Function: Manages logical addressing and routing of data packets.
        Protocols: IP (Internet Protocol)
            IP (Internet Protocol):
                Handles logical addressing and routing of packets.
                Ensures packets are sent across different networks to their destination.

    Transport Layer (Layer 4)
        Function: Provides end-to-end communication and data integrity.
        Protocols: TCP (Transmission Control Protocol), UDP (User Datagram Protocol)
            TCP (Transmission Control Protocol):
                Ensures reliable, connection-oriented communication.
                Provides error checking, flow control, and data segmentation.
            UDP (User Datagram Protocol):
                Provides a faster, connectionless communication method.
                Suitable for applications that need speed over reliability.

    Session Layer (Layer 5)
        Function: Manages sessions between applications.
        Examples: NetBIOS, PPTP
        Role in Protocol Interaction: Establishes, maintains, and terminates sessions.

    Presentation Layer (Layer 6)
        Function: Translates data formats between the application and the network.
        Examples: SSL/TLS, JPEG, GIF
        Role in Protocol Interaction: Encrypts, compresses, and translates data.

    Application Layer (Layer 7)
        Function: Interfaces directly with end-user applications and provides networking services.
        Protocols: HTTP (HyperText Transfer Protocol), DNS (Domain Name System)
            HTTP (HyperText Transfer Protocol):
                Manages the exchange of web documents.
                Operates at the highest level, providing the rules for transferring files.
            DNS (Domain Name System):
                Resolves domain names to IP addresses.
                Facilitates the use of human-readable addresses in networking.

Relationships and Interactions

    DNS and IP:
        DNS operates at the Application Layer (Layer 7) and translates domain names to IP addresses used by the Network Layer (Layer 3).
        Example: When a user enters a URL, DNS resolves the domain to an IP address.

    IP and TCP/UDP:
        IP operates at the Network Layer (Layer 3), providing logical addressing and routing.
        TCP and UDP operate at the Transport Layer (Layer 4), using IP addresses to send data.
        Example: IP determines the path for data packets, while TCP ensures reliable delivery, and UDP provides faster, connectionless transmission.

    HTTP and TCP:
        HTTP operates at the Application Layer (Layer 7) and relies on TCP at the Transport Layer (Layer 4) for reliable data transmission.
        Example: HTTP requests and responses are sent over a TCP connection to ensure data integrity.

Protocol Examples and Layers

    Application Layer (Layer 7): HTTP, DNS, SMTP, FTP
    Presentation Layer (Layer 6): SSL/TLS, JPEG
    Session Layer (Layer 5): NetBIOS, PPTP
    Transport Layer (Layer 4): TCP, UDP
    Network Layer (Layer 3): IP, ICMP, ARP
 */

#include "handlers.h"

#define PORT "8080" //Which port to send traffic through
#define BACKLOG 20 //How many connections can be in queue at once


int main() {
	int status, sockfd, new_fd; //Variables for status and socket descriptors
    struct addrinfo hints, *servinfo, *p; //Variables for address info
    struct sockaddr_storage their_addr; //Connector's address information
    socklen_t sin_size; //Size of struct sockaddr_storage
    char s[INET6_ADDRSTRLEN]; //Buffer to hold the client's IP address
    int yes = 1; //For setsockopt() SO_REUSEADDR

    memset(&hints, 0, sizeof hints); //Make sure the struct is empty
    hints.ai_family = AF_UNSPEC; //Allow for both IPv4 or IPv6 addresses
    hints.ai_socktype = SOCK_STREAM; //TCP Stream Sockets
    hints.ai_flags = AI_PASSIVE; //Fill in my IP for me

    //Get address info
    if ((status = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    //Loop through all the results and bind to the first one we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        //Create a socket
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
    
        //Set socket options
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            close(sockfd);
            exit(1);
        }

        //Bind the socket to the port
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server:bind");
            continue;
        }

        break;
    }

    //Check if we successfully bound to a socket
    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }

    //Free the linked list of address info
    freeaddrinfo(servinfo);

    //Listen for incoming connections
    if(listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    //Main loop to accept and handle connections
    while (1) {
        sin_size = sizeof their_addr; //Size of struct sockaddr_storage
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        //Convert the addr to a string and print it
        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        printf("server: got connection from %s\n", s);

        //Handle the HTTP request in a child process
        if (!fork()) {
            close(sockfd); //Child doesn't need listener
            handle_http_request(new_fd); //Defined in headers.h
            exit(0);
        }
        close(new_fd); //Parent doesn't need listener
    }
    
    return 0;
}