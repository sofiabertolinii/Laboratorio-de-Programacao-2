#include "heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


// Cria um heap com capacidade inicial 'cap'
Heap* cria_heap(int cap) {
    Heap *novo = malloc(sizeof(Heap));
    if(novo == NULL) return NULL;
    novo->dados = malloc(cap * sizeof(int));
    if(novo->dados == NULL) return NULL;

    novo->capacidade = cap;
    novo->tamanho = 0;
    
    return novo;
}

// Libera a memória do heap
void destroi_heap(Heap *h) {
    if(h == NULL) return;
    free(h->dados);
    free(h);
}

// Retorna o número de elementos no heap
int tamanho_heap(Heap *h) {
    return h->tamanho;
}

static void troca(int *valor1, int *valor2) {
    int aux = *valor1;
    *valor1 = *valor2;
    *valor2 = aux;
}

// Insere um valor no heap (max heap)
// para achar o índice do pai = (idx - 1)/2
void insere_heap(Heap *h, int valor) {
    if(h == NULL) return;

    int idx_novo = h->tamanho; // índice do novo elemento (foi inserido por último)
    h->dados[h->tamanho] = valor; //coloca na última posição
    h->tamanho++; 
    
    // enquanto não chegar no índice zero
    while(idx_novo > 0) {
        int pai = (idx_novo - 1)/2; // calcula o índice do pai

        if(h->dados[idx_novo] > h->dados[pai]) { // se o filho for maior que o pai
            troca(&h->dados[idx_novo], &h->dados[pai]); // troca
            idx_novo = pai; // sobe
        } else {
            break; // se o filho for menor sai do laço
        }
    }
}

// Retorna True se a árvore estiver ordenada
bool ordenada (Heap *h) {
    if(h == NULL) return false;
    if(h->tamanho <= 1) return true;

    for(int i = h->tamanho; i > 0; i--) {
        int pai = (i - 1)/2;
        if(h->dados[i] > h->dados[pai]) return false;
    }

    return true;
}

// Remove e retorna o elemento de maior prioridade (raiz)
int remove_heap(Heap *h) {
    if(h == NULL || h->tamanho == 0) return -1;
    int raiz = h->dados[0];
    h->dados[0] = h->dados[h->tamanho - 1];
    h->tamanho--;

    int idx = 0;
    while(true) {
        int esq = 2*idx + 1;
        int dir = 2*idx + 2;
        int maior = idx;

        if(esq < h->tamanho && h->dados[esq] > h->dados[maior]) maior = esq;
        if(dir < h->tamanho && h->dados[dir] > h->dados[maior]) maior = dir;

        if(maior == idx) break; // raiz maior que filhos
        troca(&h->dados[idx], &h->dados[maior]);
        idx = maior;
    }
} 

// Mostra o heap no console (para visualização)
void imprime_heap(Heap *h) {
    for(int i = 0; i < h->tamanho; i++) {
        printf("%d ", h->dados[i]);
    }
    printf("\n");
}

// Funções internas auxiliares (opcional para prototipar)
void heapify_up(Heap *h, int index);
void heapify_down(Heap *h, int index);
void troca(int *a, int *b);
