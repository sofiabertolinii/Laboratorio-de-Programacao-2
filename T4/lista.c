#include "lista.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// definição dos nós da lista
struct no {
    void *dado; // dado armazenado no nó
    struct no *ant; // nó anterior
    struct no *prox; // nó seguinte
};

// definição da estrutura da lista 
struct lista {
    struct no *ini; // primeiro nó
    struct no *fim; // último nó
    struct no *corr; // nó corrente
    int tam; // número de elementos
    int tamanho_item; // tamanho em bytes de cada elemento (recebido em l_cria)
    int pos; // posição corrente
};

// definindo os protótipos das funções internas
static bool l_antes_inicio(Lista self);
static bool l_apos_fim(Lista self);
static bool l_ultimo_elemento(Lista self);
static bool l_primeiro_elemento(Lista self);
static struct no *retorna_no(Lista self, void *item);
static void l_insere_vazia(Lista self, struct no *novo);
static void l_insere_inicio(Lista self, struct no *novo);
static void l_insere_fim(Lista self, struct no *novo);

// cria e retorna uma lista vazia, que irá armazenar dados do tamanho informado
// o ponteiro retornado aponta para uma estrutura recém alocada, que deve
//   ser destruída (com l_destroi) quando a lista não for mais necessária
Lista l_cria(int tamanho_de_um_item) {
    Lista nova = malloc(sizeof(struct lista));
    if (!nova) return NULL;

    nova->ini = NULL;
    nova->fim = NULL;
    nova->corr = NULL;
    nova->pos = -1;
    nova->tam = 0;
    nova->tamanho_item = tamanho_de_um_item;

    return nova;
}

// destrói uma lista
// essa lista não deve ser utilizada após essa chamada
// se a lista não estiver vazia, a memória com os dados que ela contém é liberada,
//   e os dados são perdidos
void l_destroi(Lista self) {
    if (!self) return;

    struct no *p = self->ini;
    while (p != NULL) {
        struct no *prox = p->prox;
        free(p->dado);
        free(p);
        p = prox;
    }

    free(self);
}

// retorna true se não houver nenhum elemento na lista
bool l_vazia(Lista self) {
    return (!self || self->tam == 0);
}

// retorna o tamanho (número de elementos) da lista
int l_tam(Lista self) {
    if (!self) return 0;
    return self->tam;
}

// retorna true se existe um item na posição corrente
bool l_item_valido(Lista self) {
    if (!self) return false;
    return (self->corr != NULL && self->pos >= 0 && self->pos < self->tam);
}

// copia o item na posição corrente da lista para o endereço item
// retorna false e não copia nada se a posição corrente não corresponder
//   a um item válido (se estiver antes do início ou após o final)
bool l_item(Lista self, void *item) {
    if (!self || !l_item_valido(self)) return false;
    memcpy(item, self->corr->dado, self->tamanho_item);
    return true;
}

// retorna um ponteiro para o item que está na posição corrente da lista
// retorna NULL se a posição corrente não corresponder a um item válido
//   (se estiver antes do início ou após o final)
void *l_item_ptr(Lista self) {
    if (!self || !l_item_valido(self)) return NULL;
    return self->corr->dado;
}

// posiciona antes do início da lista
void l_inicio(Lista self) {
    if (!self) return;
    self->corr = NULL;
    self->pos = -1;
}

// posiciona após o final da lista
void l_final(Lista self) {
    if (!self) return;
    self->corr = NULL;
    self->pos = self->tam;
}

// altera a posição corrente para a posição do pos-ésimo elemento da lista
// pos pode ser negativo, conta a partir do final da lista
// pos pode representar uma posição após o último ou antes do primeiro item,
//   caso em que altera a posição corrente para uma das posições especiais
// retorna true se a posição resultante corresponder a um item válido ou 
//   false caso seja uma das posições especiais
bool l_posiciona(Lista self, int pos) {
    if (!self) return false;

    // se a lista estiver vazia, posiciona antes do início
    if (l_vazia(self)) {
        l_inicio(self);
        return false;
    }

    // ajusta posição negativa
    if (pos < 0) pos += self->tam;

    // se ainda for negativa, posiciona antes do início
    if (pos < 0) {
        l_inicio(self);
        return false;
    }

    // se pos for maior ou igual ao tamanho, posiciona após o final
    if (pos >= self->tam) {
        l_final(self);
        return false;
    }

    // casos normais
    struct no *temp = self->ini;
    for (int i = 0; i < pos; i++)
        temp = temp->prox;

    self->corr = temp;
    self->pos = pos;
    return true;
}

