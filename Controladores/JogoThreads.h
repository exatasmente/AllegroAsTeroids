enum TECLAS {
   KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_SPACE
};

// variavéis de controle
bool menu;
int opcao;
int opcao2;
int desenha;
int velocidadeAsteroid;
int teclas[5] = {0,0,0,0,0};

// assinatura dos procedimentos
void * teclado(ALLEGRO_THREAD *thread,void *param);
void atualizaPosicao(Coordenada *posicao);
void atualizaDesenhos(ListaDesenho *lista,ALLEGRO_BITMAP *sprite , Coordenada *posicao);
void controleMenu(Jogo *jogo);
void controleAddRankig(Jogo *jogo);

void atualizaPosicao(Coordenada *posicao){
    /*
    Tem o dever de atualizar a coordenada atual do jogador baseado nas teclas pressionadas
    */
    if(teclas[KEY_UP]){
        posicao->dy +=  sin(posicao->angulo*ALLEGRO_PI/180) * 5;
        posicao->dx +=  cos(posicao->angulo*ALLEGRO_PI/180) * 5;
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
    /*
    Responsavél por inserir um desenho em uma das listas de desenhos utilizadas pelo programa
    */
    Desenho *desenho = novoDesenho(sprite,posicao,0);
    addDesenho(lista,desenho);
    desenho = NULL;
    
}
void * teclado(ALLEGRO_THREAD *thread,void *param){
    /*
    Procedimento de controle localizado entre a borda e os modelos 
    Procedimento chamado para ser rodado em uma thread
    Responsavél por escutar os eventos do teclado e do timer
    Assim controlando o que deve ser desenhado na tela
    */
    // variavéis auxiliares 
    Jogo *jogo = (Jogo*) param;
    Jogador *jogador = jogo->jogador;
    int lado = 0;
    //inicializa o rand
    srand(time(NULL));
    //variavéis de verificação de intervalo de tempo
    time_t tinicio, tfim;
    time_t dificuldade;
    //armazena o tempo atual da máquina nas duas variavéis abaixo
    time(&tinicio);
    time(&dificuldade);
    // instância e inicaializa a variavél "sample" com o som a ser reproduzido no disparo
    ALLEGRO_SAMPLE *sample = al_load_sample("Sons/tiro.wav");

    while(jogo->sair){
        //instância uma variavél do tipo ALLEGRO_EVENT    
        ALLEGRO_EVENT evento;
        //aguarda um evendo vindo do teclado ou de tempo e armazena tal evento na variavél declarada acima
        al_wait_for_event(jogo->filaEventos, &evento);
        //caso o evento seja do tipo timer
        if(evento.type == ALLEGRO_EVENT_TIMER &!menu){
            //a posição do jogador deve ser atualizada
            atualizaPosicao(jogador->posicao);
        // caso contrário se o evento seja to tipo tecla pressionada (ALLEGRO_EVENT_KEY_DOWN)
        }else if(evento.type == ALLEGRO_EVENT_KEY_DOWN ){
            //switch para saber qual tecla foi pressionada
            switch(evento.keyboard.keycode) {
                //caso seja seta para cima
                case ALLEGRO_KEY_UP:
                    teclas[KEY_UP] = 1; 
                    break;
                //caso seja seta para baixo
                case ALLEGRO_KEY_DOWN:
                    teclas[KEY_DOWN] = 1;
                    break;
                //caso seja seta para esquerda
                case ALLEGRO_KEY_LEFT: 
                    teclas[KEY_LEFT] = 1;
                    break;
                //caso seja seta para direita
                case ALLEGRO_KEY_RIGHT:
                    teclas[KEY_RIGHT] = 1;
                    break;
                case ALLEGRO_KEY_SPACE:
                    teclas[KEY_SPACE] = 1;    
                    break;
            }
            // define q não precisa ser desenhado na tela
            desenha = 0;
        //caso contrário se seja um evento de tecla solta
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
        // caso contrário se for um evento do tipo para fechar a janela
        }else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
            //define q o jogo deve ser fechado
            jogo->sair = 0;
            
        }
        // caso seja preciso desenha e a fila de eventos eteja vazia
        if(desenha && al_is_event_queue_empty(jogo->filaEventos) && !menu ){
            //define q não precisa ser desenhado pois já está dentro do bloco responsavél por isso
            desenha = 0;
            //caso a tecla espaço tenha sido pressionada
            if(teclas[KEY_SPACE]){
                //carrega o sprite do tiro
                ALLEGRO_BITMAP *sprite = al_load_bitmap("Sprites/Jogador/tiro.png");                
                //inicializa uma coordenada baseada na posição atual da nave na tela
                Coordenada *posicao = initCoordenada(6,12,jogador->posicao->dx,jogador->posicao->dy,jogador->posicao->angulo);
                // repoduz o som de tiro
                al_play_sample(sample, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                // bloqueia os endereços de memória abaixo
                al_lock_mutex(jogo->listaTiros->mutex);
                //chamada do procedimento para adicionar na lista de tiros o tiro instânciado acima
                atualizaDesenhos(jogo->listaTiros,sprite,posicao);
                //desbloquiea os endereços de memória acima
                al_unlock_mutex(jogo->listaTiros->mutex);

                sprite = NULL;
                
             
            }
            //bloqueia os endereços de memória abaixo
            al_lock_mutex(jogo->listaDesenho->mutex);
            //caso a tecla para cima esteja pressionda
            if(teclas[KEY_UP]){
                //chamada do procedimento para adicionar na lista desenhos da nave com o propulsor ativado
                atualizaDesenhos(jogo->listaDesenho,jogador->sprite[0],jogador->posicao);                
            }else{
                //chamada do procedimento para adicionar na lista desenhos da nave com o propulsor desativado
                atualizaDesenhos(jogo->listaDesenho,jogador->sprite[1],jogador->posicao);
            }
            //desbloquiea dos endereços de memória acima
            al_unlock_mutex(jogo->listaDesenho->mutex);
            //armazena o tempo atual na variavél abaixo
            time(&tfim);
            
            //calcula a diferença entre os tempos abaixo
            double diff = difftime(tfim,tinicio);
            double velocidade = difftime(tfim,dificuldade);
            //caso o tempo de velocidade dos asteroids seja maior que 15 segundos
            if(velocidade > 15.0 ){
                //incrementa a velocidade dos asteroids
                velocidadeAsteroid++;
                //armazena o tempo atual da máquina na variavél abaixo
                time(&dificuldade);
            }
            //caso o intevalo de um asteroid para o outro tenha sido atingido
            if(diff > 1){
                //carrega o sprite do asteroid 
                
                ALLEGRO_BITMAP *sprite = al_load_bitmap("Sprites/Asteroid/asteroid.png");
                Coordenada *posicao;
                al_lock_mutex(jogo->listaAsteroids->mutex);
                // verfica qual o ponto de partida q o asteroid vai ser renderizado
                switch(lado){
                    case 0:
                        posicao = initCoordenada(40,40,10,rand()%jogo->altura,rand()%360);
                        atualizaDesenhos(jogo->listaAsteroids,sprite,posicao);
                        lado = rand()%4;
                        break;
                    case 1:
                        posicao = initCoordenada(40,40,rand()%jogo->largura,10,rand()%360);
                        atualizaDesenhos(jogo->listaAsteroids,sprite,posicao);
                        lado = rand()%4;
                        break;
                    case 2:
                        posicao = initCoordenada(40,40,rand()%jogo->largura,jogo->altura,rand()%360);
                        atualizaDesenhos(jogo->listaAsteroids,sprite,posicao);
                        lado = rand()%4;
                        break;
                    case 3:
                        posicao = initCoordenada(40,40,jogo->largura,rand()%jogo->altura,rand()%360);
                        atualizaDesenhos(jogo->listaAsteroids,sprite,posicao);
                        lado = rand()%4;
                        break;
                }
                al_unlock_mutex(jogo->listaAsteroids->mutex);
                
                time(&tinicio);
                
            }
            
            //a thread descança por 0.05 segundos
            al_rest(0.05);
        }

     
    }
    //no fim do jogo o endereço de memória abaixo é liberado
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

void controleMenu(Jogo *jogo){
    ALLEGRO_BITMAP *buffer = NULL;
    ALLEGRO_COLOR escolha1 =  al_map_rgb(255, 0, 0);
    ALLEGRO_COLOR escolha0 =  al_map_rgb(255, 255, 255);

    buffer = al_create_bitmap(jogo->largura,jogo->altura);
    al_set_target_bitmap(buffer);
    al_draw_bitmap(jogo->fundo, 0, 0, 0);                
    al_draw_textf(jogo->fonte, al_map_rgb(255, 0, 0), (jogo->largura/2)-80, (jogo->altura/2), ALLEGRO_ALIGN_LEFT,"NOVO JOGO");
    al_draw_textf(jogo->fonte, al_map_rgb(255, 255, 255), (jogo->largura/2)-80, (jogo->altura/2)+40, ALLEGRO_ALIGN_LEFT,"RANKING"); 
    al_draw_textf(jogo->fonte, al_map_rgb(255, 255, 255), (jogo->largura/2)-80, (jogo->altura/2)+80, ALLEGRO_ALIGN_LEFT,"SAIR"); 
    al_set_target_bitmap(al_get_backbuffer(jogo->janela));
    al_draw_bitmap(buffer,0,0,0);
    al_destroy_bitmap(buffer);
    atualiza();

    while(menu){
        ALLEGRO_EVENT evento;
        al_wait_for_event(jogo->filaEventos, &evento);
        if(evento.type == ALLEGRO_EVENT_KEY_DOWN){
            buffer = NULL;
            buffer = al_create_bitmap(jogo->largura,jogo->altura);
            al_set_target_bitmap(buffer);
            
            switch(evento.keyboard.keycode) {
                case ALLEGRO_KEY_UP:
                    opcao = (opcao-1)%3;                
                    break;
                case ALLEGRO_KEY_DOWN:
                    opcao = (opcao+1)%3;
                    break;
                case ALLEGRO_KEY_SPACE:
                    menu = false;
                    break;
                default:
                    al_draw_bitmap(al_get_backbuffer(jogo->janela), 0, 0, 0);

            }
            al_draw_bitmap(jogo->fundo, 0, 0, 0);                
            al_draw_textf(jogo->fonte, (opcao == 0 ? escolha1 : escolha0 ),(jogo->largura/2)-80, (jogo->altura/2), ALLEGRO_ALIGN_LEFT,"NOVO JOGO");
            al_draw_textf(jogo->fonte, (opcao == 1 ? escolha1 : escolha0 ),(jogo->largura/2)-80, (jogo->altura/2)+40, ALLEGRO_ALIGN_LEFT,"RANKING"); 
            al_draw_textf(jogo->fonte, (opcao == 2 ? escolha1 : escolha0 ), (jogo->largura/2)-80, (jogo->altura/2)+80, ALLEGRO_ALIGN_LEFT,"SAIR"); 
                    

            al_set_target_bitmap(al_get_backbuffer(jogo->janela));
            al_draw_bitmap(buffer,0,0,0);
            al_destroy_bitmap(buffer);
            atualiza();
        }else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
            jogo->sair = 0;
            menu = false;
        }
     
    }
}
void controleAddRankig(Jogo *jogo){
    ALLEGRO_BITMAP *buffer = NULL;
    ALLEGRO_COLOR escolha1 =  al_map_rgb(255, 0, 0);
    ALLEGRO_COLOR escolha0 =  al_map_rgb(255, 255, 255);
    char valores[36] ={'A','B','C','D','E','F','G','H','I','J','K',
                        'L','M','N','O','P','Q','R','S','T','U','V',
                        'W','Y','X','Z','0','1','2','3','4','5','6','7','8','9'};
    
    int campos[3] = {0,0,0};
    menu = true;
    buffer = al_create_bitmap(jogo->largura,jogo->altura);
    al_set_target_bitmap(buffer);
    al_draw_bitmap(jogo->fundo, 0, 0, 0);                
    al_draw_bitmap(jogo->fundo, 0, 0, 0);                
    al_draw_textf(jogo->fonte, escolha0,(jogo->largura/2)-50, (jogo->altura/2), ALLEGRO_ALIGN_LEFT,"%c",valores[campos[0]]);
    al_draw_textf(jogo->fonte, escolha1,(jogo->largura/2), (jogo->altura/2), ALLEGRO_ALIGN_LEFT,"%c",valores[campos[1]]);
    al_draw_textf(jogo->fonte, escolha1,(jogo->largura/2)+50, (jogo->altura/2), ALLEGRO_ALIGN_LEFT,"%c",valores[campos[2]]);
    al_set_target_bitmap(al_get_backbuffer(jogo->janela));
    al_draw_bitmap(buffer,0,0,0);
    al_destroy_bitmap(buffer);
    atualiza();
    opcao = 0;
    while(menu){
        ALLEGRO_EVENT evento;
        al_wait_for_event(jogo->filaEventos, &evento);
        if(evento.type == ALLEGRO_EVENT_KEY_DOWN){
            buffer = NULL;
            buffer = al_create_bitmap(jogo->largura,jogo->altura);
            al_set_target_bitmap(buffer);
            
            switch(evento.keyboard.keycode) {
                case ALLEGRO_KEY_UP:
                    campos[opcao2] =  (campos[opcao2]+1)%36;  
                    break;
                case ALLEGRO_KEY_DOWN:
                    campos[opcao2] =  (campos[opcao2]-1)%36;  
                    break;
                case ALLEGRO_KEY_LEFT:
                    opcao2 = (opcao2-1)%3;                
                    break;
                case ALLEGRO_KEY_RIGHT:
                    opcao2 = (opcao2+1)%3;
                    break;
                case ALLEGRO_KEY_SPACE:
                    menu = false;
                    break;
                default:
                    al_draw_bitmap(al_get_backbuffer(jogo->janela), 0, 0, 0);

            }
            al_draw_bitmap(jogo->fundo, 0, 0, 0);                
            al_draw_textf(jogo->fonte, (opcao2 == 0 ? escolha0 : escolha1),(jogo->largura/2)-50, (jogo->altura/2), ALLEGRO_ALIGN_LEFT,"%c",valores[campos[0]]);
            al_draw_textf(jogo->fonte,(opcao2 == 1 ? escolha0 : escolha1),(jogo->largura/2), (jogo->altura/2), ALLEGRO_ALIGN_LEFT,"%c",valores[campos[1]]);
            al_draw_textf(jogo->fonte,(opcao2 == 2 ? escolha0 : escolha1),(jogo->largura/2)+50, (jogo->altura/2), ALLEGRO_ALIGN_LEFT,"%c",valores[campos[2]]);
            
            
                    

            al_set_target_bitmap(al_get_backbuffer(jogo->janela));
            al_draw_bitmap(buffer,0,0,0);
            al_destroy_bitmap(buffer);
            atualiza();
        }else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
            jogo->sair = 0;
            menu = false;
        }
     
    }
    FILE *arq = fopen("../ranking.rk","ab");
	
    fprintf(arq,"%c%c%c: %d\n",valores[campos[0]],valores[campos[1]],valores[campos[2]],jogo->jogador->pontos);
    fclose(arq);
    for(int i = 0 ; i < 5 ; i++){
        teclas[i] = 0;
    }
    exibeRanking(jogo);
}
