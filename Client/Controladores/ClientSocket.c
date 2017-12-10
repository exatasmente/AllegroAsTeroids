#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080

typedef struct socketData{
    int tipo;
    Coordenada *posicao;
}SocketData;

Coordenada *charToCoordenada(char *msg);

void *clientSocketHanddle(ALLEGRO_THREAD *thread ,void *param){
    Jogo *jogo = (Jogo*)param;

    int cSocket, porta, nBytes;
    char *buffer[61];
    struct sockaddr_in enderecoServidor;
    socklen_t enderecoSize;

    cSocket = socket(PF_INET, SOCK_DGRAM, 0);

    enderecoServidor.sin_family = AF_INET;
    enderecoServidor.sin_port = htons(7891);
    enderecoServidor.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(enderecoServidor.sin_zero, '\0', sizeof enderecoServidor.sin_zero);  

    enderecoSize = sizeof enderecoServidor;

    Coordenada *dados;
    char *fmsg;
    sprintf(fmsg,"%d|%d",jogo->largura,jogo->altura);
    nBytes = strlen(fmsg) + 1;           
    sendto(cSocket,fmsg,nBytes,0,(struct sockaddr *)&enderecoServidor,enderecoSize);
    while(jogo->sair){
       recvfrom(cSocket,buffer,sizeof(char)*62 ,0,NULL, NULL);
       dados = charToCoordenada(buffer);
       
        al_lock_mutex(jogo->listaAsteroids->mutex);
        if(jogo->listaAsteroids->qt < jogo->listaAsteroids->tam){
        atualizaDesenhos(jogo->listaAsteroids,
                         al_load_bitmap("Sprites/Asteroid/asteroid.png"),
                         dados);
        }
        al_unlock_mutex(jogo->listaAsteroids->mutex);
       
    }

return 0;}


Coordenada *charToCoordenada(char *msg){
    Coordenada *data = (Coordenada*)malloc(sizeof(Coordenada));
    sscanf(msg,"%f|%f|%f|%f|%f|",&data->x,&data->y,&data->dx,&data->dy,&data->angulo);
    return data;
}