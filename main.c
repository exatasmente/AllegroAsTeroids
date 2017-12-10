#include "includes.h"
// estrutura utilizada pra exebição e ordenação do ranking
typedef struct ranking{
    char *nome[3];
    int pontos;
}Ranking;

//variavél utilizada para aramzenar o som da explosão
ALLEGRO_SAMPLE *explosaoSample;

//assinatura das funções
void desenhaNave(Jogo *jogo);
void desenhaTiros(Jogo *jogo);
void desenhaInterface(Jogo *jogo);
void desenhaAsteroides(Jogo *jogo);
void exibeRanking(Jogo *jogo);

int main(){
    // Instância as variavéis de prioridade 1
    Jogo *jogo;
    Jogador *jogador;
    // Inicializa a variavél jogo 
    jogo = novoJogo(800,600);
    initJogo(jogo);
    // define a dificuldade inicial
    velocidadeAsteroid = 4;
    // indica que o menu está ativo
    menu = true;
    // inicializa o mutex 
    mutex  = al_create_mutex();
    // carrega o som utilizado na explosão do asteroid
    explosaoSample = al_load_sample("Sons/explosao.wav");
    // Instância um vetor com dois ponteiros para uma Estrutura do tipo ALLEGRO_BITMAP
    ALLEGRO_BITMAP *sprites[2];
    // carrega as imagens utilizadas na renderização da nve
    sprites[0] = al_load_bitmap("Sprites/Jogador/sprite.png");
    sprites[1] = al_load_bitmap("Sprites/Jogador/sprite1.png");
    // inicializa a variavél jogador 
    jogador = initJogador(initCoordenada(24,24,320,240,0),3,0,sprites);
    // define q o joador pertence à estrutura jogo
    jogo->jogador = jogador;
    //Instânica e inicializa a trhead que será usada para controle de eventos do jogo
    ALLEGRO_THREAD *threadTeclado = al_create_thread(&teclado,jogo);
    
    // loop inicial para exibir o menu do jogo
    mostraMenu(jogo);
    if(jogo->sair)
        al_start_thread(threadTeclado);


    // Loop principal do jogo
    while(jogo->sair){
        if(!menu){
            desenhaInterface(jogo);
            desenhaNave(jogo);
            desenhaTiros(jogo);
            desenhaAsteroides(jogo);
            if(jogo->jogador->vidas == 0){
        		menu = true;
                controleAddRankig(jogo);
	            jogador = initJogador(initCoordenada(24,24,320,240,0),3,0,sprites);
		        jogo->jogador= jogador;
		        velocidadeAsteroid = 4;
		        mostraMenu(jogo);
		        menu = false;
            }   
            
        }
    }
    // libera o espaço de memória utilizado pelo som e pela thread
    al_destroy_sample(explosaoSample);
    al_destroy_thread(threadTeclado);
    // chamada pro procedimento para finalizar o jogo
    finaliza(jogo);
    return 0;
}
void mostraMenu(Jogo *jogo){
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

}
void desenhaInterface(Jogo *jogo){
    /*
    Procedimento responsavél por exibir a pontuação e a vida do jogador na tela.
    Tal processo consiste em desenhar os valores em uma estrutura do tipo ALLEGRO_BITMAP
    e depois exibir tal bitmap na tela
    */
    // Instância uma váriavél do tipo ALLEGRO_BITMAP
    ALLEGRO_BITMAP *buffer = NULL;
    // inicializar a variavél difinindo as dimenções do bitmap 
    buffer = al_create_bitmap(jogo->largura,jogo->altura);
    // define o bitmap "buffer" como alvo para renderizar 
    al_set_target_bitmap(buffer);
    // desenha no bitmap "buffer" o que está sendo exibido na tela
    al_draw_bitmap(al_get_backbuffer(jogo->janela), 0, 0, 0);     
    // desenha os textos indicando os pontos do jogador e as vidas restantes do mesmo           
    al_draw_textf(jogo->fonte, al_map_rgb(255, 255, 255), 10, 10, ALLEGRO_ALIGN_LEFT,"PONTOS : %d", jogo->jogador->pontos);
    al_draw_textf(jogo->fonte, al_map_rgb(255, 255, 255), jogo->largura-200, 10, ALLEGRO_ALIGN_LEFT," VIDAS:  %d", jogo->jogador->vidas);
    // atualiza o bitmap com os desenho definidos
    atualiza();
    // define a tela com área de desenho
    al_set_target_bitmap(al_get_backbuffer(jogo->janela));
    // desenha o bitmap "buffer" na tela q será exibida ao usuário
    al_draw_bitmap(buffer,0,0,0);
    // libera o espaço de memória utlizado pela variavél "buffer"
    al_destroy_bitmap(buffer);    
    
}
void desenhaNave(Jogo *jogo){
    //verifica se exixtem desenhos à serem renderizados 
    if(jogo->listaDesenho->qt > 0){        
        // define a variavél de sinal para colisão da nave com os asteroids com falso
        int colisao = 0;
        //Instância uma variavél do tipo ALLEGRO_BITMAP
        ALLEGRO_BITMAP *buffer = NULL;
        //inicializa a variavél "buffer"
        buffer = al_create_bitmap(jogo->largura,jogo->altura);
        // define o bitmap "buffer" como alvo para renderizar 
        al_set_target_bitmap(buffer);
        // desenha na no bitmap "buffer" a imagem de fundo do jogo
        al_draw_bitmap(jogo->fundo,0,0,0); 

        //Instância duas variavéis de auxilo para desenho ta tela
        Desenho *asteroide;
        Desenho *desenho;
        // bloqueia a regiao de memória da lista de desenhos relacionado à nave do jogador
        al_lock_mutex(jogo->listaDesenho->mutex);            
        // retira um desenho da lista de desenhos 
        desenho = removerDesenho(jogo->listaDesenho,0);
        // bloqueia a regiao de memória da lista de desenhos relacionado aos asteroids 
        al_lock_mutex(jogo->listaAsteroids->mutex);
        // laco de repetição para verificar se hà colisão com os asteroids exibidos na tela 
        for(int j = 0 ; j < jogo->listaAsteroids->qt; j++){
            //Detecta Colisão
            asteroide = jogo->listaAsteroids->fila[j];
            //Colisão usando o método de distância
            float distancia = sqrt(pow(desenho->posicao->dx-asteroide->posicao->dx,2)+ pow(desenho->posicao->dy-asteroide->posicao->dy,2));
            if(distancia < desenho->posicao->x+desenho->posicao->y ){
                colisao = 1; 
                break;
            }
            
        }
        // asteroid tem q ser removido
        if(colisao){
            asteroide->id = -1;
        }
        // desbloqueia dos endereços de memória bloqueados anteriormente
        al_unlock_mutex(jogo->listaAsteroids->mutex);
        al_unlock_mutex(jogo->listaDesenho->mutex);            
        // Caso não haja colisão a nave será renderizada no bitmap definido anteriormente "buffer"
        if(!colisao){
            //Verifica se a posição atual do jogador está dentro da tela
            if(verificaPosicao(jogo,desenho->posicao)){
                //desenha no bitmap "buffer" o sprite da nave na coordenada passada como parânmetro
                al_draw_rotated_bitmap(desenho->imagem,
                                       desenho->posicao->x,
                                       desenho->posicao->y,
                                       desenho->posicao->dx,
                                       desenho->posicao->dy,
                                       desenho->posicao->angulo*ALLEGRO_PI/180,
                                       desenho->flags);
                
                // troca o bitmap padrão para o bitimap da tela
                al_set_target_bitmap(al_get_backbuffer(jogo->janela));
                // desenha o bitmap "buffer" na tela
                al_draw_bitmap(buffer,0,0,0);
                // libera o endereço de memória utilizado pela variavél "buffer"
                al_destroy_bitmap(buffer);
                
            }
        }else{
            // caso haja colisão a vida do jogador é decrementada
            jogo->jogador->vidas -= 1;

        }
        
        asteroide = NULL;
        desenho = NULL;
        
    }            

}
void desenhaTiros(Jogo *jogo){
    // verifica se exixtem desenhos à serem renderizados 
    if(jogo->listaTiros->qt > 0){
            //Instância uma variavél do tipo ALLEGRO_BITMAP
            ALLEGRO_BITMAP *buffer = NULL;
            //inicializa a variavél "buffer"
            buffer = al_create_bitmap(jogo->largura,jogo->altura);
            // define o bitmap "buffer" como alvo para renderizar 
            al_set_target_bitmap(buffer);
            // desenha na no bitmap "buffer" o que está sendo exibido na tela
            al_draw_bitmap(al_get_backbuffer(jogo->janela), 0, 0, 0);
            //Instância duas variavéis de auxilo para desenho ta tela                
            Desenho *asteroide;
            Desenho *tiro;

            //define a variavél de sinal para colisão como false
            int colisao = 0;
            //laco de repetição para verificar cada tiro à sder exibido na tela
            for(int i = 0 ; i < jogo->listaTiros->qt ; i++){
                // Atualiza Posição dos tiros
                tiro = jogo->listaTiros->fila[i];
                tiro->posicao->dy +=  sin(tiro->posicao->angulo*ALLEGRO_PI/180 ) * 15;
                tiro->posicao->dx +=  cos(tiro->posicao->angulo*ALLEGRO_PI/180 ) * 15;
                // verifica se a posição do tiro à ser renderizado está dentro da tela
                if(verificaPosicao(jogo,tiro->posicao)){
                    // bloquiea a endereço de menória utilizada pela lista de asteroids
                    al_lock_mutex(jogo->listaAsteroids->mutex);
                    // laco de repetição para para verificar se os asteroids foram atingidos pelo tiro atual
                    for(int j = 0 ; j < jogo->listaAsteroids->qt; j++){
                        asteroide = jogo->listaAsteroids->fila[j];
                        // caso o id dos asteroid seja diferente dos valores -1 e -2 
                        if(asteroide->id >= 0){
                            //Detecta Colisão
                            float distancia = sqrt(pow(tiro->posicao->dx-asteroide->posicao->dx,2)+ pow(tiro->posicao->dy-asteroide->posicao->dy,2));
                            // condição para a colisão ser verdadeira
                            if(distancia < tiro->posicao->x+tiro->posicao->y ){
                                //incrementa q quntidade de pontos do jogador
                                jogo->jogador->pontos++;
                                // altera o valor da variavél de controle "colisao" para verdadeiro
                                colisao = 1;
                                // para a iteração
                                break;
                            }
                            
                        }
                    
                    }
                    if(colisao){
                        // Reproduz o som da explosão do jogo
                        al_play_sample(explosaoSample, 1.0, 0.0, 1.8, ALLEGRO_PLAYMODE_ONCE, NULL);
                        // define que o asteroid foi atingido
                        asteroide->id = -2;
                        //Bloquiea do endereço de memória utlizado abaixo
                        al_lock_mutex(jogo->listaTiros->mutex);
                        // define q o tiro tem que ser removido da lista de tiros
                        tiro->id = -1;
                        // desbloqueia do endereçco de memória utilizado acima
                        al_unlock_mutex(jogo->listaTiros->mutex);
                        // define a variavé de colisão como falso
                        colisao = 0;
                    }else{
                        // caso não haja colisão  desenho o tiro na tela
                        al_draw_rotated_bitmap(tiro->imagem,
                                            tiro->posicao->x-6,
                                            (tiro->posicao->y/2),
                                            tiro->posicao->dx,
                                            tiro->posicao->dy,
                                            tiro->posicao->angulo*ALLEGRO_PI/180,
                                            tiro->flags);
                        
                    }
                    // desbloqueia o endereco de memória utilizado 
                    al_unlock_mutex(jogo->listaAsteroids->mutex);
                }else{
                    // bloquiea o endereco de memória
                    al_lock_mutex(jogo->listaTiros->mutex);
                    // defina q o tiro tem q ser removido
                    tiro->id = -1;
                    // desbloquiea o endereco de memória
                    al_unlock_mutex(jogo->listaTiros->mutex);
                }
            }
            // troca o bitmap para o bitmap da tela
            al_set_target_bitmap(al_get_backbuffer(jogo->janela));
            // desenha o bitamap "buffer" na tela
            al_draw_bitmap(buffer,0,0,0);
            // libera o espaço de memória utilizado pela variavél buffer
            al_destroy_bitmap(buffer);
            asteroide = NULL;
            tiro = NULL;
            // chamada do procedimento para remover os tiros destivados na lista de tiros
            removerDesativados(jogo->listaTiros);
            
    }            
}
void desenhaAsteroides(Jogo *jogo){
    //verifica se existem asteroids para serem desenhados
    if(jogo->listaAsteroids->qt > 0){
            // instância uma variavél do tipo ALLEGRO_BITMAP
            ALLEGRO_BITMAP *buffer = NULL;
            // inicializa a variavél instÇanciada acima com as dimenssões da tela principal
            buffer = al_create_bitmap(jogo->largura,jogo->altura);
            // define que o bitmap "buffer" será utlizado para as proximas renderizações
            al_set_target_bitmap(buffer);
            // desenha o que está na tela atual no bitmap "buffer"
            al_draw_bitmap(al_get_backbuffer(jogo->janela), 0, 0, 0);
            //variavél auxiliar
            Desenho *asteroide;
            //bloqueia o endereço de memória
            al_lock_mutex(jogo->listaAsteroids->mutex);                
            // laco de repetição para percorrer a lista de asteroids
            for(int i = 0 ; i < jogo->listaAsteroids->qt ; i++){
                asteroide = jogo->listaAsteroids->fila[i];
                // caso a asteroide seja válido
                if(asteroide->id >= 0){
                    // atualiza a posição dele
                    asteroide->posicao->dy +=  sin(asteroide->posicao->angulo*ALLEGRO_PI/180 ) * velocidadeAsteroid;
                    asteroide->posicao->dx +=  cos(asteroide->posicao->angulo*ALLEGRO_PI/180 ) * velocidadeAsteroid;
                }    
                // caso o asteroid esteja dentro da tela e não seja inválido
                if(verificaPosicao(jogo,asteroide->posicao) && asteroide->id >= 0){
                    // desenha o asteroid na tela
                    al_draw_rotated_bitmap(asteroide->imagem,
                                            asteroide->posicao->x,
                                            asteroide->posicao->y,
                                            asteroide->posicao->dx,
                                            asteroide->posicao->dy,
                                            asteroide->posicao->angulo*ALLEGRO_PI/180,
                                            asteroide->flags);
                   
                        
                //caso contrário se o asteroid tenha sido  atringido por um tiro
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
                    
                    }else{
                        // define q o asteroid tem q ser removido da lista
                        asteroide->id = -1;
                    }
                }else{
                    // define q o asteroid tem q ser removido da lista
                    asteroide->id = -1;
                }
                
            }
            //desbloqueia os endereços de memória acima
            al_unlock_mutex(jogo->listaAsteroids->mutex);
            // troca o bitmap para o bitmap da tela
            al_set_target_bitmap(al_get_backbuffer(jogo->janela));
            // desenha o bitmap "buffer" na tela prinicpal
            al_draw_bitmap(buffer,0,0,0);
            // libera o espaçõ de memória utilizado pela variavél "buffer"
            al_destroy_bitmap(buffer);
            asteroide = NULL;
            // chamada do pocedimento para remover ao asteroides com id -1
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
    if(arquivo == NULL){
	arquivo = fopen("ranking.rk","wb");
    }

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
