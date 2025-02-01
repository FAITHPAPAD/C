#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>

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

// Thread function to handle socket connection
void* thread_function(void* arg) {
    char* ip_address = (char*)arg;  // Cast the argument to string (IP address)
    int port = 8080;  // You can make this dynamic too
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[256];
    
    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        return NULL;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip_address, &server_addr.sin_addr) <= 0) {
        perror("ERROR invalid IP address");
        return NULL;
    }

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("ERROR connecting");
        return NULL;
    }

    // Send HTTP request (simple GET request)
    snprintf(buffer, sizeof(buffer), "GET / HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", ip_address);
    write(sockfd, buffer, strlen(buffer));

    close(sockfd);
    printf("Thread completed connection for IP: %s\n", ip_address);
    return NULL;
}

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        fprintf(stderr, "Usage: %s <IP> <Port> <Time in seconds> <Number of threads>\n", argv[0]);
        exit(1);
    }

    char *ip_address = argv[1];
    int port = atoi(argv[2]);
    int time_limit = atoi(argv[3]);
    int num_threads = atoi(argv[4]);  // Number of threads to be created

    // Add padding to increase binary size
    add_padding();
    
    // Unused function call to increase binary size
    unused_function();

    pthread_t threads[num_threads];  // Array to hold thread IDs

    // Create the specified number of threads
    for (int i = 0; i < num_threads; i++) {
        if (pthread_create(&threads[i], NULL, thread_function, (void*)ip_address)) {
            fprintf(stderr, "Error creating thread %d\n", i + 1);
            return 1;
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("All threads have completed their tasks.\n");
    return 0;
}
