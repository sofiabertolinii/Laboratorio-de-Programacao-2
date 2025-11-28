#include "bb1.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

// função estática que verifica se a função está correta
// retorna false se o bb1 for inválido ou incoerente
// tamanho negativo, memória nula, tamanho maior que cap
// o tam é o número de caracteres da cadeia
// a mem é o ponteiro para o primeiro caractere da cadeia
// essa função não deve ser chamada diretamente, só por outras funções
// é uma boa prática chamar essa função no início de cada função que recebe um bb1
// para garantir que o bb1 é válido antes de usá-lo
static void bb1_ok(bb1 cad)
{
  assert(cad.tam >= 0);
  assert(cad.tam == 0 || cad.mem != NULL);
}

// função auxiliar para criar um bb1 a partir de uma string
bb1 bb1_cria(char *str)
{
  return (bb1){.tam = strlen(str), .mem = str};
}

// destrói a cadeia cad
void bb1_destroi(bb1 cad)
{
  bb1_ok(cad);
}

// retorna o tamanho (número de bytes) de cad
int bb1_tam(bb1 cad)
{
  bb1_ok(cad);
  return cad.tam;
}

// retorna um ponteiro para o primeiro caractere de cad
// esse ponteiro pode não apontar para uma string compatível com strings
char *bb1_mem(bb1 cad)
{
  bb1_ok(cad);
  return cad.mem;
}

// retorna o caractere na posição pos da cadeia cad
// retorna 0 se pos for fora dos limites de cad
char bb1_ch(bb1 cad, int pos)
{
    bb1_ok(cad);
    if (pos < 0 || pos >= cad.tam) {
        return 0;
    }
    return cad.mem[pos];
}


// retorna uma cadeia que é a substring que inicia na posição pos
// de cad e tem tam bytes
// o ponteiro e tamanho da substring devem ser limitados ao conteúdo de cad
// o bb1 retornado não é alterável, e só aponta para a memória de cad
// o bb1 retornado não pode ser usado se cad for alterada depois
// desta chamada
// por exemplo: se cad contém "barco":
//   pos=2,  tam=3 sub="rco"
// se estiver fora dos limites (negativo), ajusta para zero
// se estiver fora dos limites (maior que o tamanho), ajusta para o final
bb1 bb1_sub(bb1 cad, int pos, int tam)
{
    bb1_ok(cad);

  // ajusta a posição negativa pra contar do final
    if (pos < 0) {
      pos = cad.tam + pos;
    }

    // se passar do início (ainda é negativa) volta pra zero
    if (pos < 0) {
      pos = 0;
    }

    if (pos >= cad.tam) {
    return (bb1){ .tam = 0, .mem = cad.mem + cad.tam };
  }


    // se passar do tamanho, ajusta para o tamanho (vazia)
    if (pos > cad.tam) {
      pos = cad.tam;
    }

    // ajusta o tamanho se passar do final
    if (pos + tam > cad.tam) {
      tam = cad.tam - pos;
    }

    // retorna uma nova bb1 apontando para o pedaço da string
    return (bb1){ .tam = tam, .mem = cad.mem + pos };
}


int bb1_busca_c(bb1 cad, int pos, bb1 chs)
{
  bb1_ok(cad);
  bb1_ok(chs);

  // se a posição for negativa, ajusta para começar a contagem do final
  if (pos < 0) {
    pos = cad.tam + pos;
  }

  // se ainda for negativa ajusta para a posição 0
  if (pos < 0) {
    pos = 0;
  }

  // se a posição for maior ou igual ao tamanho da cadeia, não tem como achar
  if (pos >= cad.tam) {
    return -1;
  }

  for(int i = pos; i < cad.tam; i++) {
    for(int j = 0; j < chs.tam; j++) {
      if (chs.mem[j] == cad.mem[i]) {
        return i;
      }
    }
  }
  
  return -1;
}

