typedef struct jogador{
    Coordenada *posicao;
    int vidas;
    int pontos;
    ALLEGRO_BITMAP *sprite[2];
}Jogador;

Jogador *initJogador(Coordenada *posicao, int vidas,int pontos,ALLEGRO_BITMAP *sprite[2]){
    Jogador *novo;
    novo = (Jogador*) malloc(sizeof(Jogador));
    novo->posicao = posicao;
    novo->pontos = pontos;
    novo->vidas = vidas;
    novo->sprite[0] = sprite[0];
    novo->sprite[1] = sprite[1];
    return novo;
}

