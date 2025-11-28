#include <stdio.h>
#include <string.h>
#include "bb2.h"   // supondo que a struct e funções estejam declaradas aqui
#include "bb2.c"
// Função auxiliar: imprime o estado da cadeia
void mostra(const char *msg, bb2 cad) {
    printf("%s | tam=%d cap=%d mem=\"%s\"\n", msg, cad.tam, cad.cap, cad.mem);
}

int main(void) {
    // Caso 1: vetor com espaço suficiente
    char buffer1[20] = "Ola";
    bb2 cad1 = { buffer1, (int)strlen(buffer1), sizeof(buffer1) };

    mostra("Inicial cad1", cad1);

    // Teste do catc
    bb2_catc(&cad1, '!');
    mostra("Depois de catc '!'", cad1);

    // Teste do preenche
    bb2_preenche(&cad1, 10, '.');
    mostra("Depois de preenche tam=10 '.'", cad1);

    printf("\n");

    // Caso 2: vetor cheio (sem espaço pra novos caracteres)
    char buffer2[5] = "ABCD"; // já ocupa 4 + '\0' = cheio
    bb2 cad2 = { buffer2, (int)strlen(buffer2), sizeof(buffer2) };

    mostra("Inicial cad2", cad2);

    // Teste do catc (não deve alterar)
    bb2_catc(&cad2, 'Z');
    mostra("Tentativa de catc 'Z' (sem espaço)", cad2);

    // Teste do preenche (também não deve alterar)
    bb2_preenche(&cad2, 10, 'X');
    mostra("Tentativa de preenche tam=10 'X' (sem espaço)", cad2);

    printf("\n");

    // Caso 3: vetor vazio (capacidade zero)
    bb2 cad3 = { NULL, 0, 0 };
    mostra("Inicial cad3 (cap=0)", cad3);

    // Nenhuma função deve alterar
    bb2_catc(&cad3, 'A');
    bb2_preenche(&cad3, 5, 'B');
    mostra("Depois de catc/preenche (sem efeito)", cad3);

    printf("\n");

    // Caso 4: preenche com tam menor que o atual (não deve alterar)
    char buffer4[15] = "Teste123";
    bb2 cad4 = { buffer4, (int)strlen(buffer4), sizeof(buffer4) };

    mostra("Inicial cad4", cad4);
    bb2_preenche(&cad4, 4, '*'); // tam=4 < tam atual (8), não altera
    mostra("Tentativa de preenche tam=4 '*'", cad4);

    return 0;
}

