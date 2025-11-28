#include "fila-prio.h"
#include <stdlib.h>
#include <string.h>

struct fila_prio {
    void *dados; // vetor que armazena os elementos
    int capacidade; // tamanho máximo atual do vetor
    int tamanho; // quantidade de elementos na fila
    int bytes_por_item; // tamanho de cada item em bytes
    compara_t *comparador; // função de comparação
};

// Aloca e retorna uma nova fila, vazia.
// Os dados a serem colocados na fila terão o tamanho "bytes_por_item",
//   e podem ser comparados pela função apontada por "comparador".
FilaPrio fp_cria(int bytes_por_item, compara_t *comparador) {
    FilaPrio fila = malloc(sizeof(struct fila_prio));
    if(fila == NULL) return NULL;

    fila->bytes_por_item = bytes_por_item;
    fila->comparador = comparador;
    fila->tamanho = 0;
    fila->capacidade = 10; // tamanho inicial
    fila->dados = malloc(fila->bytes_por_item * fila->capacidade);
    if(fila->dados == NULL) {
        free(fila);
        return NULL;
    }

    return fila;
}


// Libera a memória ocupada pela fila.
void fp_destroi(FilaPrio self) {
    if(self == NULL || self->dados == NULL) return;
    free(self->dados);
    free(self);
}

// Retorna o número de itens atualmente na fila.
int fp_tam(FilaPrio self) {
    if(self == NULL || self->dados == NULL) return 0;
    return self->tamanho;
}

// Retorna o número de bytes
int fp_bytes(FilaPrio self) {
    if(self == NULL || self->dados == NULL) return 0;
    return self->bytes_por_item;
}

// Retorna o número do índice do pai
int fp_idx_pai(int i) {
    return (i - 1)/2;
}

// Retorna o índice do filho da esquerda
int fp_filho_esq(int i) {
    return 2*i + 1;
}

// Retorna o índice do filho da direita
int fp_filho_dir(int i) {
    return 2*i + 2;
}

// Função interna pra realocar a memória quando necessário
static void realoca(FilaPrio self) {
    if(self == NULL || self->dados == NULL) return;

    // realoca pra 1.5 vezes maior do tamanho se o tamanho for maior ou igual a capacidade alocada
    if(fp_tam(self) >= self->capacidade) {
        self->capacidade += self->capacidade/2;
        self->dados = realloc(self->dados, fp_bytes(self) * self->capacidade);

    }
}

// memcpy(destino, source, qtd de bytes)
// função auxiliar que troca os dados na lista
static void troca (FilaPrio self, int a, int b) {
    if(self == NULL || self->dados == NULL) return;

    char *enderecoA = (char *)self->dados + a * fp_bytes(self); // calcula os endereços
    char *enderecoB = (char *)self->dados + b * fp_bytes(self);

    char *aux = malloc(fp_bytes(self)); // aloca o auxiliar pra fazer a troca
    if(aux == NULL) return;

    memcpy(aux, enderecoA, fp_bytes(self)); // faz a troca 
    memcpy(enderecoA, enderecoB, fp_bytes(self));
    memcpy(enderecoB, aux, fp_bytes(self));

    free(aux);
}

// compara o elemento do índice idx com o seu pai (o pai tem que ter prioridade)
// ordena de baixo pra cima (o maior fica no topo)
// fórmul: (idx - 1)/2 = índice do pai
static void heapifyup(FilaPrio self, int idx) {
    if(self == NULL || self->dados == NULL) return;
    while(idx > 0) {
        int idx_pai = fp_idx_pai(idx); // cálcula o índice do pai

        char *item = (char *)self->dados + idx * fp_bytes(self); // endereço do filho
        char *pai = (char *)self->dados + idx_pai * fp_bytes(self); // endereço do pai

        if(self->comparador(item, pai) > 0) {
            troca(self, idx, idx_pai);
            idx = idx_pai;
        } else {
            break;
        }
    }
}

// Insere o item apontado por "dado" na fila "self".
void fp_insere(FilaPrio self, void *dado) {
    if(dado == NULL || self == NULL || self->dados == NULL) return;
    realoca(self);
    
    // calcula o endereço a partir do ponto de partida
    // usa memcpy pra passaer pro final
    // transformar em char pra usar o memcpy
    char *endereco =(char *) self->dados + fp_tam(self) * fp_bytes(self);
    memcpy(endereco, dado, fp_bytes(self));
    self->tamanho++;

    heapifyup(self, self->tamanho - 1);
}

// ordena de cima pra baixo
// vai do indice 0 até o final trocando se o filho tiver prioridade sobre o pai
static void heapifydown(FilaPrio self, int idx) {
    if (self == NULL || self->dados == NULL) return;

    while (1) {
        int filho_esquerdo = fp_filho_esq(idx);
        int filho_direito  = fp_filho_dir(idx);
        int maior = idx;

        void *item_atual = (char *)self->dados + idx * self->bytes_por_item;

        // compara com o filho esquerdo
        if (filho_esquerdo < self->tamanho) {
            void *item_esq = (char *)self->dados + filho_esquerdo * self->bytes_por_item;
            if (self->comparador(item_esq, item_atual) > 0)
                maior = filho_esquerdo;
        }

        // compara com o filho direito
        if (filho_direito < self->tamanho) {
            void *item_dir = (char *)self->dados + filho_direito * self->bytes_por_item;
            void *item_maior = (char *)self->dados + maior * self->bytes_por_item;
            if (self->comparador(item_dir, item_maior) > 0)
                maior = filho_direito;
        }

        if (maior != idx) {
            troca(self, idx, maior);
            idx = maior;  // continua descendo
        } else break;
    }
}


// memcpy(destino, source, qtd de bytes)
// Remove o primeiro item da fila, copiando seu conteúdo para a 
//   região apontada por "dado".
void fp_remove(FilaPrio self, void *dado) {
    if(self == NULL || self->dados == NULL || fp_tam(self) == 0) return;
    memcpy(dado, (char *)self->dados, fp_bytes(self));

    // o último dado passa a ser o primeiro
    char *ultimo = (char *)self->dados + (self->tamanho - 1) * self->bytes_por_item;
    memcpy(self->dados, ultimo, fp_bytes(self));
    self->tamanho--;

    // chama a heapfydown
    heapifydown(self, 0);
}