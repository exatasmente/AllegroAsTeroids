

typedef struct tiro{
    Coordenada *posicao;
    ALLEGRO_BITMAP *sprite;
}Tiro;


Tiro *initTiro(Coordenada *posicao, ALLEGRO_BITMAP *sprite){
    Tiro *novo;
    novo = (Tiro*) malloc(sizeof(Tiro));
    novo->posicao = posicao;
    novo->sprite = sprite;

    return novo;
}