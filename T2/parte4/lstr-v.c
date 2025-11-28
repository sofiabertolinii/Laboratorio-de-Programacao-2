
// PARTE 2


// a estrutura lstr é opaca e nao é acessada pelo usuário
// implementar as funções de lstr.h usando as funções de str.c
// usar ALOCAÇÃO CONTÍGUA DE MEMÓRIA
// e uma área de memória contígua também alocada onde sçao colocadas as strings
// a área para strings deve ser realocada conforme necessidade
// a alocação mínima é de 8 strings
// se a memória estiver cheia deve ser realocada com tamanho 50% maior
// em uma remoção, se a região ocupada for inferior a metade da capacidade,
// deve ser realocada com tamanho 25% maior que a ocupação

#include "lstr.h"
#include "str.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#define LSTR_CAP_MIN 8  // capacidade mínima da lista


struct lstr {
    str *itens; // área de strings alocada dinamicamente com alocação contígua
    int tam; // quantidade de strings alocadas
    int cap; // capacidade
    int pos; // posição corrente
};

// cria e retorna uma lista vazia
// o ponteiro retornado aponta para uma estrutura recém alocada, que deve
//   ser destruída (com ls_destroi) quando a lista não for mais necessária
Lstr ls_cria() {
    struct lstr *lista = malloc(sizeof(struct lstr)); // aloca do tamanhod da struct da lista
    if (lista == NULL) {
        printf("Erro de alocacao.\n");
        return NULL;
    }

    lista->tam = 0; // ainda tem 0 str guardadas
    lista->cap = LSTR_CAP_MIN; // a capacidade é a mínima (8)
    lista->pos = -1; // posição antes do início

    // aloca o array de strings
    lista->itens = malloc(lista->cap * sizeof(str));
    if (lista->itens == NULL){ // se der erro libera o que já foi alocado
        printf("Erro de alocacao.\n");
        free(lista);
        return NULL;
    }

    return lista;
}

// destrói uma lista 
// essa lista não deve ser utilizada após essa chamada 
// esta função destrói a lista, não as strings que ela contém 
// dar free na memória alocada da lista

void ls_destroi(Lstr self) {
    if (self == NULL) {
        return;
    }

    free(self->itens);
    free(self); // libera a struct
}


// retorna true se não houver nenhum elemento na lista
bool ls_vazia(Lstr self) {
    return self->tam == 0;
}

// retorna o tamanho (número de elementos) da lista
int ls_tam(Lstr self) {
    return self->tam;
}

// retorna true se existe um item na posição corrente
// se a posição for -1 é a posição inicial (não tem item lá)
// se for maior que o tamanho da lista é uma posição inválida
bool ls_item_valido(Lstr self) {
    return (self->pos >= 0 && self->pos < self->tam);
}

// retorna a string na posição corrente da lista
// essa função não deve ser chamada se a posição corrente estiver antes do
//   início ou depois do final da lista

str ls_item(Lstr self) {
    if (!ls_item_valido(self)) {
        printf("Erro: Posição inválida em ls_item");
        exit(1);
    }

    return self->itens[self->pos];
}


// posiciona antes do início da lista
void ls_inicio(Lstr self) {
    self->pos = -1;
}

// posiciona após o final da lista
void ls_final(Lstr self) {
    self->pos = self->tam;
}


// USEI O NULL PARA REPRESENTAR AS POSIÇÕES ANTES OU DEPOIS DAS POSIÇÕES VÁLIDAS



// altera a posição corrente para a posição do pos-ésimo elemento da lista
// pos pode ser negativo, conta a partir do final da lista
// pos pode representar uma posição após o último ou antes do primeiro item,
//   caso em que altera a posição corrente para uma das posições especiais
void ls_posiciona(Lstr self, int pos) {
    int destino;

    if (pos >= 0) {
        destino = pos;
    } else {
        // conta a partir do final
        destino = self->tam + pos;
    }

    // agora trata os limites
    if (destino < 0) {
        self->pos = 0; // antes do primeiro
    } else if (destino > self->tam - 1) {
        self->pos = self->tam; // após o último
    } else {
        self->pos = destino; // posição válida
    }
}

