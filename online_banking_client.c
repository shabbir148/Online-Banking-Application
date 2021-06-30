#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<unistd.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>

#define SEND_BUFFER 1024
#define REC_BUFFER 1024


void send_to_server(int clientfd, char *message)
{
    int size_header = (strlen(message) - 1) / SEND_BUFFER + 1;
    int send_status = write(clientfd, &size_header, sizeof(int));

    char *send_message = (char *)malloc(size_header * SEND_BUFFER);
    strcpy(send_message, message);

    for (int i = 0; i < size_header; i++)
    {
        int send_status = write(clientfd, send_message, SEND_BUFFER);
        send_message += SEND_BUFFER;
    }
}

char* receive_from_server(int clientfd)
{
    int size_header = 0;
    // printf("%d\n", sockfd);
    int receive_status = read(clientfd, &size_header, sizeof(int));
    // printf("%d\n", receive_status);
    if (receive_status <= 0)
    {
        shutdown(clientfd, SHUT_WR);
        return NULL;
    }

    char *message = (char *)malloc(size_header * REC_BUFFER);
    memset(message, 0, size_header * REC_BUFFER);

    char *receive_message = message;

    for (int i = 0; i < size_header; i++)
    {
        int receive_status = read(clientfd, message, REC_BUFFER);
        message += REC_BUFFER;
    }

    return receive_message;
}

void handler()
{
   printf("\n\n\nShut Down.\n......GOOOD BYE......\n");
   exit(1);
}

//char* receive_from_server(int clientfd)
//{
//    char *message = (char *)malloc(REC_BUFFER);
//    memset(message, 0, REC_BUFFER);
//    char *rec_mes = message;
//    
//    int number_byte = read(clientfd, message, REC_BUFFER);
//    if (number_byte <= 0)
//    {
//        shutdown(clientfd, SHUT_WR);//shut down the client
//        return NULL;
//    }
//    return rec_mes;
//}

//void send_to_server(int clientfd, char *message)
//{
//    char *send_message = (char *)malloc(SEND_BUFFER);
//    strcpy(send_message, message);
//    write(clientfd, send_message, sizeof(SEND_BUFFER));
//}


int main(int argc, char **argv)
{

   if (signal(SIGINT, handler) == SIG_ERR)
   {
        printf("Error in catching SIGINT\n");
   }
    
  while(1) {
        
    if (argc < 2)
    {
        printf("\n......Please enter port number for connecting bank server......\n");
        exit(0);
    }

    
    printf("CLIENT...\n");

    int clientfd = socket(AF_INET, SOCK_STREAM,0);


    int port_number = atoi(argv[1]);
    
    struct sockaddr_in client;
    client.sin_family = AF_INET;
    client.sin_port = htons(port_number);
    client.sin_addr.s_addr = inet_addr("127.0.0.1");

    int c = connect(clientfd, (struct sockaddr*)(&client), sizeof(client));


    char *bank_response;
    char message[240];
    
    while(1)
    {
    
       bank_response = receive_from_server(clientfd);
       if(bank_response == NULL)
       {
           printf("\n\nDisconnected.....\n");
           break;
       }
       printf("%s\n", bank_response);
       free(bank_response);
       int i=0;
       //special
       memset(message,0,sizeof(message));
    
       printf("Reply to server: \n");
       scanf(" %[^\n]", message);
    
       send_to_server(clientfd, message);
    }
    exit(1);
  }
  return 0;
}
