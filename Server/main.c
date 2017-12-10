#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "Modelos/Coordenada.h"
typedef struct socketData{
    int tipo;
    Coordenada *posicao;
}SocketData;


int main(void){
  int udpSocket, nBytes;
  char buffer[20];
  
  struct sockaddr_in serverAddr, clientAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size, client_addr_size;
  int altura, largura;

  /*Create UDP socket*/
  udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

  /*Configure settings in address struct*/
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(7891);
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  /*Bind socket with address struct*/
  bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  /*Initialize size variable to be used later on*/
  addr_size = sizeof serverStorage;
  Coordenada *data;
  int lado = 0;
  srand(time(NULL));
  nBytes = recvfrom(udpSocket,buffer,20,0,(struct sockaddr *)&serverStorage, &addr_size);
  sscanf(buffer,"%d|%d",&largura,&altura);

  printf("Novo Jogo : %dx%d\n",largura,altura);
  for(;;){
         switch(lado){
            case 0:
                data = initCoordenada(40,40,0,rand()%altura,rand()%90);
                lado = rand()%4;
                break;
            case 1:
                data = initCoordenada(40,40,rand()%largura,0,rand()%90);
                lado = rand()%4;
                break;
            case 2:
                data = initCoordenada(40,40,rand()%largura,altura,rand()%90);
                lado = rand()%4;
                break;
            case 3:
                data = initCoordenada(40,40,largura,rand()%altura,rand()%90);
                lado = rand()%4;
                break;
        }
        free(data);
        char *msg[61];    
        sprintf(msg,"%f|%f|%f|%f|%f|",data->x,data->y,data->dx,data->dy,data->angulo);
        sendto( udpSocket, msg, strlen(msg)+1,0,(struct sockaddr *)&serverStorage, addr_size);
        sleep(0.05);
        
        
  }

  return 0;
}

