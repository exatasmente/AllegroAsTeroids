
typedef struct node{
    struct node *prox;
    struct node *ant;
    Coordenada *valor;
}Node;
typedef struct listaDesenho{
    Node *inicio;
    Node *fim;
    int qt;
}Lista;

Lista *initLista();
void add(Lista *lista,Node *node);
Node *remover(Lista *lista);
void destroyDesenho(Lista *lista);
Node *novoNode(Coordenada *valor);

Node *novoNode(Coordenada *valor)
    {
    Node *novo;
    novo = (Node*) malloc(sizeof(Node));
    novo->ant  = NULL;
    novo->prox = NULL;
    novo->valor = valor;

    return novo;
}


Lista *initLista(){
    Lista *novo;

    novo = (Lista*) malloc(sizeof(Lista));

    novo->inicio = novoNode(NULL);
    novo->fim = novoNode(NULL);
    novo->inicio->prox = novo->fim;
    novo->fim->ant = novo->inicio;
    novo->qt = 0;
}

void add(Lista *lista,Node *node){
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
    
    
}

Node *remover(Lista*lista){
    if(lista->qt > 0)
        {
        Node *aux = lista->inicio->prox;
        aux->prox->ant = lista->inicio;
        lista->inicio->prox = aux->prox;
        lista->qt--;
        return aux;
    }
    return NULL;
}
void destroyDesenho(Lista *lista){
    free(lista);
    
}