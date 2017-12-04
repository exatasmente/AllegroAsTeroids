#include "includes.h"


void desenhaNave(Jogo *jogo);
void desenhaTiros(Jogo *jogo);
void desenhaInterface(Jogo *jogo);
void desenhaAsteroides(Jogo *jogo);

ALLEGRO_SAMPLE *explosaoSample;
int main(){
    Jogo *jogo;
    Jogador *jogador;
    jogo = novoJogo(800,600);
    initJogo(jogo);
    mutex  = al_create_mutex();
    explosaoSample = al_load_sample("explosao.wav");
    ALLEGRO_BITMAP *sprites[2];
    sprites[0] = al_load_bitmap("sprite.png");
    sprites[1] = al_load_bitmap("sprite1.png");
    jogador = initJogador(initCoordenada(24,24,320,240,0),3,0,sprites);
    jogo->jogador = jogador;
    ALLEGRO_THREAD *threadTeclado = al_create_thread(&teclado,jogo);
    al_start_thread(threadTeclado);
   while(jogo->sair){
        desenhaInterface(jogo);
        desenhaNave(jogo);
        desenhaTiros(jogo);
        desenhaAsteroides(jogo);
        if(jogo->jogador->vidas == 0){
            jogo->sair = 0;
        }   
        
    }
    al_destroy_sample(explosaoSample);
    al_destroy_thread(threadTeclado);
    finaliza(jogo);
    return 0;
}
void desenhaInterface(Jogo *jogo){
    ALLEGRO_BITMAP *buffer = NULL;
    buffer = al_create_bitmap(jogo->largura,jogo->altura);
    al_set_target_bitmap(buffer);
    al_draw_bitmap(al_get_backbuffer(jogo->janela), 0, 0, 0);                
    al_draw_textf(jogo->fonte, al_map_rgb(255, 255, 255), 10, 10, ALLEGRO_ALIGN_LEFT,"PONTOS : %d", jogo->jogador->pontos);
    al_draw_textf(jogo->fonte, al_map_rgb(255, 255, 255), jogo->largura-200, 10, ALLEGRO_ALIGN_LEFT," VIDAS:  %d", jogo->jogador->vidas);
    atualiza();        
    al_set_target_bitmap(al_get_backbuffer(jogo->janela));
            
    al_draw_bitmap(buffer,0,0,0);
    al_destroy_bitmap(buffer);    
}
void desenhaNave(Jogo *jogo){
    if(jogo->listaDesenho->qt > 0){        
        int colisao = 0;
        ALLEGRO_BITMAP *buffer = NULL;
        buffer = al_create_bitmap(jogo->largura,jogo->altura);
        al_set_target_bitmap(buffer);
        al_draw_bitmap(jogo->fundo,0,0,0); 
        
        Desenho *asteroide;
        Desenho *desenho;
        al_lock_mutex(jogo->listaDesenho->mutex);            
        desenho = removerDesenho(jogo->listaDesenho,0);
        al_lock_mutex(jogo->listaAsteroids->mutex);
        for(int j = 0 ; j < jogo->listaAsteroids->qt; j++){
            asteroide = jogo->listaAsteroids->fila[j];
            float distancia = sqrt(pow(desenho->posicao->dx-asteroide->posicao->dx,2)+ pow(desenho->posicao->dy-asteroide->posicao->dy,2));
            if(distancia < desenho->posicao->x+desenho->posicao->y ){
                colisao = 1;
                break;
            }
                   
        }
        if(colisao){
            asteroide->id = -1;
        }
        al_unlock_mutex(jogo->listaAsteroids->mutex);
        al_unlock_mutex(jogo->listaDesenho->mutex);            
        if(!colisao){
            if(verificaPosicao(jogo,desenho->posicao)){
                al_draw_rotated_bitmap(desenho->imagem,
                                       desenho->posicao->x,
                                       desenho->posicao->y,
                                       desenho->posicao->dx,
                                       desenho->posicao->dy,
                                       desenho->posicao->angulo*ALLEGRO_PI/180,
                                       desenho->flags);
                
                
                al_set_target_bitmap(al_get_backbuffer(jogo->janela));
                        
                al_draw_bitmap(buffer,0,0,0);
                al_destroy_bitmap(buffer);
                
            }
        }else{
            jogo->jogador->vidas -=1;
            al_draw_text(jogo->fonte, al_map_rgb(255, 0, 0), (jogo->largura/2)-150, jogo->altura/2, ALLEGRO_ALIGN_LEFT,"VOCE MORREU");
            al_set_target_bitmap(al_get_backbuffer(jogo->janela));
            al_draw_bitmap(buffer,0,0,0);
            atualiza();
            al_destroy_bitmap(buffer);
            
            
        }
        asteroide = NULL;
        desenho = NULL;
    
    }            

}
void desenhaTiros(Jogo *jogo){

    if(jogo->listaTiros->qt > 0){
            ALLEGRO_BITMAP *buffer = NULL;
            buffer = al_create_bitmap(jogo->largura,jogo->altura);
            al_set_target_bitmap(buffer);
            al_draw_bitmap(al_get_backbuffer(jogo->janela), 0, 0, 0);
                
            Desenho *asteroide;
            Desenho *tiro;
            int colisao = 0;
            for(int i = 0 ; i < jogo->listaTiros->qt ; i++){
                tiro = jogo->listaTiros->fila[i];
                tiro->posicao->dy +=  sin(tiro->posicao->angulo*ALLEGRO_PI/180 ) * 15;
                tiro->posicao->dx +=  cos(tiro->posicao->angulo*ALLEGRO_PI/180 ) * 15;
                
                if(verificaPosicao(jogo,tiro->posicao)){
                    
                    al_lock_mutex(jogo->listaAsteroids->mutex);
                    for(int j = 0 ; j < jogo->listaAsteroids->qt; j++){
                        asteroide = jogo->listaAsteroids->fila[j];
                        if(asteroide->id >= 0){
                            float distancia = sqrt(pow(tiro->posicao->dx-asteroide->posicao->dx,2)+ pow(tiro->posicao->dy-asteroide->posicao->dy,2));
                      
                            if(distancia < tiro->posicao->x+tiro->posicao->y ){
                                jogo->jogador->pontos++;
                                colisao = 1;
                                break;
                            }
                        }
                    
                    }
                    if(colisao){
                        al_play_sample(explosaoSample, 1.0, 0.0, 1.8, ALLEGRO_PLAYMODE_ONCE, NULL);
                        asteroide->id = -2;
                        al_lock_mutex(jogo->listaTiros->mutex);
                        tiro->id = -1;
                        
                        al_unlock_mutex(jogo->listaTiros->mutex);
                        colisao = 0;
                    }else{
                        al_draw_rotated_bitmap(tiro->imagem,
                                            tiro->posicao->x-6,
                                            (tiro->posicao->y/2),
                                            tiro->posicao->dx,
                                            tiro->posicao->dy,
                                            tiro->posicao->angulo*ALLEGRO_PI/180,
                                            tiro->flags);
                        
                    }   
                    al_unlock_mutex(jogo->listaAsteroids->mutex);
                }else{
                    al_lock_mutex(jogo->listaTiros->mutex);
                    tiro->id = -1;
                    
                    al_unlock_mutex(jogo->listaTiros->mutex);
                }
            }
            al_set_target_bitmap(al_get_backbuffer(jogo->janela));
            al_draw_bitmap(buffer,0,0,0);
            al_destroy_bitmap(buffer);
            asteroide = NULL;
            tiro = NULL;
            removerDesativados(jogo->listaTiros);
            
    }            
}
void desenhaAsteroides(Jogo *jogo){

    if(jogo->listaAsteroids->qt > 0){
            ALLEGRO_BITMAP *buffer = NULL;
            buffer = al_create_bitmap(jogo->largura,jogo->altura);
            al_set_target_bitmap(buffer);
            al_draw_bitmap(al_get_backbuffer(jogo->janela), 0, 0, 0);
            
            Desenho *asteroide;
            al_lock_mutex(jogo->listaAsteroids->mutex);                
            for(int i = 0 ; i < jogo->listaAsteroids->qt ; i++){
                asteroide = jogo->listaAsteroids->fila[i];
                if(asteroide->id >= 0){
                    asteroide->posicao->dy +=  sin(asteroide->posicao->angulo*ALLEGRO_PI/180 ) * 4;
                    asteroide->posicao->dx +=  cos(asteroide->posicao->angulo*ALLEGRO_PI/180 ) * 4;
                }    
                if(verificaPosicao(jogo,asteroide->posicao) && asteroide->id >= 0){
                    al_draw_rotated_bitmap(asteroide->imagem,
                                            asteroide->posicao->x,
                                            asteroide->posicao->y,
                                            asteroide->posicao->dx,
                                            asteroide->posicao->dy,
                                            asteroide->posicao->angulo*ALLEGRO_PI/180,
                                            asteroide->flags);
                   
                        
                
                }else if(asteroide->id == -2){
                    if(asteroide->posExplosao < 12){
                        ALLEGRO_BITMAP *explosao = al_load_bitmap(d[asteroide->posExplosao]);
                    
    
                        al_draw_bitmap(explosao,
                                        asteroide->posicao->dx,
                                        asteroide->posicao->dy,
                                        asteroide->flags);
                        asteroide->posExplosao++;
                        al_destroy_bitmap(explosao);
                    }else{
                        asteroide->id = -1;
                    }
                }else{
                    asteroide->id = -1;
                }
                
            }
            al_unlock_mutex(jogo->listaAsteroids->mutex);
            al_set_target_bitmap(al_get_backbuffer(jogo->janela));
            
            al_draw_bitmap(buffer,0,0,0);
            al_destroy_bitmap(buffer);
            asteroide = NULL;
            removerDesativados(jogo->listaAsteroids);
            
    }            
}

