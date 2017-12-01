
#include "AlThreadsController.h"
#include "AlDesenhosController.h"

ALLEGRO_MUTEX *mutex;
ALLEGRO_COND  *cond;
ALLEGRO_BITMAP *tela;
typedef struct jogo{
    ALLEGRO_BITMAP* fundo;
    ALLEGRO_DISPLAY *janela;
    ALLEGRO_FONT *fonte;
    ALLEGRO_EVENT_QUEUE *filaEventos;
    ALLEGRO_TIMER *timer;
    ListaThreads *listaThreads;
    ListaDesenho *listaDesenho;
    ListaDesenho *listaTiros;
    ListaDesenho *listaAsteroids;
    Jogador *jogador;
    int largura;
    int altura;
    int sair;
    void (*inicializa)(struct jogo*);
    void (*finaliza)(struct jogo*);
    ALLEGRO_DISPLAY* (* novaJanela)(int,int);
    ALLEGRO_BITMAP* (* carregaImage)(const char*);
    void (*desenhaBitMap)(ALLEGRO_BITMAP*,float,float, int);
    void (*desenhaRotacionadoBitMap)(ALLEGRO_BITMAP*,float, float, float, float, float ,int);
    void (*atualiza)();
}Jogo;

void initJogo(Jogo *novo);
Jogo *novoJogo(int w,int h);
void finaliza();
void atualiza();

Jogo *novoJogo(int w,int h){
    Jogo *novo;
    novo = (Jogo*) malloc(sizeof(Jogo));
    novo->novaJanela = &al_create_display;
    novo->carregaImage = &al_load_bitmap;
    novo->finaliza = &finaliza;
    novo->inicializa = &initJogo;
    novo->desenhaBitMap = &al_draw_bitmap;
    novo->desenhaRotacionadoBitMap = &al_draw_rotated_bitmap;
    novo->atualiza = &atualiza;

    novo->largura = w;
    novo->altura = h;
    novo->sair = 1;
    return novo;
}
void initJogo(Jogo *novo){
    al_init();
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_install_keyboard();


    novo->timer = al_create_timer(1.0/60);
    novo->listaThreads = initListaThreads(10);
    novo->listaDesenho = initListaDesenho(10);
    novo->listaTiros = initListaDesenho(10);
    novo->listaAsteroids = initListaDesenho(50);
    novo->janela = novo->novaJanela(novo->largura,novo->altura);
    novo->filaEventos = al_create_event_queue();
    novo->fonte = al_load_font("ARCADE_R.TTF", 20, 0);

    al_register_event_source(novo->filaEventos, al_get_keyboard_event_source());
    al_register_event_source(novo->filaEventos, al_get_display_event_source(novo->janela));
    al_register_event_source(novo->filaEventos, al_get_timer_event_source(novo->timer));


    novo->fundo = novo->carregaImage("bg.jpeg");
    al_draw_bitmap(novo->fundo, 0, 0, 0);
    al_flip_display();
    al_start_timer(novo->timer);
 

    
}

void finaliza(Jogo *jogo){
    al_lock_mutex(mutex);
    
    if(jogo->listaThreads->qt > 0){
        destroyAlThread(jogo->listaThreads);
    }
    if(jogo->listaDesenho->qt > 0){
        destroyDesenho(jogo->listaDesenho);
    }
    al_destroy_font(jogo->fonte);
    al_destroy_event_queue(jogo->filaEventos);
    al_destroy_display(jogo->janela);
    
    al_unlock_mutex(mutex);
    al_destroy_mutex(mutex);
    al_destroy_cond(cond);
    al_destroy_timer(jogo->timer);
    free(jogo);
}

void atualiza(){
    
    al_flip_display();
}
