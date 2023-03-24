#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 128

/* Reads in response from server and exports to file
 *  params:
 *     sock - the file descriptor for the server socket 
 */
void handle_server_response(int sock) {
    // open local file
    FILE *fp = fopen("output.txt", "w");
    if (!fp) {
        perror("fopen() failed");
        exit(1);
    }  

    char file_buff[MAX_LINE_LENGTH];
    int count = 0;

    // print response from server to file
    while (recv(sock, file_buff, MAX_LINE_LENGTH, 0) > 0) {
        printf("Client: response recieved server\n");
        
        fprintf(fp, "%s", file_buff);
        count++;
        if (count % 4 == 0) {
            send(sock, "Received messages up to $\n", 27, 0);
        }
        if (strchr(file_buff, '$') != NULL) {
            printf("Client: end of string detected");
            break;
        }
    }
    fclose(fp);
    close(sock);
}

/* TCP Client Driver
 */
int main(int argc, char *argv[]) {
    // verify usage
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <server_ip> <port> <filename>\n", argv[0]);
        exit(1);
    }

    char *server_ip = argv[1];
    int port = atoi(argv[2]);
    char *filename = argv[3];

    // establish client socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket() failed");
        exit(1);
    }
    printf("Client: socket built for port %s\n", argv[1]);

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(port);

    // attempt to establish server connection
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect() failed");
        exit(1);
    }
    printf("Client: connected to server\n");

    // send request to server
    if (send(sock, filename, strlen(filename), 0) < 0) {
        perror("send() failed");
        exit(1);
    }
    printf("Client: send request to server\n");

    handle_server_response(sock);

    return 0;
}
