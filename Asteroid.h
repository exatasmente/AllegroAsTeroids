

typedef struct asteroide{
    Coordenada *posicao;
    ALLEGRO_BITMAP *sprite;
    
}Asteroide;

Asteroide *initAsteroid(Coordenada *posicao,ALLEGRO_BITMAP *sprite){
    Asteroide *novo;
    novo = (Asteroide*) malloc(sizeof(Asteroide));
    novo->posicao = posicao;
    novo->sprite = sprite;
    
    return novo;
}