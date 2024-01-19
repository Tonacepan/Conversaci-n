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

int comm_fd;
int continueChat = 1;
pthread_mutex_t mutex;

//para recibir los mensajes del cliente.
void *receiveMessages(void *arg) {
    char cadena[100];
	//se ejecuta mientras continueChat sea verdadero.
    while (continueChat) {
    	//cadena utilizando la funci�n bzero, para asegurarse de que est� vac�a antes de leer nuevos datos.
        bzero(cadena, 100);
        // read para leer los datos del socket comm_fd y los guarda en la variable cadena
        read(comm_fd, cadena, 100);
        printf("\nCliente: %s\n", cadena);
		//Verifica si el mensaje recibido contiene la cadena "adios" utilizando la funci�n strstr
		//el caht finaliza
        if (strstr(cadena, "adios")) {
            continueChat = 0;
        }
    }
	//el hilo se cierra
    pthread_exit(NULL);
}

//para enviar mensajes desde el servidor
void *sendMessages(void *arg) {
    char sendline[100];
	//se ejecuta mientras continueChat sea verdadero.
    while (continueChat) {
    	//Limpia el contenido de la variable sendline utilizando la funci�n bzero, para asegurarse de que est� vac�a antes de leer nuevos datos.
        bzero(sendline, 100);
        printf("Servidor: ");
        //para leer una l�nea de texto desde la entrada est�ndar y la guarda en la variable sendline
        fgets(sendline, 100, stdin);
        //para enviar el contenido de sendline al socket comm_fd y as� enviarlo al cliente.
        write(comm_fd, sendline, strlen(sendline) + 1);

        if (strstr(sendline, "adios")) {
            continueChat = 0;
        }
    }
	//el hilo se cierra
    pthread_exit(NULL);
}

int main(void) {
    char cadena[100];
    //para el socket del servido
    int listen_fd;
    //para almacenar la direcci�n del servidor
    struct sockaddr_in servaddr;

    FILE *myf = fopen("conversacion_servidor.txt", "a");
    time_t t;
    struct tm *tm;
    char hora[100];
    char *tmp;
    char sendline[100] = "Usando el puerto 22000\n";
	
	//Crea un socket utilizando la funci�n socket con el dominio AF_INET y el tipo de socket SOCK_STREAM.
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    //Limpia la estructura servaddr utilizando bzero para asegurarse de que est� vac�a antes de configurarla.
    bzero(&servaddr, sizeof(servaddr));
	
	//servaddr con la familia de direcciones AF_INET
	//la direcci�n IP del servidor como INADDR_ANY
	//el puerto del servidor como 22000.
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(22000);
	//Asocia el socket al puerto y la direcci�n IP utilizando la funci�n bind.
    bind(listen_fd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    //con una cola de espera m�xima de 10 conexiones.
    listen(listen_fd, 10);
    //Acepta una conexi�n entrante utilizando la funci�n accept y guarda el descriptor de archivo de la conexi�n en comm_fd.
    comm_fd = accept(listen_fd, (struct sockaddr *)NULL, NULL);

    printf("\n\n\t\tSe inici� el chat\n\n");
    fputs("\n\n\t\tSe inici� el chat\n\n", myf);
    
	//Crea dos hilos utilizando la funci�n pthread_create: 
	//uno para recibir mensajes (receiveThread) y otro para enviar mensajes (sendThread). 
	//Estos hilos ejecutar�n las funciones receiveMessages y sendMessages, respectivamente.
    pthread_t receiveThread, sendThread;
    pthread_create(&receiveThread, NULL, receiveMessages, NULL);
    pthread_create(&sendThread, NULL, sendMessages, NULL);
	//Espera a que los hilos finalicen su ejecuci�n utilizando la funci�n pthread_join.
    pthread_join(receiveThread, NULL);
    pthread_join(sendThread, NULL);

    printf("\n\nConversaci�n finalizada\n");
    
    fclose(myf);
    return 0;
}

