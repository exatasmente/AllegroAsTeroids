#include "Coordenada.h"
#include "Jogador.h"
#include "Jogo.h"
#include <math.h>
#include <time.h>

int desenha;
int teclas[5] = {0,0,0,0,0};
enum TECLAS {
   KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_SPACE
};

void * teclado(ALLEGRO_THREAD *thread,void *param);
void atualizaPosicao(Coordenada *posicao);
void atualizaDesenhos(ListaDesenho *lista,ALLEGRO_BITMAP *sprite , Coordenada *posicao);

void atualizaPosicao(Coordenada *posicao){
    if(teclas[KEY_UP]){
        posicao->dy +=  sin(posicao->angulo*ALLEGRO_PI/180 ) * 5;
        posicao->dx +=  cos(posicao->angulo*ALLEGRO_PI/180 ) * 5;
    }else if(teclas[KEY_DOWN]){
        posicao->dy -=  sin(posicao->angulo*ALLEGRO_PI/180 ) * 5;
        posicao->dx -=  cos(posicao->angulo*ALLEGRO_PI/180 ) * 5;
    }
    if(teclas[KEY_LEFT]){
        posicao->angulo = posicao->angulo-4 <= 0 ? 360 : posicao->angulo-4 ;
    }else if(teclas[KEY_RIGHT]){
        posicao->angulo =  posicao->angulo+4 >= 360 ? 0 : posicao->angulo+4;
    }      
    desenha = 1;
}

void atualizaDesenhos(ListaDesenho *lista,ALLEGRO_BITMAP *sprite , Coordenada *posicao){
    Desenho *desenho = novoDesenho(sprite,posicao,0);
    addDesenho(lista,desenho);
    desenho = NULL;
}
void * teclado(ALLEGRO_THREAD *thread,void *param){
    Jogo *jogo = (Jogo*) param;
    Jogador *jogador = jogo->jogador;
    int lado = 0;
    srand(time(NULL));
    
    time_t tinicio, tfim;
    time(&tinicio);
    ALLEGRO_SAMPLE *sample = al_load_sample("tiro.wav");
    al_rest(1);
    while(jogo->sair){     
        ALLEGRO_EVENT evento;
        al_wait_for_event(jogo->filaEventos, &evento);
        
        if(evento.type == ALLEGRO_EVENT_TIMER){
            atualizaPosicao(jogador->posicao);
            
        }else if(evento.type == ALLEGRO_EVENT_KEY_DOWN){
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
            desenha = 0;
        }else if(evento.type == ALLEGRO_EVENT_KEY_UP){
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
        }else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
            jogo->sair = 0;
            
        }
        if(desenha && al_is_event_queue_empty(jogo->filaEventos)){
            desenha = 0;
            if(teclas[KEY_SPACE]){
                ALLEGRO_BITMAP *sprite = al_load_bitmap("tiro.png");                
                Coordenada *posicao = initCoordenada(6,12,jogador->posicao->dx,jogador->posicao->dy,jogador->posicao->angulo);
                
                al_play_sample(sample, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
              
                al_lock_mutex(jogo->listaTiros->mutex);
                atualizaDesenhos(jogo->listaTiros,sprite,posicao);

                al_unlock_mutex(jogo->listaTiros->mutex);
                sprite = NULL;
                
             
            }

            al_lock_mutex(jogo->listaDesenho->mutex);
            if(teclas[KEY_UP]){
                atualizaDesenhos(jogo->listaDesenho,jogador->sprite[0],jogador->posicao);                
            }else{
                atualizaDesenhos(jogo->listaDesenho,jogador->sprite[1],jogador->posicao);
            }
            al_unlock_mutex(jogo->listaDesenho->mutex);

            time(&tfim);
            double diff = difftime(tfim,tinicio);
            
            if(diff > 0.5){
                ALLEGRO_BITMAP *sprite = al_load_bitmap("asteroid.png");
                Coordenada *posicao;
                al_lock_mutex(jogo->listaAsteroids->mutex);
                switch(lado){
                    case 0:
                        posicao = initCoordenada(40,40,0,rand()%jogo->altura,rand()%90);
                        atualizaDesenhos(jogo->listaAsteroids,sprite,posicao);
                        lado = 1;
                        break;
                    case 1:
                        posicao = initCoordenada(40,40,rand()%jogo->largura,0,rand()%180);
                        atualizaDesenhos(jogo->listaAsteroids,sprite,posicao);
                        lado = 2;
                        break;
                    case 2:
                        posicao = initCoordenada(40,40,rand()%jogo->largura,jogo->altura,rand()%90);
                        atualizaDesenhos(jogo->listaAsteroids,sprite,posicao);
                        lado = 3;
                        break;
                    case 3:
                        posicao = initCoordenada(40,40,jogo->largura,rand()%jogo->altura,rand()%180);
                        atualizaDesenhos(jogo->listaAsteroids,sprite,posicao);
                        lado = 0;
                        break;
                }
                al_unlock_mutex(jogo->listaAsteroids->mutex);
                
                time(&tinicio);
            }
            atualiza();    
            al_rest(0.05);
        }

     
    }
      al_destroy_sample(sample);
    
}
int verificaPosicao(Jogo *jogo,Coordenada *posicao){
    if (posicao->dx >= jogo->largura || posicao->dx <= (jogo->largura*(-1))/2){
            return 0;
    }
    if (posicao->dy >= jogo->altura || posicao->dy <= (jogo->altura*(-1))/2 ){
        return 0;
    }
    return 1;
}