// avança para a próxima posição da lista
// se a posição corrente for anterior ao início, passa para a posição do
//   primeiro item da lista
// passa para a posição após o final se a posição corrente for a posição
//   do último elemento da lista ou se a lista estiver vazia
// continua na posição após o final se já estiver nela
// retorna true se a posição resultante contiver um item válido (retorna
//   false se a posição resultante for após o final da lista)
bool ls_avanca(Lstr self) {
    if(self == NULL) {
        return false;
    }

    if (self->pos < 0) { // se a posição for antes do inicio passa pro primeiro da lista
        self->pos = 0;
    } else if (self->pos == self->tam - 1 || self->tam == 0) { // se a posição for a do ultimo elemento ou se ela estiver vazia
        self->pos = self->tam;
    } else if (self->pos < self->tam) { // avança normalmente
        self->pos++;
    }

    return ls_item_valido(self);
}

// recua para a posição anterior da lista
// se a posição corrente for após o final, passa para a posição do último
//   item da lista 
// passa para a posição antes do início se a posição corrente for a do
//   primeiro elemento da lista ou se a lista estiver vazia
// continua na posição antes do início se já estiver nela
// retorna true se a posição resultante contiver um item válido (retorna
//   false se a posição resultante for antes do início da lista)
bool ls_recua(Lstr self) {
    if (self == NULL) {
        return false;
    }

    if (self->tam == 0) { // se a lista ta vazia
        self->pos = -1;
        return false;
    }

    if (self->pos >= self->tam) { // se a posição ta após o final
        self->pos = self->tam - 1;
    } else if (self->pos == 0) { // se a posição é o primeiro da list
        self->pos = -1;
    } else {
        self->pos--;
    }

    return ls_item_valido(self);
}

// fazer uma função auxiliar que realoca a memória
// A área para as strings deve ser realocada conforme necessidade. Pode não ter nada alocado para uma lista vazia, ou com alocação mínima. 
// A alocação mínima deve comportar 8 strings. Em uma inserção, se a memória estiver cheia, a região deve ser realocada com tamanho 50% maior 
// (se era 20, vira 30). Em uma remoção, se a região ocupada for inferior à metade da capacidade, deve ser realocada com tamanho 25% maior que a 
// ocupação (se tinha espaço para 20 e ficou com 9, realoca para 11).
static void realoca(Lstr self) {
    int nova_cap = self->cap; // começa igual a atual

    // se não tem nada na lista ainda define a capacidade mínima
    if (self->tam == 0) { 
        nova_cap = LSTR_CAP_MIN;
    } 

    // se está cheio aumenta metade
    else if (self->tam == self->cap) { 
        nova_cap = (int)(self->cap * 1.5);
    } 

    // se for metade ou menos vira 25% maior que o ocupado
    else if (self->tam < self->cap / 2) { 
        nova_cap = (int)(self->tam * 1.25);
        if (nova_cap < LSTR_CAP_MIN) { // nunca menor que o mínimo
            nova_cap = LSTR_CAP_MIN;
        }
    }

    // se a nova capacidade for diferente da atual
    if (nova_cap != self->cap) {
        str *novo = realloc(self->itens, nova_cap * sizeof(str));
        if (novo == NULL) {
            printf("Erro de alocacao.\n");
            return;
        }
        self->itens = novo;
        self->cap = nova_cap;
    }
}



// insere a string cad ANTES da posição corrente da lista
// caso a posição corrente seja antes do início, insere no início; 
// caso seja após o fim, insere no fim
// a posição corrente passa a ser a do item inserido
void ls_insere_antes(Lstr self, str cad) {
    realoca(self);

    if(self->pos < 0) { // se ta antes do começo
        for(int i = self->tam; i > 0; i--) { // move tudo uma posição pra frente
            self->itens[i] = self->itens[i - 1];
        }
        self->itens[0] = cad;
        self->pos = 0;

    } else if(self->pos > self->tam - 1) { // se ta na última posição
        self->itens[self->tam] = cad;
        self->pos = self->tam;
    } else {
        for(int i = self->tam; i > self->pos; i--) { // em casos onde a posição esta no meio da lista
            self->itens[i] = self->itens[i - 1];
        }
        self->itens[self->pos] = cad;
    }
    self->tam++;
}

