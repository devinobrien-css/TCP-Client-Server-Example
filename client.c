#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 1024

void receive_file(int sock, FILE *fp) {
    char line[MAX_LINE_LENGTH];
    int count = 0;
    while (recv(sock, line, MAX_LINE_LENGTH, 0) > 0) {
        if (strcmp(line, "$") == 0) {
            break;
        }
        fprintf(fp, "%s", line);
        count++;
        if (count % 4 == 0) {
            send(sock, "Received messages up to $\n", 27, 0);
        }
    }
    fclose(fp);
    close(sock);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <server_ip> <port> <filename>\n", argv[0]);
        exit(1);
    }
    char *server_ip = argv[1];
    int port = atoi(argv[2]);
    char *filename = argv[3];

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket() failed");
        exit(1);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(port);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect() failed");
        exit(1);
    }

    if (send(sock, filename, strlen(filename), 0) < 0) {
        perror("send() failed");
        exit(1);
    }

    FILE *fp = fopen("output.txt", "w");
    if (!fp) {
        perror("fopen() failed");
        exit(1);
    }

    receive_file(sock, fp);

    return 0;
}