// função de uso interno que verifica se a posição corrente é antes do início
static bool l_antes_inicio(Lista self) {
    return (self && self->corr == NULL && self->pos < 0);
}

// função de uso interno que verifica se a posição corrente é após o final
static bool l_apos_fim(Lista self) {
    return (self && self->corr == NULL && self->pos >= self->tam);
}

// função de uso interno que verifica se a posição corrente é a do último elemento
static bool l_ultimo_elemento(Lista self) {
    return (self && self->corr == self->fim && self->pos == self->tam - 1);
}

// função de uso interno que verifica se a posição corrente é a do primeiro elemento
static bool l_primeiro_elemento(Lista self) {
    return (self && self->corr == self->ini && self->pos == 0);
}

// avança para a próxima posição da lista
// se a posição corrente for anterior ao início, passa para a posição do
//   primeiro item da lista
// passa para a posição após o final se a posição corrente for a posição
//   do último elemento da lista ou se a lista estiver vazia
// continua na posição após o final se já estiver nela
// retorna true se a posição resultante contiver um item válido (retorna
//   false se a posição resultante for após o final da lista)
bool l_avanca(Lista self) {
    if (!self) return false;

    // se estiver antes do início, vai para o primeiro elemento
    if (l_antes_inicio(self)) {
        self->corr = self->ini;
        self->pos = 0;
        return true;
    }

    // se for o último elemento ou a lista estiver vazia, vai para após o final
    if (l_vazia(self) || l_ultimo_elemento(self)) {
        l_final(self);
        return false;
    }

    // se estiver após o final, não faz nada
    if (l_apos_fim(self)) return false;

    // casos normais
    self->corr = self->corr->prox;
    self->pos++;
    return l_item_valido(self);
}

// recua para a posição anterior da lista
// se a posição corrente for após o final, passa para a posição do último
//   item da lista 
// passa para a posição antes do início se a posição corrente for a do
//   primeiro elemento da lista ou se a lista estiver vazia
// continua na posição antes do início se já estiver nela
// retorna true se a posição resultante contiver um item válido (retorna
//   false se a posição resultante for antes do início da lista)
bool l_recua(Lista self) {
    if (!self) return false;

    // se estiver após o final, vai para o último elemento
    if (l_apos_fim(self)) {
        self->corr = self->fim;
        self->pos = self->tam - 1;
        return true;
    }

    // se for o primeiro elemento ou a lista estiver vazia, vai para antes do início
    if (l_vazia(self) || l_primeiro_elemento(self)) {
        l_inicio(self);
        return false;
    }

    // se estiver antes do início, não faz nada
    if (l_antes_inicio(self)) return false;

    // casos normais
    self->corr = self->corr->ant;
    self->pos--;
    return l_item_valido(self);
}

// função de uso interno que cria um nó com o dado apontado por item
static struct no *retorna_no(Lista self, void *item) {
    struct no *novo = malloc(sizeof(struct no));
    if (!novo) return NULL;

    novo->dado = malloc(self->tamanho_item);
    if (!novo->dado) {
        free(novo);
        return NULL;
    }

    memcpy(novo->dado, item, self->tamanho_item);
    novo->ant = NULL;
    novo->prox = NULL;
    return novo;
}

// função de uso interno que insere um nó na lista vazia
static void l_insere_vazia(Lista self, struct no *novo) {
    self->ini = self->fim = self->corr = novo;
    self->pos = 0;
}

// função de uso interno que insere no início
static void l_insere_inicio(Lista self, struct no *novo) {
    novo->prox = self->ini;
    if (self->ini) self->ini->ant = novo;
    self->ini = novo;
    self->corr = novo;
    self->pos = 0;
}

