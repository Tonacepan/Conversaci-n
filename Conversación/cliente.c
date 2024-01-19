#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <pthread.h> // Biblioteca de hilos

int sockfd;
int continueChat = 1;
pthread_mutex_t mutex;

void *receiveMessages(void *arg) {
    char cadena[100];

    while (continueChat) {
        bzero(cadena, 100);
        read(sockfd, cadena, 100);
        printf("\nServidor: %s\n", cadena);

        if (strstr(cadena, "adios")) {
            continueChat = 0;
        }
    }

    pthread_exit(NULL);
}

void *sendMessages(void *arg) {
    char sendline[100];

    while (continueChat) {
        bzero(sendline, 100);
        printf("Cliente:");
        
        fgets(sendline, 100, stdin);
        write(sockfd, sendline, strlen(sendline) + 1);

        if (strstr(sendline, "adios")) {
            continueChat = 0;
        }
    }

    pthread_exit(NULL);
}

int main(void) {
    char cadena[100];
    struct sockaddr_in servaddr;

    FILE *myf = fopen("conversacion_cliente.txt", "a");
    time_t t;
    struct tm *tm;
    char hora[100];
    char *tmp;
    char sendline[100] = "Usando el puerto 22000\n";

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(22000);

    inet_pton(AF_INET, "127.0.0.1", &(servaddr.sin_addr));
    connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    printf("\n\n\t\tSe inició el chat\n\n");
    fputs("\n\n\t\tSe inició el chat\n\n", myf);

    pthread_t receiveThread, sendThread;
    pthread_create(&receiveThread, NULL, receiveMessages, NULL);
    pthread_create(&sendThread, NULL, sendMessages, NULL);

    pthread_join(receiveThread, NULL);
    pthread_join(sendThread, NULL);

    printf("\n\nConversación finalizada\n");
    
    fclose(myf);
    return 0;
}

