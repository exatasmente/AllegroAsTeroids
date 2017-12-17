typedef struct coordenada{
    double x;
    double y;
    double dx;
    double dy;
    int angulo;
    int tipo;
}Coordenada;
Coordenada *initCoordenada(double x, double y , double dx , double dy,double angulo,int tipo){
    Coordenada *novo;
    novo = (Coordenada*) malloc(sizeof(Coordenada));
    novo->x = x;
    novo->y = y;
    novo->dx = dx;
    novo->dy = dy;
    novo->angulo = angulo;
    novo->tipo = tipo;
    return novo;
}

