#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "grafo.h"

struct aresta {
    int destino; // índice do nó de destino
    void *dado; // valor associado à aresta (ex: peso)
    struct aresta *prox; // próxima aresta que sai do mesmo nó
};

struct no {
    void *dado; // dado que o nó guarda
    struct aresta *adj; // ponteiro para a primeira aresta da lista de adjacência
};

struct grafo {
    struct no *nos; // vetor dinâmico de nós (guarda os dados dos nós e as listas de adjacência)
    int nnos; // número atual de nós
    int tam_no; // tamanho do dado de cada nó em bytes
    int tam_aresta; // tamanho do dado de cada aresta em bytes
    int capacidade; // para evitar realocar a cada novo nó

    // auxiliares para consultas com grafo_proxima_aresta()
    int no_consulta; // nó base da consulta
    struct aresta *aresta_atual; // ponteiro para a aresta sendo percorrida
    bool consulta_por_origem; // verdadeiro quando parte de um nó e falso quando chegam
    int indice_busca; // usado no percurso de "arestas que chegam"
};

// declarando as funções estáticas
static void grafo_realoca(struct grafo *self);
static void remove_arestas_para_no(Grafo self, int no);
static void remove_arestas_de_no(Grafo self, int no);
static void cria_aresta(Grafo self, int origem, int destino, void *pdado);
static bool existe_aresta(Grafo self, int origem, int destino);
static struct aresta *retorna_aresta(Grafo self, int origem, int destino);
static void remove_aresta(Grafo self, int origem, int destino);
static bool eh_valido(Grafo self, int origem);
static void *proxima_aresta_partindo(Grafo self, int *vizinho);
static void *proxima_aresta_chegando(Grafo self, int *vizinho);
static bool dfs_tem_ciclo(Grafo g, int v, int *estado);
static void calcula_grau_entrada(Grafo self, int *grau_entrada);
static void inicializa_fila_grau_zero(Grafo self, int *grau_entrada, Fila fila);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NÓS

// realoca vetor de nós se necessário
static void grafo_realoca(struct grafo *self) {

    // se o número de nós atingiu a capacidade, dobra o tamanho do vetor
    if (self->nnos >= self->capacidade) {
        int nova_capacidade = self->capacidade * 2;
        // se capacidade era 0, define um valor inicial
        if (nova_capacidade == 0) nova_capacidade = 4;

        // realoca vetor de nós
        struct no *novo = realloc(self->nos, nova_capacidade * sizeof(struct no));
        if (!novo) {
            printf("Erro: falha ao realocar vetor de nós\n");
            exit(1);
        }

        // inicializa novos espaços com NULL (ainda não usados)
        for (int i = self->capacidade; i < nova_capacidade; i++) {
            novo[i].dado = NULL;
            novo[i].adj = NULL;
        }

        self->nos = novo;
        self->capacidade = nova_capacidade;
    }
}

// cria um grafo vazio que suporta dados dos tamanhos fornecidos (em bytes)
//   nos nós e nas arestas
Grafo grafo_cria(int tam_no, int tam_aresta) {
    Grafo grafo = malloc(sizeof(*grafo));
    if (!grafo) return NULL;

    // preenchendo os campos
    grafo->nnos = 0;
    grafo->tam_no = tam_no;
    grafo->tam_aresta = tam_aresta;
    grafo->capacidade = 8;
    grafo->nos = malloc(grafo->capacidade * sizeof(struct no));
    if (!grafo->nos) {
        free(grafo);
        printf("Erro: falha ao alocar vetor de nós\n");
        return NULL;
    }

    // deixando os dados com NULL
    for (int i = 0; i < grafo->capacidade; i++) {
        grafo->nos[i].dado = NULL;
        grafo->nos[i].adj = NULL;
    }

    grafo->no_consulta = -1;
    grafo->aresta_atual = NULL;
    grafo->consulta_por_origem = true;
    grafo->indice_busca = 0;

    return grafo;
}

