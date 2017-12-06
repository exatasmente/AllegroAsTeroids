#include "includes.h"

typedef struct ranking{
    char *nome[3];
    int pontos;
}Ranking;


void desenhaNave(Jogo *jogo);
void desenhaTiros(Jogo *jogo);
void desenhaInterface(Jogo *jogo);
void desenhaAsteroides(Jogo *jogo);

ALLEGRO_SAMPLE *explosaoSample;
int main(){
    Jogo *jogo;
    Jogador *jogador;
    menu = true;
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

    while(menu){
        controleMenu(jogo);
        switch(opcao){
            case 0:
                menu = false;
                break;
            case 1:
                exibeRanking(jogo);
                break;
            case 2:
                jogo->sair = 0;
                menu = false;
                break;

        }
    }
    if(jogo->sair)
        al_start_thread(threadTeclado);
    
    while(jogo->sair){
        if(!menu){
            desenhaInterface(jogo);
            desenhaNave(jogo);
            desenhaTiros(jogo);
            desenhaAsteroides(jogo);
            if(jogo->jogador->vidas == 0){
                controleAddRankig(jogo);
                jogo->sair = 0;
            }   
            
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

            //Detecta Colisão
            asteroide = jogo->listaAsteroids->fila[j];
            float distancia = sqrt(pow(desenho->posicao->dx-asteroide->posicao->dx,2)+ pow(desenho->posicao->dy-asteroide->posicao->dy,2));
            if(distancia < desenho->posicao->x+desenho->posicao->y ){
                colisao = 1; 
                break;
            }
            ////
        }
        // asteroid tem q ser removido
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
                // Atualiza Posição
                tiro = jogo->listaTiros->fila[i];
                tiro->posicao->dy +=  sin(tiro->posicao->angulo*ALLEGRO_PI/180 ) * 15;
                tiro->posicao->dx +=  cos(tiro->posicao->angulo*ALLEGRO_PI/180 ) * 15;
                
                if(verificaPosicao(jogo,tiro->posicao)){
                    al_lock_mutex(jogo->listaAsteroids->mutex);
                    for(int j = 0 ; j < jogo->listaAsteroids->qt; j++){
                        asteroide = jogo->listaAsteroids->fila[j];
                        if(asteroide->id >= 0){

                            //Detecta Colisção
                            float distancia = sqrt(pow(tiro->posicao->dx-asteroide->posicao->dx,2)+ pow(tiro->posicao->dy-asteroide->posicao->dy,2));
                      
                            if(distancia < tiro->posicao->x+tiro->posicao->y ){
                                jogo->jogador->pontos++;
                                colisao = 1;
                                break;
                            }
                            ///////
                        }
                    
                    }
                    if(colisao){
                        // Reproduz o som da explosão do jogo
                        al_play_sample(explosaoSample, 1.0, 0.0, 1.8, ALLEGRO_PLAYMODE_ONCE, NULL);
                        // define que o asteroid foi atingido
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
                    // Desenha os sprites da animação da explosão
                    if(asteroide->posExplosao < 12){
                        ALLEGRO_BITMAP *explosao = al_load_bitmap(d[asteroide->posExplosao]);
                    
    
                        al_draw_bitmap(explosao,
                                        asteroide->posicao->dx,
                                        asteroide->posicao->dy,
                                        asteroide->flags);
                        asteroide->posExplosao++;
                        al_destroy_bitmap(explosao);
                    ////
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

void exibeRanking(Jogo *jogo){
    ALLEGRO_BITMAP *buffer = NULL;
    FILE *arquivo;
    Ranking *vetor[50];
    buffer = al_create_bitmap(jogo->largura,jogo->altura);
    al_set_target_bitmap(buffer);
    al_draw_bitmap(jogo->fundo, 0, 0, 0);                
    arquivo = fopen("ranking.rk","rb");
    int ch;
    int qt = 0;
    while((ch = getc(arquivo)) != EOF){
        if(ch =='\n'){
            qt++;
        }
    }
    qt++;
    rewind(arquivo);
    for(int i = 0 ; i < qt ; i++){
        vetor[i] = (Ranking*) malloc(sizeof(Ranking));
        fscanf(arquivo,"%s\t%d\n",vetor[i]->nome,&vetor[i]->pontos);
    }
    fclose(arquivo);

    for(int i = 0 ; i < qt ; i++){
        for(int j = i ; j < qt ; j++){
            if(vetor[i]->pontos < vetor[j]->pontos){
                Ranking *aux = (Ranking*)malloc(sizeof(Ranking));
                for(int s = 0 ; s < 3 ; s++)
                    aux->nome[s] = vetor[i]->nome[s];

                aux->pontos = vetor[i]->pontos;
                vetor[i] = vetor[j];
                vetor[j] = aux;
            }
        }
    }

    int auxY = 140;
    al_draw_textf(jogo->fonte, al_map_rgb(255, 255, 255), (jogo->largura/2)-80, 100, ALLEGRO_ALIGN_LEFT,"JRD : %d",INT_MAX);
    for(int i = 0 ; i < (qt > 6 ? 6 : qt) ; i++){
        al_draw_textf(jogo->fonte, al_map_rgb(255, 255, 255), (jogo->largura/2)-80,auxY, ALLEGRO_ALIGN_LEFT,"%s : %d",vetor[i]->nome,vetor[i]->pontos);
        auxY += 40;
    }

    al_set_target_bitmap(al_get_backbuffer(jogo->janela));
    al_draw_bitmap(buffer,0,0,0);
    al_destroy_bitmap(buffer);   
    atualiza(); 
    while(true){
        ALLEGRO_EVENT evento;
        al_wait_for_event(jogo->filaEventos, &evento);
        if(evento.type == ALLEGRO_EVENT_KEY_DOWN){
            if(evento.keyboard.keycode == ALLEGRO_KEY_SPACE) {
                break;
            }
        }
    }
    menu = true;

}


 
