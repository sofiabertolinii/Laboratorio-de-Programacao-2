#include "fila-prio.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

// Função de comparação para inteiros (retorna 1 se a > b, -1 se a < b, 0 se a == b)
int compara_int(void *a, void *b) {
    int *x = (int *)a;
    int *y = (int *)b;
    if (*x > *y) return 1;
    if (*x < *y) return -1;
    return 0;
}

int aleat_entre_1_e_10() {
    // inicializa o gerador de números aleatórios apenas uma vez
    static int iniciado = 0;
    if (!iniciado) {
        srand(time(NULL)); // usa o relógio como semente
        iniciado = 1;
    }

    // gera número entre 1 e 10 (inclusive)
    return 1 + rand() % 10;
}

void zera_tempo(struct timespec *inicio, struct timespec *fim) {
    (*inicio).tv_sec = 0;
    (*fim).tv_sec = 0;
}

double calcula_tempo(struct timespec inicio, struct timespec fim) {
    double tempo_decorrido = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;
    return tempo_decorrido;
}

int *cria_vetor() {
    
}

int main() {
    struct timespec inicio, fim;
    
    // CRIANDO O VETOR
    int tamanho = 100;
    int valores[tamanho];
    for(int i = 0; i < 100; i++) {
        valores[i] = aleat_entre_1_e_10();
    }

    // CRIAÇÃO DA LISTA
    clock_gettime(CLOCK_MONOTONIC, &inicio);
    FilaPrio fila = fp_cria(sizeof(int), compara_int);
    if (fila == NULL) {
        printf("Falha ao criar a fila!\n");
        return 1;
    }
    clock_gettime(CLOCK_MONOTONIC, &fim);
    double tempo = calcula_tempo(inicio, fim);
    printf("Tempo para criar a lista: %.6f\n", tempo);

    zera_tempo(&inicio, &fim);

    // INSERIR VALORES
    clock_gettime(CLOCK_MONOTONIC, &inicio);
    for (int i = 0; i < 100; i++) {
        fp_insere(fila, &valores[i]);
    }
    clock_gettime(CLOCK_MONOTONIC, &fim);
    tempo = calcula_tempo(inicio, fim);
    printf("Tempo para inserir 100 elementos: %.6f\n", tempo);

    zera_tempo(&inicio, &fim);

    // REMOVER OS ELEMENTOS
    int removido;
    clock_gettime(CLOCK_MONOTONIC, &inicio);
    while (fp_tam(fila) > 0) {
        fp_remove(fila, &removido);
    }
    clock_gettime(CLOCK_MONOTONIC, &fim);
    tempo = calcula_tempo(inicio, fim);
    printf("Tempo para remover os 100 elementos: %.6f\n", tempo);

    // Destruir a fila
    fp_destroi(fila);

    return 0;
}