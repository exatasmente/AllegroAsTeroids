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
    Lista *listas[2];
}SocketSala;

int udpSocket;
int atual;
struct sockaddr_in serverAddr;

ALLEGRO_MUTEX *mutex;
void envia(void *param);
void *recebe(ALLEGRO_THREAD *thread,void *param);
void conecta(SocketSala *partida,int pos);

Coordenada *charToCoordenada(char *msg)
    {
    Coordenada *data = (Coordenada*)malloc(sizeof(Coordenada));
    char aux[11];
    float pos[6];
    int p = 0;
    int paux = 0;
    for(int i = 0 ; i < strlen(msg); i++)
        {
        if(msg[i] == '|')
            {
                pos[p] = atof(aux);
                p++;
                if(paux < 10)
                    {
                    for(int j = 0 ; j < 11 ; j++)
                        {
                        aux[j] = ' ';
                    }
                        
                }
                paux = 0;
        }else
            {
                aux[paux] = msg[i];
                paux++;
        }
        
    }
    data->x = pos[0];
    data->y = pos[1];
    data->dx = pos[2];
    data->dy = pos[3];
    data->angulo = pos[4];
    data->tipo = pos[5];
return data;}


void conecta(SocketSala *partida,int pos)
    {
    char buffer[20];
    int nBytes;
    nBytes = recvfrom(udpSocket,buffer,20,0,(struct sockaddr *)&partida->pessoas[pos], &partida->tamSock[pos]);
    sscanf(buffer,"%d|%d",&partida->janelas[pos]->largura,&partida->janelas[pos]->altura);
    printf("Novo Jogo : %dx%d\n",partida->janelas[0]->largura,
                                 partida->janelas[0]->altura);

}

void *recebe(ALLEGRO_THREAD *thread,void *param)
    {
    
    int nBytes;
    int pos = atual++;
    SocketSala *partida = (SocketSala*)param;
    Coordenada *data;
    
    while(1)
        {
        char *buffer[62];    
        nBytes = recvfrom(udpSocket,buffer,62,0,(struct sockaddr *)&partida->pessoas[pos], &partida->tamSock[pos]);
        data = (Coordenada*) malloc(sizeof(Coordenada));
        printf("Recebeu %d: %s\n",pos,buffer);
        data = charToCoordenada(buffer);
        al_lock_mutex(mutex);
        add(partida->listas[pos],novoNode(data));
        al_unlock_mutex(mutex);
    }
    

}

void envia(void *param)
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
        //sendto( udpSocket, msg, strlen(msg)+1,0,(struct sockaddr *)&partida->pessoas[0], partida->tamSock[0]);
        //sendto( udpSocket, msg, strlen(msg)+1,0,(struct sockaddr *)&partida->pessoas[1], partida->tamSock[1]);
        if(partida->listas[1]->qt > 0){
            al_lock_mutex(mutex);
            Coordenada *dados = (Coordenada*) remover(partida->listas[1])->valor;
            al_unlock_mutex(mutex);
            char *msg[61];    
            sprintf(msg,"%f|%f|%f|%f|%d|%d|",dados->x,dados->y,dados->dx,dados->dy,dados->angulo,0);
            sendto( udpSocket, msg, strlen(msg)+1,0,(struct sockaddr *)&partida->pessoas[0], partida->tamSock[0]);
            free(dados);
        }
        if(partida->listas[0]->qt > 0){
            al_lock_mutex(mutex);
            Coordenada *dados = (Coordenada*) remover(partida->listas[0])->valor;
            al_unlock_mutex(mutex);
            char *msg[61];    
            sprintf(msg,"%f|%f|%f|%f|%d|%d|",dados->x,dados->y,dados->dx,dados->dy,dados->angulo,0);
            sendto( udpSocket, msg, strlen(msg)+1,0,(struct sockaddr *)&partida->pessoas[1], partida->tamSock[1]);
            free(dados);
        }
        
        
        free(data);        
    }

}

int main(void){
  SocketSala *partida;
  mutex = al_create_mutex();

  ALLEGRO_THREAD *player1;
  ALLEGRO_THREAD *player2;
  /*Create UDP socket*/
  udpSocket = socket(PF_INET, SOCK_DGRAM, 0);
  partida = (SocketSala*)malloc(sizeof(SocketSala));

  partida->listas[0]= initLista();
  partida->listas[1]= initLista();

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
  player1 = al_create_thread(&recebe,partida);
  player2 = al_create_thread(&recebe,partida);

  conecta(partida,0);
  al_start_thread(player1);
  conecta(partida,1);


  
  al_start_thread(player2);
  envia(partida);
  return 0;
}


