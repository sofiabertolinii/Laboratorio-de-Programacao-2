
#include <stdio.h>
#include <stdbool.h>

// Implemente strings melhores em C.

// Enquanto não vem uma descrição mais completa, implemente uma função que realiza a substituição de uma substring em uma string por outra string. 
// A função deve ter a assinatura abaixo:

//    bool strsubst(int n, char s[n], int pos, int tam, char *outra);
// O vetor s, de tamanho n contém uma string (terminada por '\0'). A função deve substituir a substring de tamanho tam que inicia pelo caractere 
// que está na posição pos de s pela string que inicia em outra. Se a substituição for bem sucedida, a função deve retornar true. 
// Caso contrário, deve retornar false. Motivos para isso:

// s não sem tamanho suficiente para conter o resultado
// pos/tam não constituem uma substring de s
// A string em s pode ser vazia. A substring pode ser vazia. A string em outra pode ser vazia.

// Suponha que s tenha tamanho 12, e conteúdo "abacaxi".

// A chamada strsubst(12, s, 5, 2, "te") deve substituir o valor em s por "abacate".
// A chamada strsubst(12, s, 7, 0, ".") deve substituir o valor em s por "abacaxi.".
// A chamada strsubst(12, s, 3, 4, "") deve substituir o valor em s por "aba".
// A chamada strsubst(12, s, 0, 0, "Um ") deve substituir o valor em s por "Um abacaxi".
// A chamada strsubst(12, s, 0, 0, "Umas ") deve não alterar o valor em s e retornar false.

// se a posição for 0 empurra pra tras

// PARÂMETROS
// n é o tamanho da string original
// s é a string original (onde a substituição deve ser feita)
// pos é a posição onde a substituição começa
// *outra é a string que deve ser colocada na substituição
// tam representa a quantidade de caracteres que devem ser removidos da string original

int calcula_tamanho(char s[])
{
    int tam = 0;
    for(int i = 0; s[i] != '\0'; i++) {
        tam++;
    }
    return tam;
}

bool strsubst(int n, char s[n], int pos, int tam, char *outra) {
    int tam_s = calcula_tamanho(s);
    int tam_outra = calcula_tamanho(outra);

    // validações
    if (pos > tam_s || (pos + tam) > tam_s) {
        return false;
    }

    int novo_tamanho = tam_s - tam + tam_outra;
    if (novo_tamanho > n - 1) { // precisa caber o '\0'
        return false;
    }

    // Mover o restante da string para abrir/fechar espaço
    if (tam_outra != tam) {
        for (int i = tam_s; i >= pos + tam; i--) {
            s[i + (tam_outra - tam)] = s[i];
        }
    }

    // Copiar 'outra' para a posição desejada
    for (int i = 0; i < tam_outra; i++) {
        s[pos + i] = outra[i];
    }

    return true;
}

int main() {
    char s[12] = "abacaxi";
    if (strsubst(12, s, 5, 2, "te")) { // abacate
        printf("Deu certo. Resultado: %s\n", s);
    } else {
        printf("Erro");
    }
    
    char s2[12] = "abacaxi";
    if (strsubst(12, s2, 7, 0, ".")) { // abacaxi.
        printf("Deu certo. Resultado: %s\n", s2);
    } else {
        printf("Erro");
    }

    char s3[12] = "abacaxi";
    if (strsubst(12, s3, 3, 4, "")) {
        printf("Deu certo. Resultado: %s\n", s3);
    } else {
        printf("Erro");
    }

    char s4[12] = "abacaxi";
    if (strsubst(12, s4, 0, 0, "Um ")) { // Um abacaxi
        printf("Deu certo. Resultado: %s\n", s4);
    } else {
        printf("Deu erro");
    }

    char s5[12] = "abacaxi";
    if (strsubst(12, s, 0, 0, "Umas ")) { // Erro, há mais caracteres sendo utilizados do que posições
        printf("Deu certo. Resultado: %s\n", s5);
    } else {
        printf("Deu erro");
    }
    return 0;
}
