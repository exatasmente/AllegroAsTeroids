typedef struct desenho{
    ALLEGRO_BITMAP *imagem;
    Coordenada *posicao;
    int flags;
    int id;
    int posExplosao;
}Desenho;

typedef struct node{
    struct node *prox;
    struct node *ant;
    Desenho *valor;
}Node;
typedef struct listaDesenho{
    Node *inicio;
    Node *fim;
    ALLEGRO_MUTEX *mutex;
    int qt;
    int tam;
    int ids;    
}ListaDesenho;

Desenho *novoDesenho(ALLEGRO_BITMAP *imagem,Coordenada *posicao,int flags);
ListaDesenho *initListaDesenho(int tam);
void addDesenho(ListaDesenho *lista,Node *node);
Desenho *removerDesenho(ListaDesenho *lista);
void removerDesativados(ListaDesenho *lista);
void destroyDesenho(ListaDesenho *lista);
Node *novoNode(Desenho *valor);

Desenho *novoDesenho(ALLEGRO_BITMAP *imagem,Coordenada *posicao,int flags){
    Desenho *novo;
    novo = (Desenho*) malloc(sizeof(Desenho));
    novo->imagem = imagem;
    novo->posicao = posicao;
    novo->flags = flags;
    
    novo->id = 0;
    novo->posExplosao = 0;
    return novo;
}
Node *novoNode(Desenho *valor)
    {
    Node *novo;
    novo = (Node*) malloc(sizeof(Node));
    novo->ant  = NULL;
    novo->prox = NULL;
    novo->valor = valor;

    return novo;
}


ListaDesenho *initListaDesenho(int tam){
    ListaDesenho *novo;

    novo = (ListaDesenho*) malloc(sizeof(ListaDesenho));

    novo->inicio = novoNode(NULL);
    novo->fim = novoNode(NULL);
    novo->inicio->prox = novo->fim;
    novo->fim->ant = novo->inicio;

    novo->mutex =  al_create_mutex();
    novo->tam = tam;
    novo->qt = 0;
    novo->ids = 0;
}

void addDesenho(ListaDesenho *lista,Node *node){
    if(lista->qt < lista->tam){
      if(lista->inicio->prox == lista->fim)
        {
        lista->inicio->prox = node;
        node->ant = lista->inicio;
        node->prox = lista->fim;
        lista->fim->ant = node;
        }else
            {
            lista->inicio->prox->ant = node;
            node->prox = lista->inicio->prox;
            node->ant = lista->inicio;
            lista->inicio->prox = node;
        
        }
        lista->qt++;
    }else{
        al_destroy_bitmap(node->valor->imagem);
        free(node);
    }
    
    
}

Desenho *removerDesenho(ListaDesenho *lista){
    if(lista->qt > 0)
        {
        Node *aux = lista->inicio->prox;
        aux->prox->ant = lista->inicio;
        lista->inicio->prox = aux->prox;
        lista->qt--;
        return aux->valor;
    }
    return NULL;
}
void removerDesativados(ListaDesenho *lista){
    if(lista->qt > 0){
        Node *aux = lista->inicio->prox;
        Node *aux2;
        while(aux != lista->fim)
            {
            if(aux->valor->id == -1 )
                {
                al_destroy_bitmap(aux->valor->imagem);
                
                aux->prox->ant = aux->ant;
                aux->prox->ant->prox = aux->prox;
                lista->qt--;
                free(aux->valor);
                aux = aux->prox->ant->prox ;    
            }else
                {
                aux = aux->prox;
            }
        }        
    }
}
void destroyDesenho(ListaDesenho *lista){
    al_destroy_mutex(lista->mutex);
    free(lista);
    
}