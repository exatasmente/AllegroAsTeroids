

typedef struct desenho{
    ALLEGRO_BITMAP *imagem;
    Coordenada *posicao;
    int tipo;
    int flags;
    int id;
    ALLEGRO_BITMAP *explosao[12];    
    int posExplosao;
}Desenho;

typedef struct listaDesenho{
    Desenho **fila;
    ALLEGRO_MUTEX *mutex;
    int qt;
    int tam;
    int ids ;
    void (* addDesenho)(struct listaDesenho*,Desenho*);
    Desenho* (* removerDesenho)(struct listaDesenho*,int);
    void (* removerDesativados)(struct listaDesenho*);
    
}ListaDesenho;


Desenho *novoDesenho(ALLEGRO_BITMAP *imagem,Coordenada *posicao,int tipo,int flags);
ListaDesenho *initListaDesenho(int tam);
void addDesenho(ListaDesenho *lista,Desenho *desenho);
Desenho *removerDesenho(ListaDesenho *lista,int id);
void removerDesativados(ListaDesenho *lista);
void destroyDesenho(ListaDesenho *lista);



Desenho *novoDesenho(ALLEGRO_BITMAP *imagem,Coordenada *posicao,int tipo,int flags){
    Desenho *novo;
    novo = (Desenho*) malloc(sizeof(Desenho));
    novo->imagem = imagem;
    novo->posicao = posicao;
    novo->flags = flags;
    novo->tipo = tipo;
    novo->id = 0;
    char *d[] = {"explosao0.png","explosao1.png","explosao3.png","explosao4.png","explosao5.png","explosao6.png","explosao7.png","explosao7.png","explosao8.png","explosao9.png","explosao10.png","explosao11.png"};
    for(int i = 0 ; i < 12 ; i++){
        novo->explosao[i] = al_load_bitmap(d[i]);
    }
    novo->posExplosao = 0;
    return novo;
}

ListaDesenho *initListaDesenho(int tam){
    ListaDesenho *novo;
    novo = (ListaDesenho*) malloc(sizeof(ListaDesenho));
    novo->fila = (Desenho**) malloc(sizeof(Desenho*)*tam);
    novo->addDesenho = &addDesenho;
    novo->removerDesenho = &removerDesenho;
    novo->removerDesativados = &removerDesativados;
    novo->mutex =  al_create_mutex();
    novo->tam = tam;
    novo->qt = 0;
    novo->ids = 0;
}

void addDesenho(ListaDesenho *lista,Desenho *desenho){
    if(lista->qt < lista->tam){
        desenho->id = lista->ids++;
        if(lista->qt <= lista->tam){
            lista->fila[lista->qt] = desenho;
            lista->qt++;
        }
    }
    
}
Desenho *removerDesenho(ListaDesenho *lista,int id){
    if(lista->qt > 0){
        Desenho *desenho;
        desenho = lista->fila[id];
        lista->fila[id] = (Desenho*)malloc(sizeof(Desenho));
        for(int j = id ; j < lista->qt-1 ; j++){    
            lista->fila[j] = lista->fila[j+1];
        }

        free(lista->fila[lista->qt-1]);
        lista->qt--;
        return desenho;
        
    }
    return NULL;
}
void removerDesativados(ListaDesenho *lista){
    if(lista->qt > 0){
        for(int j = 0 ; j < lista->qt ; j++){    
            if(lista->fila[j]->id == -1 ){
                
                for(int i = 0 ; i < lista->qt; i++){
                    lista->fila[i] = lista->fila[i+1];                    
                }
                lista->qt--;
            }
        }
        
    }

}

void destroyDesenho(ListaDesenho *lista){
    al_destroy_mutex(lista->mutex);
    free(lista);
    
}