#include "cc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MIN_ALLOC 8    // alocação mínima
// a alocação vai aumentando em potências de dois

static void cc_ok(cc cad)
{
  assert(cad.tam >= 0);
  assert(cad.cap >= 0);
  assert(cad.mem != NULL);
  if (cad.cap > 0) {
    assert(cad.cap > cad.tam);
    assert(cad.cap >= MIN_ALLOC);
    assert(cad.cap == (cad.cap & -cad.cap)); // é potência de 2?
  }

}

// retorna uma string não alterável com o conteúdo da string str, pois cap == 0
cc cc_cria(char *str)
{
  return (cc){.tam = strlen(str), .cap = 0, .mem = str};
}

// as funções static que eu declarei são usadas como auxiliares dentro do próprio código
// tem a cc_copia_2 que copia os bytes de src para dest
// tem a cc_move que copia os bytes de src para dest mesmo que haja sobreposição
// tem a cc_reserva que calcula a nova capacidade da string e realoca nas funções cat, catc que mudam o tamanho 

// copia n bytes de src para dest sem sobreposição
static void cc_copia_2(char *dest, const char *src, int n) {
  for (int i = 0; i < n; i++) {
      dest[i] = src[i];
  }
}

// as funções static são usadas como auxiliares dentro do próprio código
// elas não devem ser chamadas por usuários da biblioteca e não aparecem no arquivo de declaração das funções (.h)
// elas não fazem parte da interface pública da biblioteca

// copia n bytes de src para dest mesmo que haja sobreposição
static void cc_move(char *dest, const char *src, int n) {
  if (dest < src) {
      // copia normalmente
      for (int i = 0; i < n; i++) {
          dest[i] = src[i];
      }
  } else if (dest > src) {
      // copia de tras pra frente pra nao sobrepor
      for (int i = n - 1; i >= 0; i--) {
          dest[i] = src[i];
      }
  }
}



cc cc_copia(cc cad) {
  cc_ok(cad);
  cc novo;

  // tamanho da string original
  novo.tam = cad.tam;

  // calcula capacidade mínima: precisa ser > tam + 1 (para o '\0')
  int cap = MIN_ALLOC; // quantidade minima de alocação

  // enquanto for menor que o tamanho, dobra a capacidade
  // pq ela deve ser uma potência de 2
  while (cap <= novo.tam) {
      cap *= 2;
  }
  novo.cap = cap;

  // aloca memória
  novo.mem = malloc(novo.cap * sizeof(char));
  if (novo.mem == NULL) {
      // erro de alocação -> retorna string vazia não alterável
      return (cc){ .tam = 0, .cap = 0, .mem = "" };
  }

  // copia os caracteres da string original
  cc_copia_2(novo.mem, cad.mem, novo.tam);

  // coloca o terminador
  novo.mem[novo.tam] = '\0';

  return novo;
}


void cc_destroi(cc cad)
{
  cc_ok(cad);
  if (cad.cap > 0) free(cad.mem);
}

// retorna o tamanho da string cad
int cc_tam(cc cad)
{
  cc_ok(cad);
  return cad.tam;
}

// retorna um ponteiro para o primeiro caractere de cad
char *cc_mem(cc cad)
{
  cc_ok(cad);
  return cad.mem;
}

// retorna o caractere na posição pos de cad
// retorna '\0' se pos for inválido ou fora dos limites
char cc_ch(cc cad, int pos)
{
    cc_ok(cad);
    if (pos < 0) {
        pos = cad.tam + pos; // conta do final caso a posição seja negativa
    }

    if(pos < 0 || pos >= cad.tam) { // se a posição for inválida
        return '\0'; // fora dos limites
    }
    
    return cad.mem[pos];
}