// PARA CORRIGIR
// retorna a primeira posição em cad, não antes de pos, onde tem
//   algum caractere não pertencente a chs
// retorna -1 se não encontrar
// se pos referenciar uma posição antes do primeiro caractere de cad, a busca
//   deve iniciar no primeiro caractere
int bb1_busca_nc(bb1 cad, int pos, bb1 chs)
{
  bb1_ok(cad);
  bb1_ok(chs);
  
  if (pos < 0) {
    pos = cad.tam + pos;
  }

  if (pos < 0) {
    pos = 0;
  }
  
  if (pos >= cad.tam) {
    return -1;
  }

  for (int i = pos; i < cad.tam; i++) {
      bool tem = false;
      
      for (int j = 0; j < chs.tam; j++) {
          if (cad.mem[i] == chs.mem[j]) {
              tem = true;
              break;
          }
      }
      if (!tem) {
          return i;
      }
  }


  return -1;
}


// retorna a última posição em cad, não posterior a pos, onde tem
//   algum caractere pertencente a chs
int bb1_busca_rc(bb1 cad, int pos, bb1 chs)
{
    bb1_ok(cad);
    bb1_ok(chs);

    // ajusta pos negativa
    if (pos < 0) {
        pos = cad.tam + pos;
    }

    // se ainda for menor que 0, começa no início
    if (pos < 0) {
        pos = 0;
    }

    // se for maior ou igual ao tamanho, começa no último
    if (pos >= cad.tam) {
        pos = cad.tam - 1;
    }

    for (int i = pos; i >= 0; i--) {
        for (int j = 0; j < chs.tam; j++) {
            if (cad.mem[i] == chs.mem[j]) {
                return i;
            }
        }
    }

    return -1;
}

// retorna a última posição em cad, não posterior a pos, onde tem
//   algum caractere NÃO pertencente a chs
int bb1_busca_rnc(bb1 cad, int pos, bb1 chs)
{
    bb1_ok(cad);
    bb1_ok(chs);

    if (pos < 0) {
        pos = cad.tam + pos;
    }

    if (pos < 0) {
        pos = 0;
    }

    if (pos >= cad.tam) {
        pos = cad.tam - 1;
    }

    for (int i = pos; i >= 0; i--) {
        bool pertence = false;
        for (int j = 0; j < chs.tam; j++) {
            if (cad.mem[i] == chs.mem[j]) {
                pertence = true;
                break;
            }
        }
        if (!pertence) {
            return i;
        }
    }

    return -1;
}


// PARA CORRIGIR
//  retorna a primeira posição em cad, não antes de pos, onde tem
//   a substring buscada
// retorna -1 se não encontrar
// se pos referenciar uma posição antes do primeiro caractere de cad, a busca
//   deve iniciar no primeiro caractere
int bb1_busca_s(bb1 cad, int pos, bb1 buscada)
{
    bb1_ok(cad);
    bb1_ok(buscada);

    if (pos < 0) {
        pos = cad.tam + pos;
    }
    if (pos < 0) {
        pos = 0;
    }
    if (pos >= cad.tam) {
        return -1;
    }

    if (buscada.tam == 0) {
        return pos; // string vazia sempre "encontra"
    }
    if (buscada.tam > cad.tam - pos) {
        return -1; // impossível caber
    }

    for (int i = pos; i <= cad.tam - buscada.tam; i++) {
        int j;
        for (j = 0; j < buscada.tam; j++) {
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


// PARA CORRIGIR
// retorna true se cad e cadb forem iguais, falso caso sejam diferentes
// se não forem do mesmo tamanho, são diferentes
bool bb1_igual(bb1 cad, bb1 cadb)
{
  bb1_ok(cad);
  bb1_ok(cadb);

  // se os tamanhos forem diferentes 
  if (cad.tam != cadb.tam) {
    return false;
  }

  // se algum dos carcateres for diferente retorna falso
  for(int i = 0; i < cad.tam; i++) {
    if(cad.mem[i] != cadb.mem[i]) {
      return false;
    }
  }

  return true;
}

// imprime a cadeia em cad na saída padrão
// se cad for nulo, não imprime nada
// se cad for vazio, imprime uma string vazia
// se cad for não nulo, imprime a cadeia na saída padrão
void bb1_imprime(bb1 cad)
{
  bb1_ok(cad);
  fwrite(cad.mem, 1, cad.tam, stdout);
  // poderia ser também printf("%.*s", cad.tam, cad.mem);
}
