#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "Modelos/Coordenada.h"
#include <allegro5/allegro.h>
#include "ListaCordenada.c"
typedef struct janela{
    int altura;
    int largura;
}Janela;

typedef struct socketSala{
    struct sockaddr_storage *pessoas[2];
    int sockInt[2];
    socklen_t tamSock[2];
    Janela *janelas[2];
}SocketSala;

int udpSocket;
int atual;
struct sockaddr_in serverAddr;
Lista *lista;

ALLEGRO_MUTEX *mutex;

void connect(SocketSala *partida,int pos)
    {
    char buffer[20];
    int nBytes;
    nBytes = recvfrom(udpSocket,buffer,20,0,(struct sockaddr *)&partida->pessoas[pos], &partida->tamSock[pos]);
    sscanf(buffer,"%d|%d",&partida->janelas[pos]->largura,&partida->janelas[pos]->altura);
    printf("Novo Jogo : %dx%d\n",partida->janelas[0]->largura,
                                 partida->janelas[0]->altura);

}
void *recive(ALLEGRO_THREAD *thread,void *param)
    {
    
    int nBytes;
    int pos = atual++;
    SocketSala *partida = (SocketSala*)param;
    Coordenada *data;
    
    while(1)
        {
        char *buffer[61];    
        nBytes = recvfrom(udpSocket,buffer,20,0,(struct sockaddr *)&partida->pessoas[pos], &partida->tamSock[pos]);
        data = (Coordenada*) malloc(sizeof(Coordenada));

        sscanf(buffer,"%f|%f|%f|%f|%d|%d|",data->x,data->y,data->dx,data->dy,data->angulo,data->tipo);
        al_lock_mutex(mutex);
        push(lista,novoNode(data));
        al_unlock_mutex(mutex);
    }
    

}

void *post(ALLEGRO_THREAD *thread,void *param)
    {
    SocketSala *partida = (SocketSala*)param;
    Coordenada *data;
    int lado = 0;
    srand(time(NULL));
    int player = 0;
    while(1)
        {
        switch(lado)
            {
            case 0:
                data = initCoordenada(40,40,0,rand()%partida->janelas[player]->altura,rand()%360,2);
                lado = rand()%4;
                break;
            case 1:
                data = initCoordenada(40,40,rand()%partida->janelas[player]->largura,0,rand()%360,2);
                lado = rand()%4;
                break;
            case 2:
                data = initCoordenada(40,40,rand()%partida->janelas[player]->largura,partida->janelas[player]->altura,rand()%360,2);
                lado = rand()%4;
                break;
            case 3:
                data = initCoordenada(40,40,partida->janelas[player]->largura,rand()%partida->janelas[player]->altura,rand()%360,2);
                lado = rand()%4;
                break;
        }
        char *msg[61];    
        sprintf(msg,"%f|%f|%f|%f|%d|%d|",data->x,data->y,data->dx,data->dy,data->angulo,data->tipo);
        printf("%s\n",msg);
        sendto( udpSocket, msg, strlen(msg)+1,0,(struct sockaddr *)&partida->pessoas[0], partida->tamSock[0]);
        sendto( udpSocket, msg, strlen(msg)+1,0,(struct sockaddr *)&partida->pessoas[1], partida->tamSock[1]);
        sleep(1);
        free(data);        
    }

}

int main(void){
  SocketSala *partida;
  lista = initLista();
  mutex = al_create_mutex();

  /*Create UDP socket*/
  udpSocket = socket(PF_INET, SOCK_DGRAM, 0);
  partida = (SocketSala*)malloc(sizeof(SocketSala));
  partida->janelas[0] = (Janela*)malloc(sizeof(Janela));
  partida->janelas[1] = (Janela*)malloc(sizeof(Janela));

  /*Configure settings in address struct*/
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(7891);
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  /*Bind socket with address struct*/
  bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  /*Initialize size variable to be used later on*/
  partida->tamSock[0] = sizeof partida->pessoas[0] ;
  partida->tamSock[1] = sizeof partida->pessoas[1] ;
  
  return 0;
}

