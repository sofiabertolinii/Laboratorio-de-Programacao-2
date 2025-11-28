#include "fila.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct no {
    void *dado;
    struct no *prox;
};

struct fila {
    struct no *ini; // início da fila 
    struct no *fim; // final da fila 
    struct no *perc; // ponteiro usado no percurso
    int tam; // número de elementos
    int tamanho_item; // tamanho de cada dado armazenado
};

// cria uma fila vazia que suporta dados do tamanho fornecido (em bytes)
Fila fila_cria(int tam_do_dado) {
    Fila nova = malloc(sizeof(struct fila));
    if (!nova) return NULL;

    nova->ini = NULL;
    nova->fim = NULL;
    nova->perc = NULL;
    nova->tam = 0;
    nova->tamanho_item = tam_do_dado;
    return nova;
}

// libera a memória ocupada por uma fila
void fila_destroi(Fila self) {
    if (!self) return;
    struct no *atual = self->ini;
    // enquanto houver nós na fila
    while (atual) {
        struct no *proximo = atual->prox;
        free(atual->dado);
        free(atual);
        atual = proximo;
    }
    free(self);
}

// diz se a fila está vazia
bool fila_vazia(Fila self) {
    return !self || self->tam == 0;
}

// remove o dado no início da fila e, se pdado não for NULL, copia o
//   dado removido para *pdado
void fila_remove(Fila self, void *pdado) {
    if (!self || fila_vazia(self)) return;

    // nó a ser removido
    struct no *removido = self->ini;
    if (pdado) memcpy(pdado, removido->dado, self->tamanho_item);

    self->ini = removido->prox;
    // se a fila ficou vazia, atualiza o ponteiro do fim
    // significa que o nó removido era o único da fila
    if (!self->ini) self->fim = NULL;

    free(removido->dado);
    free(removido);
    self->tam--;
}

// insere o dado apontado por pdado no final da fila
void fila_insere(Fila self, void *pdado) {
    if (!self || !pdado) return;

    struct no *novo = malloc(sizeof(struct no));
    if (!novo) return;

    novo->dado = malloc(self->tamanho_item);
    if (!novo->dado) {
        free(novo);
        return;
    }

    // copia o dado para o novo nó
    memcpy(novo->dado, pdado, self->tamanho_item);
    novo->prox = NULL;

    // se a fila estava vazia, o novo nó é o início
    if (fila_vazia(self)) self->ini = novo;
    // se a fila não estava vazia, o próximo do fim passa a ser o novo nó
    else self->fim->prox = novo;

    self->fim = novo;
    self->tam++;
}

// inicia um percurso aos elementos da fila, a partir de uma posição inicial.
// cada dado da fila, a partir dessa posição será acessado por chamadas a
//   fila_proximo().
// se a posição for positiva, o percurso vai desde essa posição até o fim da fila.
// se a posição for negativa, o percurso vai desde essa posição até o início.
//   0 é a posição do primeiro dado (aquele que está na fila há mais tempo)
//   1 é a posição do segundo dado, etc
//   além disso,
//   -1 é a posição do último dado (o que está na fila há menos tempo)
//   -2 é a posição do penúltimo dado, etc
// se a posição for positiva além do final da fila ou negativa antes
//   do início, o percurso não percorrerá nenhum dado
void fila_inicia_percurso(Fila self, int pos_inicial) {
    if (!self) return;

    if (pos_inicial >= 0) {

        // se a posição inicial for além do final da fila o percurso é vazio
        if (pos_inicial >= self->tam) {
            self->perc = NULL;
            return;
        }

        // caso normal, percorre até a posição desejada
        self->perc = self->ini;
        for (int i = 0; i < pos_inicial; i++) {
            self->perc = self->perc->prox;
        }

    // se a posição inicial for negativa, o percurso vai até o início
    } else {
        // converte a posição negativa para positiva
        int pos = self->tam + pos_inicial;

        // se for antes do início o percurso é vazio
        if (pos < 0) {
            self->perc = NULL;
            return;
        }

        // caso normal, percorre até a posição desejada
        self->perc = self->ini;
        for (int i = 0; i < pos; i++)
            self->perc = self->perc->prox;
    }
}

// coloca o próximo dado do percurso em *pdado (se pdado não for NULL),
//   avança o percurso para o dado seguinte e retorna true
// ou, caso o percurso tenha terminado, retorna false
bool fila_proximo(Fila self, void *pdado) {
    if (!self || !self->perc) return false;

    if (pdado) memcpy(pdado, self->perc->dado, self->tamanho_item);

    self->perc = self->perc->prox;
    return true;  // sempre true enquanto ainda havia um elemento válido
}