// retorna uma cadeia que é a substring que inicia na posição pos
//   de cad e tem tam bytes
// o ponteiro e tamanho da substring devem ser limitados ao conteúdo de cad
// o cc retornado não é alterável, e só aponta para a memória de cad
// o cc retornado não pode ser usado se cad for alterada depois
//   desta chamada
// por exemplo: se cad contém "barco":
//   pos=2,  tam=3 sub="rco"
//   pos=2,  tam=4 sub="rco" (tam é ajustado para 3)
//   pos=-2, tam=1 sub="c"
//   pos=-6, tam=3 sub="ba" (pos é ajustado para 0, tam para 2)
//   pos=10, tam=5 sub="" (pos é ajustado para o final da string, tam para 0)
cc cc_sub(cc cad, int pos, int tam)
{
    cc_ok(cad);

    if (pos < 0) {
        pos = cad.tam + pos;   // conta do final se a posição for negativa
    }

    if (pos < 0) {
        // se ainda for negativo, começa do início
        // e ajusta o tamanho também
        tam += pos;   // pos é negativo, então reduz tam
        pos = 0;
    }

    if (tam < 0) {
        tam = 0;  // não pode ter substring negativa
    }

    if (pos >= cad.tam) {
        // posição depois do fim → string vazia
        return (cc){ .tam = 0, .cap = cad.cap, .mem = cad.mem + cad.tam };
    }

    if (pos + tam > cad.tam) {
        tam = cad.tam - pos;  // reduz até o final
    }

    return (cc){ .tam = tam, .cap = 0, .mem = cad.mem + pos };
}


// retorna a primeira posição em cad, não antes de pos, onde tem algum
// caractere pertencente a chs
int cc_busca_c(cc cad, int pos_ini, cc chs)
{
  cc_ok(cad);
  // se a posição for maior que o tamanho é impossível ahcar, então retorna -1
  if (pos_ini >= cad.tam) {
    return -1;
  }

  // se a posição for menor que zero, começa na posição zero
  if (pos_ini < 0) {
    pos_ini = 0;
  }

  // itera pra achar o carcatere igual
  for(int i = pos_ini; i < cad.tam; i++) {
    for(int j = 0; j < chs.tam; j++) {
      if (cad.mem[i] == chs.mem[j]) {
        return i;
      }
    }
  }
  return -1;
}


// retorna a primeira posição onde cad[i] NÃO pertence a chs
int cc_busca_nc(cc cad, int pos_ini, cc chs)
{
    cc_ok(cad);
    cc_ok(chs);

    if (pos_ini < 0)
        pos_ini = 0;

    for (int i = pos_ini; i < cad.tam; i++) {
        int j;
        for (j = 0; j < chs.tam; j++) {
            if (cad.mem[i] == chs.mem[j])
                break; // encontrou, sai do loop e i++
        }
        if (j == chs.tam)
            return i; // não pertence a chs
    }

    return -1; // não encontrou nenhum
}


// começa a iterar do final pq quer a última posição de chs que pertença a cad
// (primeira de trás pra frente)
int cc_busca_rc(cc cad, int pos_ini, cc chs)
{
  cc_ok(cad);
  cc_ok(chs);
  
  if (pos_ini >= cad.tam) {
    pos_ini = cad.tam - 1; // ajusta para o último caractere se a posiçã for maior que o tamanho de cad
  }

  // se a posição for negativa, diminui do tamanho pra contar do final
  if (pos_ini < 0) {
    pos_ini = cad.tam + pos_ini;
  }

  // se a posição ainda for menor que zero, não tem como achar
  if (pos_ini < 0) {
    return -1;
  }

  for(int i = pos_ini; i >= 0; i--) {
    for(int j = 0; j < chs.tam; j++) {
      if (cad.mem[i] == chs.mem[j]) {
        return i;
      }
    }
  }
  return -1;
}

