#include "bb2.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

bb2 bb2_cria(char *str)
{
  return (bb2){.tam = strlen(str), .cap = 0, .mem = str};
}

bb2 bb2_cria_vec(int n, char vec[n])
{
    return (bb2){ .tam = strlen(vec), .cap = n, .mem = vec };
}

// bb2_destroi não faz nada por enquanto, só existe para criar bons costumes

int bb2_tam(bb2 cad)
{
  assert(cad.tam >= 0); // verifica se o tamanho é não negativo
  return cad.tam;
}

char *bb2_mem(bb2 cad)
{
  assert(cad.tam >= 0); // verifica se o tamanho é não negativo
  return cad.mem;
}

char bb2_ch(bb2 cad, int pos)
{
  assert(cad.tam >= 0); // verifica se o tamanho é não negativo

  if (pos < 0) {
    pos = cad.tam + pos;
  }

  if (pos < 0 || pos >= cad.tam) {
    return 0; // retorna 0 se a posição for inválida
  }

  return cad.mem[pos];
}

bb2 bb2_sub(bb2 cad, int pos, int tam) {
    assert(cad.tam >= 0); // verifica se o tamanho é não negativo
    
    // ajusta a posição negativa pra contar do final
    if (pos < 0) {
        pos = cad.tam + pos;
    }
    
    // se passar do início (ainda é negativa) volta pra zero
    if (pos < 0) {
        pos = 0;
    }
    
    // se pos for maior que o tamanho, retorna uma bb2 vazia
    // que aponta para o final da string
    if (pos >= cad.tam) {
        return (bb2){ .tam = 0, .cap = cad.cap, .mem = cad.mem + cad.tam };
    }
    
    // ajusta o tamanho se passar do final
    if (pos + tam > cad.tam) {
        tam = cad.tam + pos;
    }
    
    // retorna uma nova bb2 apontando para o pedaço da string
    return (bb2){ .tam = tam, .cap = cad.cap, .mem = cad.mem + pos };   
}

// primeira posição em cad, não antes de pos, onde tem algum caractere
// pertencente a chs
// retorna -1 se não encontrar
int bb2_busca_c(bb2 cad, int pos, bb2 chs) {

  if (pos < 0) {
    pos = cad.tam + pos;
  }

  if (pos < 0) {
      pos = 0; // ajusta para o início se pos for negativa
  }

  if (pos >= cad.tam) {
      return -1; // retorna -1 se pos for maior que o tamanho
  }

  for (int i = pos; i < cad.tam; i++) {
      for (int j = 0; j < chs.tam; j++) {
          if (cad.mem[i] == chs.mem[j]) {
              return i; // retorna a posição encontrada
          }
      }
  }
  return -1; // não encontrou
}

int bb2_busca_nc(bb2 cad, int pos, bb2 chs)
{
    if (pos < 0) pos = cad.tam + pos;
    if (pos < 0) pos = 0;
    if (pos >= cad.tam) return -1;

    for (int i = pos; i < cad.tam; i++) {
        int pertence = 0;
        for (int j = 0; j < chs.tam; j++) {
            if (cad.mem[i] == chs.mem[j]) { pertence = 1; break; }
        }
        if (!pertence) return i;
    }
    return -1;
}

int bb2_busca_rc(bb2 cad, int pos, bb2 chs)
{
    if (pos < 0) pos = cad.tam + pos;
    if (pos < 0) pos = 0;
    if (cad.tam == 0) return -1;
    if (pos >= cad.tam) pos = cad.tam - 1;

    for (int i = pos; i >= 0; i--) {
        for (int j = 0; j < chs.tam; j++) {
            if (cad.mem[i] == chs.mem[j]) return i;
        }
    }
    return -1;
}

int bb2_busca_rnc(bb2 cad, int pos, bb2 chs)
{
    if (pos < 0) pos = cad.tam + pos;
    if (pos < 0) pos = 0;
    if (cad.tam == 0) return -1;
    if (pos >= cad.tam) pos = cad.tam - 1;

    for (int i = pos; i >= 0; i--) {
        int pertence = 0;
        for (int j = 0; j < chs.tam; j++) {
            if (cad.mem[i] == chs.mem[j]) { pertence = 1; break; }
        }
        if (!pertence) return i;
    }
    return -1;
}


// retorna a primeira posição em cad, não ANTES de pos, onde tem a 
// substring buscada
// retorna -1 se não encontrar
int bb2_busca_s(bb2 cad, int pos, bb2 buscada)
{
  if(pos < 0) {
    pos = cad.tam + pos;
  }

  if (pos < 0) {
    pos = 0;
  }

  if (pos >= cad.tam) {
    return -1;
  }

  if (buscada.tam > cad.tam - pos) {
    return -1;
  }

  for(int i = pos; i <= cad.tam - buscada.tam; i++) {
    int j;
    for(j = 0; j < buscada.tam; j++) {
      if (cad.mem[i + j] != buscada.mem[j]) {
        break;
      }
    }
    if (j == buscada.tam) {
      return i;
    }
  }
  return -1;
}

