#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define PAD 1024*1024  // 1MB padding per call

// Function to add padding to increase binary size
void add_padding() {
    char padding[PAD];
    memset(padding, 0, PAD);  // Pad with zeros to increase the binary size
    printf("Padding added to make binary larger.\n");
}

// Dummy unused function to increase binary size
void unused_function() {
    printf("This function does nothing but increases binary size.\n");
    for (int i = 0; i < 1000000; i++) {
        // Some pointless loop to increase size
        int a = i * i;
    }
}

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <IP> <Port> <Time in seconds>\n", argv[0]);
        exit(1);
    }

    char *ip_address = argv[1];
    int port = atoi(argv[2]);
    int time_limit = atoi(argv[3]);

    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[256];
    time_t start_time, current_time;

    // Add padding to increase binary size
    add_padding();
    
    // Unused function call to increase binary size
    unused_function();

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip_address, &server_addr.sin_addr) <= 0) {
        error("ERROR invalid IP address");
    }

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error("ERROR connecting");
    }

    // Send HTTP request (simple GET request)
    snprintf(buffer, sizeof(buffer), "GET / HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", ip_address);
    write(sockfd, buffer, strlen(buffer));

    // Get current time and calculate elapsed time
    time(&start_time);
    while (1) {
        time(&current_time);
        if (difftime(current_time, start_time) >= time_limit) {
            break;
        }
    }

    close(sockfd);
    printf("Connection closed after %d seconds.\n", time_limit);
    return 0;
}