// libera a memória ocupada por um grafo
void grafo_destroi(Grafo self) {
    if (!self) return;

    // itera sobre os nós
    for (int i = 0; i < self->nnos; i++) {
        struct aresta *a = self->nos[i].adj;
        // enquanto houver arestas, libera elas
        while (a) {
            struct aresta *prox = a->prox;
            free(a->dado);
            free(a);
            a = prox;
        }
        // libera o dado do nó
        free(self->nos[i].dado);
    }

    // libera o vetor de nós e o grafo em si
    free(self->nos);
    free(self);
}

// insere um nó no grafo, com o dado apontado por pdado (copia o conteúdo
//   de *pdado).
// retorna o número do novo nó
int grafo_insere_no(Grafo self, void *pdado) {
    if (!self || !pdado) return -1;

    grafo_realoca(self);
    // índice do novo nó
    int idx = self->nnos++;
    // aloca espaço para o dado do nó
    self->nos[idx].dado = malloc(self->tam_no);
    if (!self->nos[idx].dado) return -1;

    // copia o dado pro novo nó
    memcpy(self->nos[idx].dado, pdado, self->tam_no);
    self->nos[idx].adj = NULL; // inicializa lista de arestas como vazia
    return idx;
}

// função de uso interno que remove as arestas que apontam para um nó
static void remove_arestas_para_no(Grafo self, int no) {
    // itera sobre todos os nós para remover arestas que apontam para o nó
    for (int i = 0; i < self->nnos; i++) {
        // ponteiro para ponteiro para facilitar remoção
        struct aresta **aresta_chega = &self->nos[i].adj;
        while (*aresta_chega) {
            // se a aresta aponta para o nó a ser removido
            if ((*aresta_chega)->destino == no) {
                struct aresta *tmp = *aresta_chega;
                *aresta_chega = (*aresta_chega)->prox;
                free(tmp->dado);
                free(tmp);
            } else {
                aresta_chega = &(*aresta_chega)->prox;
            }
        }
    }
}

// função de uso interno que remove as arestas que saem de um nó
static void remove_arestas_de_no(Grafo self, int no) {
    struct aresta *aresta_sai = self->nos[no].adj;
    while (aresta_sai) {
        struct aresta *prox = aresta_sai->prox;
        free(aresta_sai->dado);
        free(aresta_sai);
        aresta_sai = prox;
    }
    self->nos[no].adj = NULL;
}

// remove um nó do grafo e as arestas incidentes nesse nó.
// a identificação dos nós remanescentes é alterada, como se esse nó
//   nunca tivesse existido.
void grafo_remove_no(Grafo self, int no) {
    if (!self || no < 0 || no >= self->nnos) return;

    // remove as arestas que apontam para esse nó
    remove_arestas_para_no(self, no);

    // remove as arestas que saem do nó
    remove_arestas_de_no(self, no);

    // libera o dado do nó
    free(self->nos[no].dado);

    // desloca os nós uma posição para frente
    for (int i = no; i < self->nnos - 1; i++) {
        self->nos[i] = self->nos[i + 1];
    }
    self->nnos--;
}

// retorna um ponteiro para o valor associado ao nó "no"
void *grafo_no(Grafo self, int no) {
    if (!self || no < 0 || no >= self->nnos) return NULL;
    return self->nos[no].dado;
}

