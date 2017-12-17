typedef struct node
    {
    void *valor;
    struct node *prox;
    struct node *ant;
}Node;

typedef struct listaEncadeada
    {
    struct node *inicio;
    struct node *fim;
    int qt;
}Lista;


void push(Lista *lista,Node *node)
    {
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

void *pop(Lista *lista)
    {
    if(lista->qt > 0)
        {
        Node *aux = lista->inicio->prox;
        aux->prox->ant = lista->inicio;
        lista->inicio->prox = aux->prox;
        lista->qt--;
        return aux->valor;
    }
}

void remover(Lista *lista, int *f(void*, void*),void *valor)
    {
    Node *aux = lista->inicio->prox;
    while(aux->prox != lista->fim)
        {
        if(f(aux->valor,valor))
            {
            aux->prox->ant = aux->ant;
            aux->prox->ant->prox = aux->prox;
            lista->qt--;
            free(aux);
            break;
        }
    }
}
void clear(Lista *lista)
    {
    free(lista);}