// retorna a última posição em cad, não posterior a pos, onde tem
//   algum caractere não pertencente a chs
// retorna -1 se não encontrar
int cc_busca_rnc(cc cad, int pos, cc chs)
{
    cc_ok(cad);
    cc_ok(chs);

    // ajusta posição negativa contando do final
    if (pos < 0)
        pos = cad.tam + pos;

    // se a posição for maior que o tamanho da astring começa a contar do último caractere
    if (pos >= cad.tam)
        pos = cad.tam - 1;
    // se ainda for negativo, começa do início
    if (pos < 0)
        pos = 0;

    // percorre para trás
    for (int i = pos; i >= 0; i--) {
        int j;
        for (j = 0; j < chs.tam; j++) {
            if (cad.mem[i] == chs.mem[j])
                break; // pertence a chs
        }
        if (j == chs.tam)
            return i; // não pertence a chs
    }

    return -1; // não encontrou
}

// retorna a primeira posição em cad, não antes de pos, onde tem
//   a substring buscada
// retorna -1 se não encontrar
// se pos referenciar uma posição antes do primeiro caractere de cad, a busca
//   deve iniciar no primeiro caractere
int cc_busca_s(cc cad, int pos_ini, cc buscada)
{
  cc_ok(cad);
  cc_ok(buscada);
  
  // se a posição for maior que o tamanho é impossível ahcar, então retorna -1
  if (pos_ini >= cad.tam) {
    return -1;
  }

  // se a posição for menor que zero, começa na posição zero
  if (pos_ini < 0) {
    pos_ini = 0;
  }

  int j = 0; // j representa a posição na string buscada
  for(int i = pos_ini; i < cad.tam; i++) {
    if (cad.mem[i] != buscada.mem[j]) {
      j = 0;
      continue;
    }

    j++;
    if (j == buscada.tam) {
      return i - j + 1; // retorna a posição na string principal onde a sub inicia
    }
  }
  return -1;
}

// retorna true se cad e cadb forem iguais, falso caso sejam diferentes
// se não forem do mesmo tamanho, são diferentes
bool cc_igual(cc cad, cc outra_cad)
{
  cc_ok(cad);
  cc_ok(outra_cad);

  if(cad.tam != outra_cad.tam) {
    return false;
  }

  for(int i = 0; i < cad.tam; i++) {
    if (cad.mem[i] != outra_cad.mem[i]) {
      return false;
    }
  }

  return true;
}

void cc_imprime(cc cad)
{
  fwrite(cad.mem, 1, cad.tam, stdout);
}


// criei essa função reserva pra evitar repetição de código
// ela calcula a nova capacidade da string e realoca
static void cc_reserva(cc *pcad, int novotam) {
  cc_ok(*pcad);
  if (pcad->cap == 0) {
    return; // não aterável, não faz nada
  }

  // capacidade mínima necessária
  // a capacidad de memória deve ser sempre superior ao tamanho da string
  // e sempre deve ter espaço para o '\0' no final
  int mincap = novotam + 1;

  // se a capacidade atual for menor que 8 (a mínima), vira 8
  int novacap = pcad->cap;
  if (novacap < 8) {
    novacap = 8;
  }

  // enquanto for menor que o mínimo calculado, dobra
  // a capacidade sempre deve ser uma potência de 2
  while (novacap < mincap) {
    novacap *= 2;
  } 

  // também não pode ser menor que 1/3 do tamanho
  if (novacap < novotam/3) {
      while (novacap < novotam/3) {
        novacap *= 2;
      }
  }

  // se mudou usa realloc e realoca
  if (novacap != pcad->cap) {
      pcad->mem = realloc(pcad->mem, novacap);
      pcad->cap = novacap;
  }
}

// adiciona ao final da cadeia apontada por pcad o conteúdo da cadeia cadb
// não faz nada se a cadeia apontada por pcad não for alterável
// coloca um caractere '\0' na posição seguinte ao final da cadeia em *pcad
void cc_cat(cc *pcad, cc cadb) {
  cc_ok(*pcad);
  cc_ok(cadb);

  if (pcad->cap == 0) {
    return; // não alterável
  }

  int novotam = pcad->tam + cadb.tam; // calcula o novo tamanho
  cc_reserva(pcad, novotam); // chama a função auxiliar que calcula a nova capacidade

  // copia cadb pro final
  cc_copia_2(pcad->mem + pcad->tam, cadb.mem, cadb.tam);

  pcad->tam = novotam; // atualiza o tamanho
  pcad->mem[pcad->tam] = '\0'; // fecha a string com o caractere de fim
}