bool bb2_igual(bb2 cad, bb2 cadb) 
{
  if (cad.tam != cadb.tam) {
    return false;
  }

  for(int i = 0; i < cad.tam; i++) {
    if (cad.mem[i] != cadb.mem[i]) {
      return false;
    }
  }

  return true;
}

void bb2_imprime(bb2 cad) 
{
  fwrite(cad.mem, 1, cad.tam, stdout);
}

// adiciona ao final da cadeia pcad o conteúdo da cadeia cadb
// não se faz NADA SE A CADEIA PCAD NÃO FOR ALTERÁVEL
// cap != 0
// coloca um \0 no final da cadeia em pcad (tem que sobrar um espaço)
// não adiciona mais caracteres do que cabe em pcad
void bb2_cat(bb2 *pcad, bb2 cadb) 
{
    if (pcad->cap == 0) {
        return; // não faz nada se não for alterável
    }

    int espaco = pcad->cap - pcad->tam - 1; // espaço livre para adicionar caracteres
    if (espaco <= 0) {
        return; // não há espaço para adicionar mais caracteres
    }

    int j;
    for (j = 0; j < cadb.tam && j < espaco; j++) {
        pcad->mem[pcad->tam + j] = cadb.mem[j];
    }

    pcad->tam += j;                 // atualiza o tamanho
    pcad->mem[pcad->tam] = '\0';    // finaliza a string
}


// adiciona o caractere c ao FINAL da cadeia apontada por pcad
// não faz nada se a cadeia apontada por pcad não for alterável
// não faz nada se não tiver espaço pro caractere no vetor
// colcoa um \0 no final
void bb2_catc(bb2 *pcad, char c)
{
  if (pcad->cap == 0) {
    return;
  }

  // tem que sobrar 2 espaços no final da string (um pro caractere c e outro pro \0)
  if (pcad->cap - pcad->tam < 2 ) {
    return;
  }

  pcad->mem[pcad->tam] = c;
  pcad->mem[pcad->tam + 1] = '\0';
  pcad->tam++;
}

void bb2_insere(bb2 *pcad, int pos, bb2 cadb)
{
    if (pcad->cap == 0) return;

    if (pos < 0) pos = pcad->tam + pos;  // negativo conta do fim
    if (pos < 0) pos = 0;
    if (pos > pcad->tam) pos = pcad->tam;

    int espaco = pcad->cap - pcad->tam - 1;
    if (espaco <= 0) return;

    int n = cadb.tam;
    if (n > espaco) n = espaco;

    // desloca sufixo para frente (de trás pra frente para não sobrescrever)
    for (int i = pcad->tam - 1; i >= pos; i--) {
        pcad->mem[i + n] = pcad->mem[i];
    }

    // copia cadb no espaço aberto
    for (int j = 0; j < n; j++) {
        pcad->mem[pos + j] = cadb.mem[j];
    }

    pcad->tam += n;
    pcad->mem[pcad->tam] = '\0';
}

void bb2_remove(bb2 *pcad, int pos, int tam)
{
    if (pcad->cap == 0) return;
    if (tam <= 0) return;

    if (pos < 0) pos = pcad->tam + pos;
    if (pos < 0) pos = 0;
    if (pos >= pcad->tam) return;

    int from = pos + tam;
    if (from > pcad->tam) from = pcad->tam; // cortar até o fim

    // move inclusive o '\0'
    int to = pos;
    while (from <= pcad->tam) {
        pcad->mem[to++] = pcad->mem[from++];
    }

    // ajusta tamanho
    int novo_tam = pcad->tam - tam;
    if (novo_tam < pos) novo_tam = pos;
    if (novo_tam < 0) novo_tam = 0;
    pcad->tam = novo_tam;
}



void bb2_preenche(bb2 *pcad, int tam, char c)
{
    // se não for alterável
    if (pcad->cap == 0) {
        return;
    }

    // se já tiver tamanho >= tam, não precisa preencher
    if (pcad->tam >= tam) {
        return;
    }

    // calcula quanto pode preencher sem ultrapassar a capacidade
    // precisa sobrar espaço para o terminador '\0'
    int max_preencher = pcad->cap - 1;
    if (tam > max_preencher) {
        tam = max_preencher;
    }

    // preenche com o caractere c até atingir "tam"
    for (int i = pcad->tam; i < tam; i++) {
        pcad->mem[i] = c;
    }

    // fecha a string
    pcad->mem[tam] = '\0';
    pcad->tam = tam;
}

