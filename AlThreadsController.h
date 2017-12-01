
typedef struct alThread{
    ALLEGRO_THREAD *thread;
    int id;
    int fim;
}AlThread;

typedef struct listaThreads{
    AlThread **fila;
    void (* addThread)(struct listaThreads*,AlThread*);
    void (* removerThread)(struct listaThreads*,int);
    ALLEGRO_MUTEX *mutex;
    ALLEGRO_COND  *cond;
    int qt;
    int tam;
}ListaThreads;


AlThread *novoAlThread(void* funcao(ALLEGRO_THREAD*,void*),void *param,int id);
ListaThreads *initListaThreads(int tam);
void addThread(ListaThreads *lista,AlThread *trhead);
void removerThread(ListaThreads *lista, int id);
void destroyAlThread(struct listaThreads *lista);

void setFim(ListaThreads *lista,int id){
    for(int i = 0 ; i < lista->qt ; i++){
        if(lista->fila[i]->id == id){
            lista->fila[i]->fim = 1;
            break;
        }
    }
}
AlThread *novoAlThread(void* funcao(ALLEGRO_THREAD*,void*),void *param,int id){
    AlThread *novo;
    novo = (AlThread*) malloc(sizeof(AlThread));
    novo->thread = al_create_thread(funcao,param);
    novo->id = id;    
    novo->fim = 0;
    al_start_thread(novo->thread);
    return novo;
}

ListaThreads *initListaThreads(int tam){
    ListaThreads *novo;
    novo = (ListaThreads*) malloc(sizeof(ListaThreads));
    novo->fila = (AlThread**)malloc(sizeof(AlThread*)*tam);
    novo->addThread = &addThread;
    novo->removerThread = &removerThread;
    novo->mutex =  al_create_mutex();
    novo->cond = al_create_cond();        
    novo->tam = tam;
    novo->qt = 0;
}

void addThread(ListaThreads *lista,AlThread *thread){
    if(lista->qt < lista->tam){
        lista->fila[lista->qt] = thread;
        lista->qt++;
    }
}
void removerThread(ListaThreads *lista, int id){

    for(int i = 0 ; i < lista->qt ; i++){
        if(lista->fila[i]->id == id ){
            lista->qt--;

            break;
        }
    }
    
}
void destroyAlThread(struct listaThreads *lista){
    for(int i = 0 ; i < lista->qt ; i++){
        lista->removerThread(lista,lista->fila[i]->id);
    }
    free(lista);
}