// retorna ponteiro para o valor associado a um nó
int grafo_nnos(Grafo self) {
    return self->nnos;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Arestas

// GRAFO ALTERA ARESTA

// função de uso interno que cria uma aresta
static void cria_aresta(Grafo self, int origem, int destino, void *pdado) {
    // se o destino ou origem forem inválidos, ou a aresta já existir, retorna
    if (!self || origem < 0 || origem >= self->nnos || destino < 0 || destino >= self->nnos) return;
    if (existe_aresta(self, origem, destino) || !pdado) return;

    // aloca a nova aresta
    struct aresta *nova = malloc(sizeof(struct aresta));
    if (!nova) return;

    nova->destino = destino;
    nova->dado = malloc(self->tam_aresta);
    if (!nova->dado) {
        free(nova);
        return;
    }

    // copia o dado da aresta
    memcpy(nova->dado, pdado, self->tam_aresta);
    nova->prox = self->nos[origem].adj;
    self->nos[origem].adj = nova;
}

// função de uso interno que verifica se uma aresta existe
static bool existe_aresta(Grafo self, int origem, int destino) {
    // se origem ou destino forem inválidos, retorna false
    if (!self || origem < 0 || origem >= self->nnos || destino < 0 || destino >= self->nnos) return false;
    struct aresta *a = self->nos[origem].adj;
    while (a) {
        if (a->destino == destino) return true;
        a = a->prox;
    }
    return false;
}

// função de uso interno que retorna a aresta entre dois nós
static struct aresta *retorna_aresta(Grafo self, int origem, int destino) {
    // se origem ou destino forem inválidos, retorna NULL
    if (!self || origem < 0 || origem >= self->nnos || destino < 0 || destino >= self->nnos) return NULL;
    struct aresta *a = self->nos[origem].adj;
    while (a) {
        if (a->destino == destino) return a;
        a = a->prox;
    }
    return NULL;
}

// função de uso interno que remove uma aresta
static void remove_aresta(Grafo self, int origem, int destino) {
    // ponteiro para ponteiro para facilitar remoção recebe a lista de adjacência do nó de origem
    struct aresta **pa = &self->nos[origem].adj;
    while (*pa) {
        // se a aresta atual é a que deve ser removida
        if ((*pa)->destino == destino) {
            struct aresta *tmp = *pa;
            *pa = (*pa)->prox;
            free(tmp->dado);
            free(tmp);
            return;
        }
        pa = &(*pa)->prox;
    }
}

// altera o valor da aresta que interliga o nó origem ao nó destino
//   (copia o valor de *pdado)
// caso a aresta não exista, deve ser criada
// caso pdado seja NULL, a aresta deve ser removida
void grafo_altera_aresta(Grafo self, int origem, int destino, void *pdado) {
    if (!self) return;

    // se a aresta não existe, ela é criada
    if (!existe_aresta(self, origem, destino)) {
        if (pdado) cria_aresta(self, origem, destino, pdado);
        return;
    }

    // se pdado for NULL, remove a aresta
    if (!pdado) {
        remove_aresta(self, origem, destino);
        return;
    }

    // se a aresta existe, altera o dado
    struct aresta *a = retorna_aresta(self, origem, destino);
    if (a) memcpy(a->dado, pdado, self->tam_aresta);
}

//////////////////////////////////////////////////////////////////////////////////////

// retorna um ponteiro para o valor associado à aresta entre os nós origem e
//   destino existir, ou NULL se tal aresta não existir
void *grafo_aresta(Grafo self, int origem, int destino) {
    struct aresta *a = retorna_aresta(self, origem, destino);
    if (!a) return NULL;
    return a->dado;
}

//////////////////////////////////////////////////////////////////////////////////////
// GRAFO PROXIMA ARESTA

// função de uso interno que verifica se o nó é válido
static bool eh_valido(Grafo self, int origem) {
    // se estiver fora do intervalo de nós, retorna false
    return (self && origem >= 0 && origem < self->nnos);
}

// essas funções iniciam consultas a arestas que partem ou chegam a um nó
// não retornam nada, apenas preparam o grafo para as próximas chamadas da grafo_proxima_aresta

// inicia uma consulta a arestas que partem do nó origem
// as próximas chamadas a 'grafo_proxima_aresta' devem retornar os valores
//   correspondentes à cada aresta que parte desse nó
void grafo_arestas_que_partem(Grafo self, int origem) {
    if (!eh_valido(self, origem)) return;
    // começa a iteração a partir da primeira aresta do nó origem
    self->no_consulta = origem;
    self->aresta_atual = self->nos[origem].adj; // primeira aresta que parte do nó
    self->consulta_por_origem = true;
}

// inicia uma consulta a arestas que chegam ao nó destino
// as próximas chamadas a 'grafo_proxima_aresta' devem retornar os valores
//   correspondentes à cada aresta que chega nesse nó
void grafo_arestas_que_chegam(Grafo self, int destino) {
    if (!eh_valido(self, destino)) return;
    // começa a iteração a partir do primeiro nó
    self->no_consulta = destino;
    self->aresta_atual = NULL; // reinicia a busca
    self->consulta_por_origem = false;
    self->indice_busca = 0; // reinicia a busca do início
}

// função de uso interno que funciona para quando a consulta é por arestas que partem do nó de origem
// passa para a próxima aresta que parte do nó de origem
// iterador de arestas que partem de um nó
static void *proxima_aresta_partindo(Grafo self, int *vizinho) {
    if (!self->aresta_atual) return NULL;

    // pega a aresta atual e avança para a próxima
    struct aresta *a = self->aresta_atual;
    self->aresta_atual = a->prox;

    // se vizinho não for NULL armazena o destino da aresta atual
    if (vizinho) *vizinho = a->destino;

    // retorna o dado da aresta atual
    return a->dado;
}

// função de uso interno que funciona para quando a consulta é por arestas que chegam ao nó de destino
// passa para a próxima aresta que chega ao nó de destino
// recebe vizinho para escrever o índice do nó de origem (quem aponta para o destino)
// iterador de arestas que chegam a um nó (essa tem que passar por todos os nós)
static void *proxima_aresta_chegando(Grafo self, int *vizinho) {
    int destino = self->no_consulta;

    // enquanto ainda tiver nós
    while (self->indice_busca < self->nnos) {
        struct aresta *a = self->aresta_atual; // guarda a aresta atual
        // se a aresta atual for null (pq a busca começou agora ou reiniciou) começa a busca do início da lista de adjacência
        if (!a) {
            a = self->nos[self->indice_busca].adj;
        // se não for null passa pra próxima
        } else {
            a = a->prox;
        }

        // enquanto ainda tiver arestas
        while (a) {
            // se a aresta atual aponta para o nó destino
            if (a->destino == destino) {
                self->aresta_atual = a;
                if (vizinho) *vizinho = self->indice_busca;
                return a->dado;
            }
            a = a->prox;
        }

        self->indice_busca++; // incrementa a posição de busca para o próximo nó
        self->aresta_atual = NULL; // reinicia a aresta atual para a próxima iteração
    }
    return NULL;
}

// retorna um ponteiro para o valor associado à próxima aresta.
// a próxima aresta é definida de acordo com a última consulta iniciada por 
//   'grafo_arestas_que_partem' ou 'grafo_arestas_que_chegam'.
// se vizinho não for NULL, recebe o valor do nó vizinho ao nó da consulta.
// caso não exista mais aresta que satisfaça a consulta, retorna NULL.

void *grafo_proxima_aresta(Grafo self, int *vizinho) {
    if (!self || self->no_consulta < 0) return NULL;

    // se a consulta é por arestas que partem do nó origem
    if (self->consulta_por_origem) {
        return proxima_aresta_partindo(self, vizinho);
    }

    // se a consulta é por arestas que chegam ao nó destino
    return proxima_aresta_chegando(self, vizinho);
}

//////////////////////////////////////////////////////////////////////////////////////
// GRAFO ORDEM TOPOLOGICA

// dfs: vai o mais fundo possível a partir de um nó marcando os que ele já visitou
// se encontrar um nó que já está sendo visitado, há um ciclo
// se chega em null marca esse nó como falso e confirma que não tem ciclo pq não chegou em nenhum já visitado
// retorna true se encontrar ciclo
// estado 0 = não visitado
// estado 1 = visitando
// estado 2 = visitado
// função de uso interno que realiza DFS para detectar ciclos
// o vetor de estados deve ter tamanho igual ao número de nós do grafo e ele tem um item pra cada nó
static bool dfs_tem_ciclo(Grafo g, int v, int *estado) {
    estado[v] = 1;
    
    // ponteiro para a primeira aresta do nó v
    struct aresta *a = g->nos[v].adj;
    while (a) {
        int w = a->destino;
        // se o nó vizinho está sendo visitado, ciclo encontrado
        if (estado[w] == 1) return true;
        // se o nó vizinho não foi visitado, faz DFS nele
        if (estado[w] == 0 && dfs_tem_ciclo(g, w, estado)) return true;
        a = a->prox;
    }
    // marca o nó como visitado e retorna false (sem ciclo encontrado)
    estado[v] = 2;
    return false;
}

// retorna true se grafo é cíclico, false caso contrário
// usei calloc para inicializar o vetor de estados com 0 que significa não visitado
bool grafo_tem_ciclo(Grafo self) {
    if (!self) return false;

    int *estado = calloc(self->nnos, sizeof(int));
    if (!estado) return false;

    // realiza DFS em cada nó não visitado
    for (int i = 0; i < self->nnos; i++)
    // se o nó não foi visitado, inicia DFS
        if (estado[i] == 0 && dfs_tem_ciclo(self, i, estado)) {
            free(estado);
            return true;
        }

    free(estado);
    return false;
}

// o grau de entrada de um nó é o número de arestas que chegam a ele
// função de uso interno que calcula o grau de entrada de cada nó
// itera sobre todas as arestas do grafo e incrementa o grau de entrada do nó de destino
static void calcula_grau_entrada(Grafo self, int *grau_entrada) {
    for (int i = 0; i < self->nnos; i++) {
        struct aresta *a = self->nos[i].adj;
        while (a) {
            grau_entrada[a->destino]++;
            a = a->prox;
        }
    }
}

// retorna uma fila contendo os números dos nós do grafo em uma ordem em
//   que, se o nó 'a' antecede 'b' na lista, não existe um caminho de 'b'
//   para 'a' no grafo
// deve retornar uma fila vazia caso tal ordem não exista
// quem chama esta função é responsável por destruir a fila.

// só da pra fazer ordem topológica em grafos que não tem ciclos (se tiver ciclo não tem como ordenar)
// usa o algoritmo de Kahn (remove nós com grau de entrada 0 e diminui o grau dos vizinhos, se algum vizinho chegar a 0 adiciona na fila)
// a ordem topológica é a ordem de nós que menos dependem de outros 
// não é pra destruir o grafo

// função de uso intero que coloca os nós de grau de entrada zero no início da fila
static void inicializa_fila_grau_zero(Grafo self, int *grau_entrada, Fila fila) {
    for (int i = 0; i < self->nnos; i++) {
        if (grau_entrada[i] == 0) {
            fila_insere(fila, &i);
        }
    }
}

Fila grafo_ordem_topologica(Grafo self) {
    if (!self) return NULL;
    int n = self->nnos;
    // se o grafo não tem nós, retorna uma fila vazia
    if (n == 0) return fila_cria(sizeof(int));

    // vetor para armazenar o grau de entrada de cada nó
    Fila ordem = fila_cria(sizeof(int));
    // aloca e inicializa o vetor de grau de entrada
    int *grau_entrada = calloc(n, sizeof(int));
    if (!grau_entrada) return ordem;

    // calcula o grau de entrada de cada nó
    // o vetor grau_entrada[i] conterá o grau de entrada do nó i
    calcula_grau_entrada(self, grau_entrada);

    // coloca primeiro na fila os nós que tem grau de entrada zero
    Fila fila = fila_cria(sizeof(int));
    inicializa_fila_grau_zero(self, grau_entrada, fila);
    
    int visitados = 0; // contador de nós visitados
    // enquanto a fila ainda tiver os nós de grau zero
    while (!fila_vazia(fila)) {
        int v; // vai receber o nó removido da fila
        fila_remove(fila, &v); // remove o nó no início da fila
        fila_insere(ordem, &v); // adiciona o nó na ordem topológica
        visitados++;

        // aqui ele "retira" o nó v do grafo, então diminui o grau de entrada dos vizinhos
        struct aresta *a = self->nos[v].adj;
        while (a) {
            int w = a->destino;
            grau_entrada[w]--;
            // se o grau de entrada do vizinho chegou a zero, adiciona na fila
            if (grau_entrada[w] == 0) fila_insere(fila, &w);
            a = a->prox;
        }
    }

    // se não visitou todos os nós, significa que há ciclo, retorna fila vazia
    if (visitados < n) {
        fila_destroi(ordem);
        ordem = fila_cria(sizeof(int)); // fila vazia
    }

    free(grau_entrada);
    fila_destroi(fila);
    return ordem;
}
