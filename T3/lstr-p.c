


// PARTE 3 - IMPLEMENTAÇÃO UTILIZANDO A LISTA DUPLAMENTE ENCADEADA



#include "str.h"
#include "lstr.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

// faz um nó que guarda a struct str
// int pos pra posição corrente
// faz um ponteiro que aponta pro próximo
// nas listas encadeadas o ultimo sempre aponta pra NULL



// todas as operações de lista são implementadas por funções prefixadas por
// `ls_`. O primeiro argumento dessas funções (exceto `ls_cria`) é um ponteiro
// para a lista objeto dessa operação, declarado como `Lstr self`, que foi
// obtido por uma chamada a `ls_cria` e que ainda não foi utilizado em uma
// chamada a `ls_destroi`.
//
// a lista tem o conceito de "posição corrente", que representa a posição do item
// que pode ser acessado (com `ls_item`).
// as operações `ls_avanca` e `ls_recua` alteram essa posição.
// existem duas posições especiais que não correspondem a um item, que são o início
// e o final da lista. Nessas posições não se pode chamar `ls_item`.
// elas são atingidas quando se avança após o último item, quando se recua antes
// do primeiro, quando a lista está vazia, quando se remove o item final da lista.
//
// o percurso de uma lista é tipicamente feito assim:
//   for (ls_inicio(l); ls_avanca(l); ) {
//     dado = ls_item(l);
//     // ...
//   }
//
// quando um item é inserido na lista, a posição corrente passa a ser a posição do
//   item inserido.
// quando um item é removido da lista, a posição corrente passa a ser a posição do
//   item que estava após o item removido.
// a posição corrente pode também ser alterada diretamente por `ls_inicio`, 
//   `ls_final` e `ls_posiciona`.

struct no {
  str dado; // armazena a string na struct str 
  struct no *ant; // guarda um ponteiro pro anterior (se eu resolver fazer a lista duplamente encadeada)
  struct no *prox; // guarda um ponteiro pro próximo 
};

// struct que vai guardar a lista 
// na verdade nao guarda a lista, só tem um ponteiro pro primeiro, pro último e pra posicão corrente da lista
// além do tamanho
struct lstr {
  struct no *ini; // ponteiro pro primeiro elemento da lista
  struct no *fim; // ponteiro pro último elemento da lista
  struct no *corr; // ponteiro pro elemento atual da lista
  int tam; // tamanho da lista
  int pos; // a posição absoluta (vai diferenciar no caso das posições especiais)
};

// operações de criação e destruição {{{1

// OK
// cria e retorna uma lista vazia
// o ponteiro retornado aponta para uma estrutura recém alocada, que deve
//   ser destruída (com ls_destroi) quando a lista não for mais necessária
Lstr ls_cria() {
    Lstr lista = malloc(sizeof(struct lstr));
    if(lista == NULL) {
        printf("Erro de alocacao. A lista nao pode ser criada.\n");
        return NULL;
    }

    lista->ini = NULL;
    lista->fim = NULL;
    lista->corr = NULL;
    lista->tam = 0;
    lista->pos = -1; // começa antes do início

    return lista;
}

// destrói uma lista
// essa lista não deve ser utilizada após essa chamada
// esta função destrói a lista, não as strings que ela contém

// tem que destruir a lista tambem
// OK
void ls_destroi(Lstr self) {
    if (!self) return;

    struct no *p = self->ini;
    while (p != NULL) {
        struct no *prox = p->prox;
        s_destroi(p->dado); // libera a string guardada no nó
        free(p);
        p = prox;
    }

    free(self);
}


// operações de acesso {{{1

// OK
// retorna true se não houver nenhum elemento na lista
bool ls_vazia(Lstr self) {
    return self->tam == 0;
}

// OK
// verifica se a posição está antes do início
// retorna true se estiver antes do inicio
static bool ls_antes_inicio(Lstr self) {
    return self->corr == NULL && self->pos < 0;
}

// OK
// verifca se a posição está após o fim
// retorna true se estiver apos o fim
static bool ls_apos_fim(Lstr self) {
    return self->corr == NULL && self->pos >= self->tam;
}

// OK
// retorna o tamanho (número de elementos) da lista
int ls_tam(Lstr self) {
    return self->tam;
}

// OK
// retorna true se existe um item na posição corrente
bool ls_item_valido(Lstr self) {
    return !(ls_antes_inicio(self) || ls_apos_fim(self));
}


// OK
// retorna a string na posição corrente da lista
// essa função não deve ser chamada se a posição corrente estiver antes do
//   início ou depois do final da lista