// função de uso interno que insere no fim
static void l_insere_fim(Lista self, struct no *novo) {
    novo->ant = self->fim;
    if (self->fim) self->fim->prox = novo;
    self->fim = novo;
    self->corr = novo;
    self->pos = self->tam; 
}

// insere o dado apontado por item antes da posição corrente da lista
// caso a posição corrente seja antes do início, o item inserido será
//   o novo primeiro elemento da lista
// caso a posição corrente seja após o fim, o item inserido será o novo
//   último elemento da lista
// a posição corrente passa a ser a do item inserido
void l_insere_antes(Lista self, void *item) {
    if (!self) return;
    struct no *novo = retorna_no(self, item);
    if (!novo) return;

    // se a lista estiver vazia passa a ser o único elemento
    if (l_vazia(self)) {
        l_insere_vazia(self, novo);
    
    // se estiver antes do início, insere no início
    } else if (l_antes_inicio(self)) {
        l_insere_inicio(self, novo);

    // se estiver após o final, insere no fim
    } else if (l_apos_fim(self)) {
        l_insere_fim(self, novo);

    // casos normais
    } else {
        novo->prox = self->corr;
        novo->ant = self->corr->ant;
        // se já tem um nó antes do corrente
        if (self->corr->ant) {
            self->corr->ant->prox = novo;
        }
        // se não tinha nó antes do corrente, atualiza o início
        else {
            self->ini = novo;
            self->corr->ant = novo;
            self->corr = novo;
            self->pos--;
        }
    }

    self->tam++;
}

// insere o dado apontado por item após a posição corrente da lista
// caso a posição corrente seja antes do início, o item inserido será
//   o novo primeiro elemento da lista
// caso a posição corrente seja após o fim, o item inserido será o novo
//   último elemento da lista
// a posição corrente passa a ser a do item inserido
void l_insere_depois(Lista self, void *item) {
    if (!self) return;
    struct no *novo = retorna_no(self, item);
    if (!novo) return;

    // se a lista estiver vazia passa a ser o único elemento
    if (l_vazia(self)) {
        l_insere_vazia(self, novo);
    
    // se estiver antes do início, insere no início
    } else if (l_antes_inicio(self)) {
        l_insere_inicio(self, novo);

    // se estiver após o final, insere no fim
    } else if (l_apos_fim(self)) {
        l_insere_fim(self, novo);
    
    // casos normais
    } else {
        novo->ant = self->corr;
        novo->prox = self->corr->prox;
        if (self->corr->prox) {
            self->corr->prox->ant = novo;
        } else {
            self->fim = novo;
            self->corr->prox = novo;
            self->corr = novo;
            self->pos++;
        }
    }

    self->tam++;
}

// se a posição corrente da lista contiver um item válido, copia o conteúdo
//   desse item para a região apontada por item, remove o item da lista e
//   retorna true
// senão, retorna false
bool l_remove(Lista self, void *item) {
    if (!self || !l_item_valido(self)) return false;

    // copia o dado antes de remover
    if (item) memcpy(item, self->corr->dado, self->tamanho_item);

    // remove o nó corrente
    struct no *para_remover = self->corr;

    // se o nó a remover tiver um nó anterior, atualiza o próximo do anterior
    if (para_remover->ant) para_remover->ant->prox = para_remover->prox;
    // se não tinha nó anterior, atualiza o início
    else self->ini = para_remover->prox;

    // se o nó a remover tiver um nó seguinte, atualiza o anterior do seguinte
    if (para_remover->prox) para_remover->prox->ant = para_remover->ant;
    // se não tinha nó seguinte, atualiza o fim
    else self->fim = para_remover->ant;

    // se tiver próximo, o próximo passa a ser o corrente
    if (para_remover->prox) {
        self->corr = para_remover->prox;
    
    // se não tiver próximo, o anterior passa a ser o corrente
    } else {
        self->corr = para_remover->ant;
        self->pos--;
    }

    // libera o nó removido e o seu dado
    free(para_remover->dado);
    free(para_remover);
    self->tam--;

    return true;
}