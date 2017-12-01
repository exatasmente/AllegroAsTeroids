#include "includes.h"


void desenhaNave(Jogo *jogo);
void desenhaTiros(Jogo *jogo);
void desenhaInterface(Jogo *jogo);
void desenhaAsteroides(Jogo *jogo);

int main(){
    Jogo *jogo;
    Jogador *jogador;
    mutex  = al_create_mutex();
    cond = al_create_cond(); 
    jogo = novoJogo(800,600);
    jogo->inicializa(jogo);
    ALLEGRO_BITMAP *sprites[2];
    sprites[0] = jogo->carregaImage("sprite.png");
    sprites[1] = jogo->carregaImage("sprite1.png");
    jogador = initJogador(initCoordenada(24,24,320,240,0),3,0,sprites);
    jogo->jogador = jogador;    
    jogo->listaThreads->addThread(jogo->listaThreads,novoAlThread(teclado,jogo,10));
    // jogo->listaThreads->addThread(jogo->listaThreads,novoAlThread(detectacolisao,jogo,11));
    
        while(jogo->sair){
            desenhaInterface(jogo);
            desenhaNave(jogo);
            desenhaAsteroides(jogo);
            desenhaTiros(jogo);
            if(jogo->jogador->vidas == 0){
                break;
            }   
        
	    }
    jogo->finaliza(jogo);
    return 0;
}
void desenhaInterface(Jogo *jogo){
    ALLEGRO_BITMAP *buffer = NULL;
    buffer = al_create_bitmap(jogo->largura,jogo->altura);
    al_set_target_bitmap(buffer);
    al_draw_bitmap(al_get_backbuffer(jogo->janela), 0, 0, 0);                
    al_draw_textf(jogo->fonte, al_map_rgb(255, 0, 0), 10, 10, ALLEGRO_ALIGN_LEFT,"PONTOS : %d", jogo->jogador->pontos);
    al_draw_textf(jogo->fonte, al_map_rgb(255, 0, 0), jogo->largura-200, 10, ALLEGRO_ALIGN_LEFT," VIDAS:  %d", jogo->jogador->vidas);
    jogo->atualiza();        
    al_set_target_bitmap(al_get_backbuffer(jogo->janela));
            
    al_draw_bitmap(buffer,0,0,0);
    al_destroy_bitmap(buffer);    
}

void desenhaNave(Jogo *jogo){
    if(jogo->listaDesenho->qt > 0){        
            int foi = 0;
            ALLEGRO_BITMAP *buffer = NULL;
            buffer = al_create_bitmap(jogo->largura,jogo->altura);
            al_set_target_bitmap(buffer);
            al_draw_bitmap(jogo->fundo,0,0,0);
            
            Desenho *asteroide;
            Desenho *desenho;
            
            al_lock_mutex(jogo->listaDesenho->mutex);            
                desenho = jogo->listaDesenho->removerDesenho(jogo->listaDesenho,0);
                    al_lock_mutex(jogo->listaAsteroids->mutex);
                    for(int j = 0 ; j < jogo->listaAsteroids->qt; j++){
                        asteroide = jogo->listaAsteroids->fila[j];
                        float distancia = sqrt(pow(desenho->posicao->dx-asteroide->posicao->dx,2)+ pow(desenho->posicao->dy-asteroide->posicao->dy,2));
                      
                        if(distancia < desenho->posicao->x+desenho->posicao->y ){
                            jogo->jogador->pontos++;
                            foi = 1;

                            break;
                        }
                    
                    }
                    if(foi){
                        asteroide->id = -1;
                    }
                    al_unlock_mutex(jogo->listaAsteroids->mutex);
              
            al_unlock_mutex(jogo->listaDesenho->mutex);            
            switch(desenho->tipo){
                case 1:
                    break;                    
                case 2:
                if(!foi){
                    if(verificaPosicao(jogo,desenho->posicao)){
                        jogo->desenhaRotacionadoBitMap(desenho->imagem,
                                                   desenho->posicao->x,
                                                   desenho->posicao->y,
                                                   desenho->posicao->dx,
                                                   desenho->posicao->dy,
                                                   desenho->posicao->angulo*ALLEGRO_PI/180,
                                                   desenho->flags);
                
                
                    //al_clear_to_color(al_map_rgba(0, 0, 0, 0));
                    al_set_target_bitmap(al_get_backbuffer(jogo->janela));
                    al_draw_bitmap(buffer,0,0,0);
            
            
                    al_destroy_bitmap(buffer);
                
                    }
                }else{
                    jogo->jogador->vidas -=1;
                    al_draw_text(jogo->fonte, al_map_rgb(255, 0, 0), (jogo->largura/2)-150, jogo->altura/2, ALLEGRO_ALIGN_LEFT,"VOCE MORREU");
                    al_set_target_bitmap(al_get_backbuffer(jogo->janela));
                    al_draw_bitmap(buffer,0,0,0);
            
                    jogo->atualiza();
                    
                    al_destroy_bitmap(buffer);
                }
                    break;
            }
    
    }            

}