// adiciona ao final da cadeia apontada por pcad o conteúdo da cadeia cadb
// não faz nada se a cadeia apontada por pcad não for alterável
// coloca um caractere '\0' na posição seguinte ao final da cadeia em *pcad
void cc_catc(cc *pcad, char c) {
  cc_ok(*pcad);

  if (pcad->cap == 0) {
    return; // não alterável
  } 

  int novotam = pcad->tam + 1; // calcula o novo tamanho (apenas um caractere)
  cc_reserva(pcad, novotam); // chama a função auxiliar que calcula a nova capacidade

  pcad->mem[pcad->tam] = c; // coloca o caractere na última posição da string
  pcad->tam = novotam; // atualiza o tamanho
  pcad->mem[pcad->tam] = '\0'; // fecha
}

// insere a cadeia cadb na posição pos da cadeia apontada por pcad
// não faz nada se a cadeia apontada por pcad não for alterável
// coloca um caractere '\0' na posição seguinte ao final da cadeia em *pcad
// pos pode ser negativo, e se estiver fora da cadeia *pcad, deve ser interpretado
//   como se fosse 0 (se estiver antes do início) ou como tam (se após o final)
void cc_insere(cc *pcad, int pos, cc cadb) {
  cc_ok(*pcad);
  cc_ok(cadb);
  if (pcad->cap == 0) {
    return; // não alterável
  }

  // ajusta posição negativa, conta do final
  if (pos < 0) {
      pos = pcad->tam + pos;
  }

  if (pos < 0) { // se a posição ainda for negativa começa do início
    pos = 0;
  }

  if (pos > pcad->tam){ // se a posição for maior que o tamanho, insere no final
    pos = pcad->tam;
  } 

  int novotam = pcad->tam + cadb.tam; // calcula o novo tamanho
  cc_reserva(pcad, novotam); // chama a função auxiliar que calcula a nova capacidade

  // move a parte depois de pos para frente
  cc_move(pcad->mem + pos + cadb.tam, pcad->mem + pos, pcad->tam - pos);

  // copia cadb para a posição
  cc_copia_2(pcad->mem + pos, cadb.mem, cadb.tam);

  // atualiza tamanho e fecha
  pcad->tam = novotam;
  pcad->mem[pcad->tam] = '\0';
}

// remove a substring de tamanho tam iniciando em pos em *pcad
// não faz nada se a cadeia apontada por pcad não for alterável
// pos e tam devem ser interpretados como em cc_sub
void cc_remove(cc *pcad, int pos, int tam) {
  cc_ok(*pcad);
  if (pcad->cap == 0) {
    return; // não alterável
  }
  if (tam <= 0) {
    return; // não tem como remover tamanho negativo ou zero
  }

  // ajusta posição negativa pra contar do final
  if (pos < 0) {
      pos = pcad->tam + pos;
  }

  // // se a posição ainda for negativa, começa do início
  if (pos < 0) {
    pos = 0;
  }

  if (pos >= pcad->tam) {
    return; // posição depois do fim da string
  }

  // ajusta tamanho para não passar do final da string
  if (pos + tam > pcad->tam) { // se ultrapassar o final
      tam = pcad->tam - pos; // ajusta para remover até o final
  }

  // move parte depois da remoção para frente
  cc_move(pcad->mem + pos, pcad->mem + pos + tam, pcad->tam - (pos + tam));

  // atualiza tamanho
  pcad->tam -= tam;
  pcad->mem[pcad->tam] = '\0'; // fecha a string

  // reduz capacidade se necessário (mantendo mínimo de 8)
  int novacap = pcad->cap;
  while (novacap > 8 && novacap > pcad->tam * 3) {
      novacap /= 2;
  }

  // se a capacidade mudou, realoca
  if (novacap != pcad->cap) {
      pcad->mem = realloc(pcad->mem, novacap);
      pcad->cap = novacap;
  }
}

