typedef struct coordenada{
    double x;
    double y;
    double dx;
    double dy;
    int angulo;
}Coordenada;

Coordenada *initCoordenada(double x, double y , double dx , double dy,double angulo){
    Coordenada *novo;
    novo = (Coordenada*) malloc(sizeof(Coordenada));
    novo->x = x;
    novo->y = y;
    novo->dx = dx;
    novo->dy = dy;
    novo->angulo = angulo;
    return novo;
}

