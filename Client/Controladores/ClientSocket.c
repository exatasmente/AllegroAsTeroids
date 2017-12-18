#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080

int cSocket, porta, nBytes;

struct sockaddr_in enderecoServidor;
socklen_t enderecoSize;



typedef struct socketData
    {
    int tipo;
    Coordenada *posicao;
}SocketData;

Coordenada *charToCoordenada(char *msg);


void *recebe(ALLEGRO_THREAD *thread ,void *param){
    Jogo *jogo = (Jogo*)param;
    
    while(jogo->sair)
        {
        char *buffer[61];
        Coordenada *dados;
        recvfrom(cSocket,buffer,sizeof(char)*62 ,0,NULL, NULL);
        printf("%s\n",buffer);
        dados = charToCoordenada(buffer);
        switch(dados->tipo)
            {
            case 0:
                al_lock_mutex(jogo->listaTiros->mutex);
                if(jogo->listaUDP->qt < jogo->listaUDP->tam)
                    {
                    atualizaDesenhos(jogo->listaUDP,al_load_bitmap("Sprites/Jogador/sprite1.png"),dados);
                }
                al_unlock_mutex(jogo->listaTiros->mutex);
            break;

            case 1:

            break;

            case 2:
                al_lock_mutex(jogo->listaAsteroids->mutex);
                if(jogo->listaAsteroids->qt < jogo->listaAsteroids->tam)
                    {
                    atualizaDesenhos(jogo->listaAsteroids,al_load_bitmap("Sprites/Asteroid/asteroid.png"),dados);
                }
                al_unlock_mutex(jogo->listaAsteroids->mutex);
                break;
        }
    
        
    }
}

void *clientSocketHanddle(ALLEGRO_THREAD *thread ,void *param)
    {
    Jogo *jogo = (Jogo*)param;
    ALLEGRO_THREAD *recebeThread = al_create_thread(&recebe,jogo);
    cSocket = socket(PF_INET, SOCK_DGRAM, 0);

    enderecoServidor.sin_family = AF_INET;
    enderecoServidor.sin_port = htons(7891);
    enderecoServidor.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(enderecoServidor.sin_zero, '\0', sizeof enderecoServidor.sin_zero);  

    enderecoSize = sizeof enderecoServidor;

    
    char *fmsg;
    sprintf(fmsg,"%d|%d",jogo->largura,jogo->altura);

    nBytes = strlen(fmsg) + 1;           
    sendto(cSocket,fmsg,nBytes,0,(struct sockaddr *)&enderecoServidor,enderecoSize);
    recvfrom(cSocket,fmsg,sizeof(char)*62 ,0,NULL, NULL);
    
    al_start_thread(recebeThread);
    while(jogo->sair){
        /*
        if(jogo->listaTiros->qt > 0){
            char *tiro[62];
            for(Node *aux = jogo->listaTiros->inicio ; aux != jogo->listaTiros->fim ; aux = aux->prox){
                Coordenada *p = aux->valor->posicao;
                sprintf(tiro,"%f|%f|%f|%f|%d|%d",p->x,p->y,p->dx,p->dy,p->angulo,2);
                sendto(cSocket,tiro,nBytes,0,(struct sockaddr *)&enderecoServidor,enderecoSize);       
            }
        }
        */
        char *nave[62];
        sprintf(nave,"%f|%f|%f|%f|%d|%d|",jogo->jogador->posicao->x
                                        ,jogo->jogador->posicao->y
                                        ,jogo->jogador->posicao->dx
                                        ,jogo->jogador->posicao->dy
                                        ,jogo->jogador->posicao->angulo,
                                        0);
        nBytes = strlen(nave) + 1;
        sendto(cSocket,nave,nBytes,0,(struct sockaddr *)&enderecoServidor,enderecoSize);       
        
    }
    al_destroy_thread(recebeThread);
return 0;}


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