// completa o final da string em *pcad com caracteres c, de forma que ela passe
//   a ter tam caracteres
// não faz nada se a cadeia apontada por pcad não for alterável ou já possuir tam
//   caracteres ou mais
// coloca um caractere '\0' na posição seguinte ao final da cadeia em *pcad
void cc_preenche(cc *pcad, int tam, char c) {
  cc_ok(*pcad);
  if (pcad->cap == 0) {
    return; // não alterável
  }
  if (pcad->tam >= tam) {
    return; // já tem tamanho suficiente
  }

  // garante espaço para tam caracteres + \0 no final chamando a função que realoca nova capacidade
  cc_reserva(pcad, tam);

  // preenche com c até atingir o novo tamanho
  for (int i = pcad->tam; i < tam; i++) {
      pcad->mem[i] = c;
  }

  // passa o novo tamanho e fecha a string
  pcad->tam = tam;
  pcad->mem[pcad->tam] = '\0';
}

// substitui, na cadeia apontada por pcad (que deve ser alterável),
//   a substring que inicia em pos e tem tam bytes pela cadeia em cadb.
// caso pos esteja fora da cadeia pcad, insere caracteres iguais a preenche
//   no início ou no final de pcad, caso necessário, para que a posição 
//   seja válida
// exemplos, suponha que *pcad seja "abacaxi", preenche seja '%':
//   pos=5, tam=2, cadb="te", *pcad deve ser transformado em "abacate"
//   7,0,"." -> "abacaxi."
//   9,0,"." -> "abacaxi%%."
//   9,0,"" -> "abacaxi%%"
//   9,10,"." -> "abacaxi%%."
//   -3,0,"123" -> "abac123axi"
//   0,1,"123" -> "123bacaxi"
//   -9,1,"123" -> "123%abacaxi"
//   3,200,"" -> "ab"
void cc_subst(cc *pcad, int pos, int tam, cc cadb, char preenche) {
  cc_ok(*pcad);
  cc_ok(cadb);
  if (pcad->cap == 0) {
      return; // não alterável
  }
  if (tam < 0) {
      tam = 0; // não existe remoção negativa
  }

  if (pos < 0) {
      pos = pcad->tam + pos; // conta de trás pra frente
  }

  // sea posição for antes do início, coloca caracteres preenche no começo
  if (pos < 0) {
      int falta = -pos; // quantos preenche precisa no começo
      int novotam = pcad->tam + falta;
      cc_reserva(pcad, novotam);

      // mover caracteres atuais pra frente
      for (int i = pcad->tam - 1; i >= 0; i--) {
          pcad->mem[i + falta] = pcad->mem[i];
      }

      // preencher início com 'preenche'
      for (int i = 0; i < falta; i++) {
          pcad->mem[i] = preenche;
      }

      pcad->tam = novotam;
      pcad->mem[pcad->tam] = '\0';
      pos = 0; // reposiciona
  }

  // se a posição for depois do fim, coloca preenche no final
  if (pos > pcad->tam) {
      int falta = pos - pcad->tam;
      int novotam = pcad->tam + falta;
      cc_reserva(pcad, novotam);

      // preencher até alcançar pos
      for (int i = pcad->tam; i < pos; i++) {
          pcad->mem[i] = preenche;
      }

      pcad->tam = novotam;
      pcad->mem[pcad->tam] = '\0';
  }

  // ajusta tam para não passar do final
  if (pos + tam > pcad->tam) {
      tam = pcad->tam - pos;
  }

  // desloca tudo depois de pos+tam para trás
  for (int i = pos; i + tam < pcad->tam; i++) {
      pcad->mem[i] = pcad->mem[i + tam];
  }
  pcad->tam -= tam;

  int novotam = pcad->tam + cadb.tam;
  cc_reserva(pcad, novotam);

  // mover parte final pra frente pra abrir espaço
  for (int i = pcad->tam - 1; i >= pos; i--) {
      pcad->mem[i + cadb.tam] = pcad->mem[i];
  }

  for (int i = 0; i < cadb.tam; i++) {
      pcad->mem[pos + i] = cadb.mem[i];
  }

  // atualizar tamanho e fechar
  pcad->tam = novotam;
  pcad->mem[pcad->tam] = '\0';
}


