#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 2048
#define MAX_PENDING 5

void handle_client(int client_sock, FILE *fp) {
    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, fp)) {
        send(client_sock, line, strlen(line), 0);
    }
    send(client_sock, "$", 1, 0);
    fclose(fp);
    close(client_sock);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    int port = atoi(argv[1]);

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("socket() failed");
        exit(1);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind() failed");
        exit(1);
    }

    if (listen(server_sock, MAX_PENDING) < 0) {
        perror("listen() failed");
        exit(1);
    }

    while (1) {
        struct sockaddr_in client_addr;
        unsigned int client_len = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
        if (client_sock < 0) {
            perror("accept() failed");
            exit(1);
        }

        char filename[MAX_LINE_LENGTH];
        int bytes_recieved;
        if ((bytes_recieved = recv(client_sock, filename, MAX_LINE_LENGTH, 0)) <= 0) {
            perror("recv() failed");
            exit(1);
        }
        filename[bytes_recieved] = '\0';
        printf("Recieved:\n");
        printf("%d\n",bytes_recieved);
        printf("%s\n",filename);

        FILE *fp = fopen(filename, "r");
        if (!fp) {
            perror("fopen() failed");
            exit(1);
        }

        handle_client(client_sock, fp);
    }

    return 0;
}
