#include "Coordenada.h"
#include "Jogador.h"
#include "Tiro.h"
#include "Jogo.h"
#include "Asteroid.h"
#include <math.h>
#include <time.h>

enum TECLAS {
   KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_SPACE
};

void * teclado(ALLEGRO_THREAD *thread,void *param);
void detectacolisao(ALLEGRO_THREAD *thread,void *param);
int verificaPosicao(Jogo *jogo,Coordenada *posicao);

void * teclado(ALLEGRO_THREAD *thread,void *param){
    Jogo *jogo = (Jogo*) param;
    int teclas[5] = {0,0,0,0,0};
    int desenha = 0;
    int lado = 0;
    srand(time(NULL));
    Tiro *tiro;
    time_t tinicio, tfim;
    time(&tinicio);
    
    al_rest(1);
    while(jogo->sair){     
        ALLEGRO_EVENT evento;
        al_wait_for_event(jogo->filaEventos, &evento);
        
        if(evento.type == ALLEGRO_EVENT_TIMER){
            if(teclas[KEY_UP]){
                jogo->jogador->posicao->dy +=  sin(jogo->jogador->posicao->angulo*ALLEGRO_PI/180 ) * 5;
                jogo->jogador->posicao->dx +=  cos(jogo->jogador->posicao->angulo*ALLEGRO_PI/180 ) * 5;
                
            }else if(teclas[KEY_DOWN]){
                jogo->jogador->posicao->dy -=  sin(jogo->jogador->posicao->angulo*ALLEGRO_PI/180 ) * 5;
                jogo->jogador->posicao->dx -=  cos(jogo->jogador->posicao->angulo*ALLEGRO_PI/180 ) * 5;
                
            }else if(teclas[KEY_LEFT]){
                jogo->jogador->posicao->angulo = jogo->jogador->posicao->angulo-4 <= 0 ? 360 : jogo->jogador->posicao->angulo-4 ;
            }else if(teclas[KEY_RIGHT]){
                jogo->jogador->posicao->angulo =  jogo->jogador->posicao->angulo+4 >= 360 ? 0 : jogo->jogador->posicao->angulo+4;
            }       
            desenha = 1;
            
        }else if(evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch(evento.keyboard.keycode) {
                case ALLEGRO_KEY_UP:
                    teclas[KEY_UP] = 1;
                    break;
                case ALLEGRO_KEY_DOWN:
                    teclas[KEY_DOWN] = 1;
                    break;
                case ALLEGRO_KEY_LEFT: 
                    teclas[KEY_LEFT] = 1;
                    break;
                case ALLEGRO_KEY_RIGHT:
                    teclas[KEY_RIGHT] = 1;
                    break;
                case ALLEGRO_KEY_SPACE:
                    teclas[KEY_SPACE] = 1;    
                    break;
            }
            
        }else if(evento.type == ALLEGRO_EVENT_KEY_UP) {
            switch(evento.keyboard.keycode) {
                case ALLEGRO_KEY_UP:
                    teclas[KEY_UP] = 0;
                    break;
                case ALLEGRO_KEY_DOWN:
                    teclas[KEY_DOWN] = 0;
                    break;
                case ALLEGRO_KEY_LEFT: 
                    teclas[KEY_LEFT] = 0;
                    break;
                case ALLEGRO_KEY_RIGHT:
                    teclas[KEY_RIGHT] = 0;
                    break;
                case ALLEGRO_KEY_SPACE:
                    teclas[KEY_SPACE] = 0;
                    break;
            }
            desenha = 0;
        }else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
            jogo->sair = 0;
            
        }
        if(desenha && al_is_event_queue_empty(jogo->filaEventos)){
            desenha = 0;
            if(teclas[KEY_SPACE]){
                tiro =  initTiro(initCoordenada(jogo->jogador->posicao->x,
                                                jogo->jogador->posicao->y,
                                                jogo->jogador->posicao->dx,
                                                jogo->jogador->posicao->dy,
                                                jogo->jogador->posicao->angulo)
                                                ,jogo->carregaImage("tiro.png"));

                    al_lock_mutex(jogo->listaTiros->mutex);

                    jogo->listaTiros->addDesenho(jogo->listaTiros,
                                       novoDesenho(tiro->sprite,
                                                   tiro->posicao,
                                                   2,
                                                   0)
                                      );

                    free(tiro);
                    al_unlock_mutex(jogo->listaTiros->mutex);
             
            }
            al_lock_mutex(jogo->listaDesenho->mutex);
            if(teclas[KEY_UP]){
                jogo->listaDesenho->addDesenho(jogo->listaDesenho,novoDesenho(jogo->jogador->sprite[0],jogo->jogador->posicao,2,0));
            }else{
                jogo->listaDesenho->addDesenho(jogo->listaDesenho,novoDesenho(jogo->jogador->sprite[1],jogo->jogador->posicao,2,0));
            }
            al_unlock_mutex(jogo->listaDesenho->mutex);
            time(&tfim);
            double diff = difftime(tfim,tinicio);
            
            if(diff > 0.5){
                if(lado){
                jogo->listaAsteroids->addDesenho(jogo->listaAsteroids,
                                                novoDesenho(jogo->carregaImage("asteroid.png"),
                                                initCoordenada(40,40,0,rand()%jogo->altura,rand()%180),2,0)
                                                );
                    lado = 0;
                }else{
                    jogo->listaAsteroids->addDesenho(jogo->listaAsteroids,
                                                novoDesenho(jogo->carregaImage("asteroid.png"),
                                                initCoordenada(40,40,rand()%jogo->largura,0,rand()%180),2,0)
                                                );
                    lado = 1;
                }
            time(&tinicio);
            }
            jogo->atualiza();    
            al_rest(0.05);
        }
     
    }
    
}
int verificaPosicao(Jogo *jogo,Coordenada *posicao){
    
    if (posicao->dx >= jogo->largura || posicao->dx <= jogo->largura*(-1)){
            return 0;
    }
    if (posicao->dy >= jogo->altura || posicao->dy <= jogo->altura*(-1) ){
        return 0;
    }
    return 1;



}