// retorna uma cópia compatível com string C da string em cad, em uma nova
// memória alocada com malloc de um byte a mais que o tamanho da string.
// é responsabilidade de quem chama esta função liberar essa memória.
char *cc_strc(cc cad)
{
  cc_ok(cad);
  // aloca espaço suficiente: tamanho + 1 (pro '\0')
  char *s = malloc(cad.tam + 1);
  if (s == NULL) {
      return NULL; // falha de alocação
  }

  // copia os caracteres
  cc_copia_2(s, cad.mem, cad.tam);

  // fecha a string com o caractere de fim
  s[cad.tam] = '\0';

  return s;
}

// passa o conteúdo de um arquivo pra uma estrutura cc
// retorna uma cadeia vazia e com cap = 0 (nao alterável) se der erro
// passa o conteúdo de um arquivo pra uma estrutura cc
// retorna uma cadeia vazia e com cap = 0 (não alterável) se der erro
cc cc_le_arquivo(cc nome)
{
  cc_ok(nome);
  char *path = cc_strc(nome); // converte cc em string normal
  if (!path) {
      return (cc){0, 0, ""}; // se deu erro retorna uma string vazia e nao alterável pq cap = 0
  }

  FILE *f = fopen(path, "rb"); // abre arquivo em binário
  free(path);
  if (!f) {
      return (cc){0, 0, ""}; // se deu erro ao abrir o arquivo retorna string vazia e nao alterável
  }

  // capacidade inicial pequena
  int cap = 16;
  char *guarda = malloc(cap);
  if (!guarda) { 
    fclose(f); 
    return (cc){0, 0, ""}; 
  } // erro de alocação

  int tam = 0; // vai guardar quantos caracteres ja foram lidos
  int c;

  // lê o arquivo byte por byte
  while ((c = fgetc(f)) != EOF) { // equanto nao chegar no fim
      // se não houver espaço, dobramos a capacidade
      if (tam >= cap - 1) {
          cap *= 2;
          char *novo = realloc(guarda, cap); // realoca se precisar de mais espaço
          if (!novo) {
              free(guarda);
              fclose(f);
              return (cc){0, 0, ""};
          }
          guarda = novo;
      }
      guarda[tam++] = (char)c; // guarda o caractere lido
  }
  fclose(f);

  guarda[tam] = '\0'; // garante finalização

  cc resultado;
  resultado.tam = tam;
  resultado.cap = cap;
  resultado.mem = guarda;
  return resultado;
}


// passa o conteúdo de uma cadeia cc para um arquivo
// o arquivo é criado se nao existir
// se o arquivo ja existir, escreve nele

void cc_grava_arquivo(cc cad, cc nome)
{
  cc_ok(cad);
  cc_ok(nome);

  char *nome_arquivo = cc_strc(nome); // passa cc para string normal
  if (!nome_arquivo) {
    return;
  } // erro de alocação

  FILE *f = fopen(nome_arquivo, "wb"); // abre em binário
  free(nome_arquivo);
  if (!f) {
    return;
  } // erro ao abrir o arquivo

  if (cad.tam > 0) {
      fwrite(cad.mem, 1, cad.tam, f); // escreve bytes exatos
  }

  fclose(f);
}