// insere a string cad após a posição corrente da lista
// caso a posição corrente seja antes do início, insere no início; caso seja
//   após o fim, insere no fim
// a posição corrente passa a ser a do item inserido
void ls_insere_depois(Lstr self, str cad) {
    realoca(self);

    if (self->pos < 0) {
        for (int i = self->tam; i > 0; i--) {
            self->itens[i] = self->itens[i - 1];
        }
        self->itens[0] = cad;
        self->pos = 0;
    } 
    else if (self->pos >= self->tam - 1) {
        self->itens[self->tam] = cad;
        self->pos = self->tam;
    } 
    else {
        for (int i = self->tam; i > self->pos + 1; i--) {
            self->itens[i] = self->itens[i - 1];
        }
        self->itens[self->pos + 1] = cad;
        self->pos++;
    }

    self->tam++;
}

// remove e retorna a string da posição corrente, que deve ser válida
// a posição corrente passa a ser a do item seguinte ao removido ou após o
// final se foi removido o item no final da lista
str ls_remove(Lstr self) {
    assert(ls_item_valido(self)); // verifica se é uma posicao valida

    str removido = self->itens[self->pos]; // guarda o item a ser removido

    // desloca todos os elementos depois de pos uma posição para trás
    for(int i = self->pos; i < self->tam - 1; i++) {
        self->itens[i] = self->itens[i + 1];
    }

    self->tam--; // diminui o tamanho

    // ajusta pos: se removeu o último, pos vai após o final
    if (self->pos >= self->tam) {
        self->pos = self->tam;  
    }

    realoca(self);
    return removido;
}

// retorna uma nova lista que é uma cópia dos tam itens de self iniciando na posição
//   corrente
// se tam for 0 ou negativo, ou se a posição corrente da lista for após o final,
//   retorna uma lista vazia
// se não houverem tam itens a partir da posição corrente, a lista retornada
//   conterá menos de tam itens (os tantos que existem a partir da posição corrente)
// a posição corrente de self é alterada para após o último item copiado
// a posição corrente da nova lista é antes do primeiro item
Lstr ls_sublista(Lstr self, int tam) {
    Lstr nova = ls_cria();

    // caso inválido retorna lista vazia
    if(tam <= 0 || self->pos >= self->tam) {
        return nova;
    }

    // calcula quantos itens realmente vão ser copiados
    int qtd = self->tam - self->pos;
    if(qtd > tam) {
        qtd = tam;
    }

    // copia os itens
    for(int i = 0; i < qtd; i++) {
        nova->itens[i] = self->itens[self->pos + i];
    }

    // ajusta tamanhos
    nova->tam = qtd;
    self->pos += qtd; // posição de self vai para após o último copiado
    nova->pos = 0; // posição de nova começa no início 

    return nova;
}


// imprime todos os itens da lista
// após a impressão, a posição corrente pode ser qualquer
void ls_imprime(Lstr self) {
    for(int i = 0; i < self->tam; i++) {
        printf("%s", s_mem(self->itens[i]));  // s_mem retorna char* interno
    }
}

// retorna uma string (nova, alterável, que deve ser destruída) contendo
//   a concatenação de todas as strings em lista, separadas pela string
//   em separador
// junta(["abacaxi", "abóbora", "abacate"], ", ") -> "abacaxi, abóbora, abacate"
str ls_junta(Lstr self, str separador) {
    if (ls_vazia(self)) {
        str vazio;
        vazio.mem = NULL;
        vazio.tamb = 0;
        vazio.tamc = 0;
        return vazio;
    }

    // cria string inicial como cópia do primeiro item
    ls_inicio(self);
    ls_avanca(self); // posiciona no primeiro item
    str resultado = s_copia(ls_item(self));

    // percorre o resto da lista
    while (ls_avanca(self)) {
        s_cat(&resultado, separador);   // adiciona separador
        s_cat(&resultado, ls_item(self)); // adiciona item
    }

    return resultado;
}

