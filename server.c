#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 128
#define MAX_PENDING 5

/* Reads in request from client and exports to file
 *  params:
 *     sock - the file descriptor for the server socket 
 */
void handle_client_request(int client_sock,  char* filename) {
    // open specified file for reading
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("fopen() failed");
        exit(1);
    }
    printf("Server: output file successfully opened\n");

    // read from file, write to client socket
    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, fp)) {
        send(client_sock, line, strlen(line), 0);
    }
    send(client_sock, "$", 1, 0);
    printf("Successfully outputed to socket\n");

    fclose(fp);
    printf("Server: file closed\n");

    close(client_sock);
    printf("Server: socket closed\n");
}

/* TCP Server Driver 
 */
int main(int argc, char *argv[]) {
    // verify usage
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    int port = atoi(argv[1]);

    // build socket on given port
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("socket() failed");
        exit(1);
    }
    printf("Server: socket built for port %s\n", argv[1]);
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    // bind socket on local host
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind() failed");
        exit(1);
    }
    printf("Server: socket opened on port %s of localhost\n", argv[1]);

    // begin listening
    if (listen(server_sock, MAX_PENDING) < 0) {
        perror("listen() failed");
        exit(1);
    }
    printf("Server: listening...\n");

    // loop to accept consecutive client requests
    while (1) {
        struct sockaddr_in client_addr;
        unsigned int client_len = sizeof(client_addr);

        // accept a client connection
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
        if (client_sock < 0) {
            perror("accept() failed");
            exit(1);
        }
        printf("Server: connection accepted from client on socket %d", client_sock);

        // read in input from client
        char body[MAX_LINE_LENGTH];
        int bytes_recieved;
        if ((bytes_recieved = recv(client_sock, body, MAX_LINE_LENGTH, 0)) <= 0) {
            perror("recv() failed");
            exit(1);
        }
        body[bytes_recieved] = '\0';

        printf("Server: recieved data from the client\n");
        printf("Bytes recieved: %d\n",bytes_recieved);
        printf("Body: %s\n",body);

        handle_client_request(client_sock,body);
    }
    return 0;
}