void desenhaTiros(Jogo *jogo){

    if(jogo->listaTiros->qt > 0){
            ALLEGRO_BITMAP *buffer = NULL;
            buffer = al_create_bitmap(jogo->largura,jogo->altura);
            al_set_target_bitmap(buffer);
            al_draw_bitmap(al_get_backbuffer(jogo->janela), 0, 0, 0);
            //al_draw_bitmap(jogo->fundo,0,0,0);  
                
            Desenho *asteroide;
            Desenho *tiro;
            int foi = 0;
            for(int i = 0 ; i < jogo->listaTiros->qt ; i++){
                jogo->listaTiros->fila[i]->posicao->dy +=  sin(jogo->listaTiros->fila[i]->posicao->angulo*ALLEGRO_PI/180 ) * 15;
                jogo->listaTiros->fila[i]->posicao->dx +=  cos(jogo->listaTiros->fila[i]->posicao->angulo*ALLEGRO_PI/180 ) * 15;
                
                if(verificaPosicao(jogo,jogo->listaTiros->fila[i]->posicao)){
                    tiro = jogo->listaTiros->fila[i];
                    al_lock_mutex(jogo->listaAsteroids->mutex);
                    for(int j = 0 ; j < jogo->listaAsteroids->qt; j++){
                        asteroide = jogo->listaAsteroids->fila[j];
                        float distancia = sqrt(pow(tiro->posicao->dx-asteroide->posicao->dx,2)+ pow(tiro->posicao->dy-asteroide->posicao->dy,2));
                      
                        if(distancia < tiro->posicao->x+tiro->posicao->y ){
                            jogo->jogador->pontos++;
                            foi = 1;

                            break;
                        }
                    
                    }
                    if(foi){
                        asteroide->id = -1;
                    }
         
                    al_unlock_mutex(jogo->listaAsteroids->mutex);
                    if(!foi){
                        jogo->desenhaRotacionadoBitMap(jogo->listaTiros->fila[i]->imagem,
                                            jogo->listaTiros->fila[i]->posicao->x-24,
                                            (jogo->listaTiros->fila[i]->posicao->y/2)-7,
                                            jogo->listaTiros->fila[i]->posicao->dx,
                                            jogo->listaTiros->fila[i]->posicao->dy,
                                            jogo->listaTiros->fila[i]->posicao->angulo*ALLEGRO_PI/180,
                                            jogo->listaTiros->fila[i]->flags);
                    
                    }
                }else{
                    al_lock_mutex(jogo->listaTiros->mutex);
                    jogo->listaTiros->fila[i]->id = -1;
                    al_unlock_mutex(jogo->listaTiros->mutex);
                }
            }
            
            al_set_target_bitmap(al_get_backbuffer(jogo->janela));
            
            al_draw_bitmap(buffer,0,0,0);
            al_destroy_bitmap(buffer);
            
            jogo->listaTiros->removerDesativados(jogo->listaTiros);
            
    }            
}
void desenhaAsteroides(Jogo *jogo){

    if(jogo->listaAsteroids->qt > 0){
            ALLEGRO_BITMAP *buffer = NULL;
            buffer = al_create_bitmap(jogo->largura,jogo->altura);
            al_set_target_bitmap(buffer);
            al_draw_bitmap(al_get_backbuffer(jogo->janela), 0, 0, 0);
            //al_draw_bitmap(jogo->fundo,0,0,0);                
                
            al_lock_mutex(jogo->listaAsteroids->mutex);                
            for(int i = 0 ; i < jogo->listaAsteroids->qt ; i++){
                
                jogo->listaAsteroids->fila[i]->posicao->dy +=  sin(jogo->listaAsteroids->fila[i]->posicao->angulo*ALLEGRO_PI/180 ) * 3;
                jogo->listaAsteroids->fila[i]->posicao->dx +=  cos(jogo->listaAsteroids->fila[i]->posicao->angulo*ALLEGRO_PI/180 ) * 3;
                
                if(verificaPosicao(jogo,jogo->listaAsteroids->fila[i]->posicao)){
             
            
                        
                        jogo->desenhaRotacionadoBitMap(jogo->listaAsteroids->fila[i]->imagem,
                                            jogo->listaAsteroids->fila[i]->posicao->x,
                                            jogo->listaAsteroids->fila[i]->posicao->y,
                                            jogo->listaAsteroids->fila[i]->posicao->dx,
                                            jogo->listaAsteroids->fila[i]->posicao->dy,
                                            jogo->listaAsteroids->fila[i]->posicao->angulo*ALLEGRO_PI/180,
                                            jogo->listaAsteroids->fila[i]->flags);
                    
                        
                
                }else{
                    
                    jogo->listaAsteroids->fila[i]->id = -1;
                    
                }
            }
            al_unlock_mutex(jogo->listaAsteroids->mutex);
            al_set_target_bitmap(al_get_backbuffer(jogo->janela));
            
            al_draw_bitmap(buffer,0,0,0);
            al_destroy_bitmap(buffer);
            
            jogo->listaAsteroids->removerDesativados(jogo->listaAsteroids);
            
    }            
}