// retorna uma string não alterável

str ls_item(Lstr self) {
    if (!ls_item_valido(self)) {
        exit(1);
    }
    return s_sub(self->corr->dado, 0, s_tam(self->corr->dado));
}

// OK
// retorna um ponteiro pra string na posição corrente
str *ls_item_ptr(Lstr self) {
    if (!ls_item_valido(self)) {
        return NULL;
    }
    return &self->corr->dado;
}

// operações de percurso {{{1

// OK
// posiciona antes do início da lista
void ls_inicio(Lstr self) {
    self->corr = NULL;
    self->pos = -1;
}

// OK
// posiciona após o final da lista
void ls_final(Lstr self) {
    self->corr = NULL;
    self->pos = self->tam;
}

// OK
// altera a posição corrente para a posição do pos-ésimo elemento da lista
// pos pode ser negativo, conta a partir do final da lista
// pos pode representar uma posição após o último ou antes do primeiro item,
//   caso em que altera a posição corrente para uma das posições especiais
void ls_posiciona(Lstr self, int pos) {

    if (ls_vazia(self)) {
        ls_inicio(self); 
        return;
    }
    
    // se pos é negativo, conta a partir do final
    if (pos < 0) {
        pos += self->tam;
    }

    // se ainda é negativo, fica antes do início
    if (pos < 0) {
        ls_inicio(self);
        return;
    }

    // se é maior ou igual ao tamanho, fica após o fim
    if (pos >= self->tam) {
        ls_final(self);
        return;
    }

    // caso normal percorre até o elemento desejado
    struct no *temp = self->ini;
    for (int i = 0; i < pos; i++) {
        temp = temp->prox;
    }

    self->corr = temp;
    self->pos = pos;
}

// OK
// avança para a próxima posição da lista
// se a posição corrente for anterior ao início, passa para a posição do
//   primeiro item da lista
// passa para a posição após o final se a posição corrente for a posição
//   do último elemento da lista ou se a lista estiver vazia
// continua na posição após o final se já estiver nela
// retorna true se a posição resultante contiver um item válido (retorna
//   false se a posição resultante for após o final da lista)
bool ls_avanca(Lstr self) {
    // caso lista vazia
    if (ls_vazia(self)) {
        ls_final(self); 
        return false;
    }

    // se está antes do início
    if (ls_antes_inicio(self)) {
        self->corr = self->ini;
        self->pos = 0;
        return true;
    }

    // se está no último elemento
    if (self->corr == self->fim && self->pos == self->tam - 1) {
        ls_final(self); 
        return false;
    }

    // se já está após o final
    if (ls_apos_fim(self)) {
        return false;
    }

    // caso normal
    self->corr = self->corr->prox;
    self->pos++;
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
    if(ls_vazia(self)) {
        ls_inicio(self);
        return false;
    }

    // se tá depois do final
    if(ls_apos_fim(self)) {
        self->corr = self->fim;
        self->pos = self->tam - 1;
        return true;
    }

    // se for o primeiro elemento
    if(self->corr == self->ini && self->pos == 0) {
        ls_inicio(self);
        return false;
    }

    // se ja esta antes do inicio
    if(ls_antes_inicio(self)) {
        return false;
    }

    // casos normais
    self->corr = self->corr->ant;
    self->pos--;
    return(ls_item_valido(self));
}

// operações de alteração da lista {{{1

// criei as statics pra usar na insere antes e insere depois
// função static que insere no início pra evitar repetição de código
static void ls_insere_inicio(Lstr self, struct no *novo) {
    self->ini = novo;
    self->fim = novo;
    self->corr = novo;
    self->pos = 0;
}

// função static que insere no fim pra evitar repetição de código
static void ls_insere_fim(Lstr self, struct no *novo) {
    novo->prox = NULL;
    novo->ant = self->fim;

    if (self->fim != NULL) {
        self->fim->prox = novo;
    }

    self->fim = novo;
    
    if (self->ini == NULL) {
        self->ini = novo;
    }
    self->corr = novo;
    self->pos = self->tam; // posição do novo nó antes de incrementar tam
}

