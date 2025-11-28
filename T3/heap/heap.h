#ifndef HEAP_H
#define HEAP_H


typedef struct {
    int *dados;      // vetor que guarda os elementos do heap
    int capacidade;  // capacidade máxima do vetor
    int tamanho;     // quantidade de elementos atualmente no heap
} Heap;

// Cria um heap com capacidade inicial 'cap'
Heap* cria_heap(int cap);

// Libera a memória do heap
void destroi_heap(Heap *h);

// Retorna o número de elementos no heap
int tamanho_heap(Heap *h);

// Insere um valor no heap (max heap)
void insere_heap(Heap *h, int valor);

// Remove e retorna o elemento de maior prioridade (raiz)
int remove_heap(Heap *h);

// Mostra o heap no console (para visualização)
void imprime_heap(Heap *h);

// Funções internas auxiliares (opcional para prototipar)
void heapify_up(Heap *h, int index);
void heapify_down(Heap *h, int index);
void troca(int *a, int *b);

#endif