// insere a string cad antes da posição corrente da lista
// caso a posição corrente seja antes do início, insere no início; caso seja
//   após o fim, insere no fim
// a posição corrente passa a ser a do item inserido
void ls_insere_antes(Lstr self, str cad) {
    struct no *novo = malloc(sizeof(struct no));
    if (!novo) {
        printf("Erro de alocacao. O item nao foi inserido na lista.\n");
        return;
    }

    novo->dado = cad;
    novo->ant = novo->prox = NULL;

    // caso lista esteja vazia
    if (self->tam == 0) {
        self->ini = novo;
        self->fim = novo;
        self->corr = novo;
        self->pos = 0;
    }

    // caso seja antes do início
    else if (ls_antes_inicio(self)) {
        ls_insere_inicio(self, novo);
    }

    // caso seja depois do fim
    else if (ls_apos_fim(self)) {
        ls_insere_fim(self, novo);
    }  

    // casos normais (inserir antes do corrente)
    else {
        struct no *antes = self->corr->ant;
        novo->prox = self->corr;
        novo->ant = antes;
        self->corr->ant = novo;
        // verifica se tinha um antes pra não dar segmentation fault
        if(antes) {
            antes->prox = novo;
        
        // se não tinha
        } else {
            self->ini = novo; 
        }
        self->corr = novo;
        // pos não muda, porque o novo ocupou a posição da corrente
    }

    self->tam++;
}



// insere a string cad após a posição corrente da lista
// caso a posição corrente seja antes do início, insere no início; caso seja
//   após o fim, insere no fim
// a posição corrente passa a ser a do item inserido
void ls_insere_depois(Lstr self, str cad) {
    struct no *novo = malloc(sizeof(struct no));
    if(novo == NULL) {
        printf("Erro de alocacao. O item nao foi inserido na lista.\n");
        return;
    }

    novo->dado = cad;
    novo->ant = NULL;
    novo->prox = NULL;

    // caso lista esteja vazia
    if (self->tam == 0) {
        self->ini = novo;
        self->fim = novo;
        self->corr = novo;
        self->pos = 0;
    }

    // caso seja antes do início
    else if (ls_antes_inicio(self)) {
        ls_insere_inicio(self, novo);
    }

    // caso seja depois do fim
    else if (ls_apos_fim(self)) {
        ls_insere_fim(self, novo);
    }  

    // casos normais
    // casos normais (inserir depois do corrente)
    else {
        struct no *depois = self->corr->prox;

        novo->ant = self->corr;
        novo->prox = depois;

        self->corr->prox = novo;

        // verifica se tinha algo após esse elemento
        if (depois) {
            depois->ant = novo;
        
        // se essa era o último elemento
        } else {
            self->fim = novo;
        }

        self->corr = novo;
        self->pos++; // agora a posição corrente é a do novo
    }

    self->tam++;
}

// remove e retorna a string da posição corrente, que deve ser válida
// a posição corrente passa a ser a do item seguinte ao removido ou após o
//   final se foi removido o item no final da lista
str ls_remove(Lstr self) {
    if (!ls_item_valido(self)) {
        printf("Erro: ls_remove chamado em posição inválida.\n");
        exit(1);
    }

    struct no *apagado = self->corr;
    str dado = apagado->dado;

    struct no *anterior = apagado->ant;
    struct no *depois = apagado->prox;

    // se não for o primeiro
    if (anterior) {
        anterior->prox = depois;
    }
    else {
        self->ini = depois; // estava no primeiro
    }

    // se não for o último
    if (depois) {
        depois->ant = anterior;
    }
    else {
        self->fim = anterior; // estava no último
    }

    // move posição corrente
    if (depois) {
        self->corr = depois;
        // pos não muda (pois o próximo assume a mesma posição)
    } else {
        self->corr = NULL;
        self->pos = self->tam - 1; // vai virar tam-1 após decrementar
        ls_final(self); // garante corr = NULL e pos = tam
    }

    free(apagado);
    self->tam--;

    return dado;
}


// outras operações {{{1

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

    // se o tamanho for negativo ou estiver após o final
    if (tam <= 0 || ls_apos_fim(self)) {
        return nova; // lista vazia
    }

    int n = 0;
    while (ls_item_valido(self) && n < tam) {
        str item = ls_item(self);
        ls_insere_depois(nova, item);
        ls_avanca(nova);  // para garantir que a corrente vai andar junto
        ls_avanca(self);  // avança na lista original
        n++;
    }

    ls_inicio(nova); // posição inicial na nova lista
    return nova;
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


// imprime todos os itens da lista
// após a impressão, a posição corrente pode ser qualquer
void ls_imprime(Lstr self) {
    if(self == NULL) {
        printf("A lista tem tamanho %d\n", self->tam);
        printf("Erro. Nao e possivel imprimir uma lista vazia\n");
    }

    struct no *temp = self->ini;
    while(temp != NULL) {
        printf("%s\n", s_mem(temp->dado));
        temp = temp->prox;
    }
}
