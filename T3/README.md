# 📌 Jogo de Digitação com Fila de Prioridade (Heap)

Este projeto implementa um **jogo de digitação de palavras** utilizando uma **fila de prioridade genérica em C**.

---

## 🧩 Dependências

Para compilar o projeto, é necessário incluir os seguintes módulos do trabalho anterior:

- `lstr`
- `str`
- `tela`

---

## ⚙️ Estrutura da Fila de Prioridade

A interface da fila de prioridade está definida no arquivo:

- `fila-prio.h`

Nesta estrutura, os elementos são inseridos em qualquer ordem, mas removidos sempre de acordo com a **prioridade definida pelo usuário**.

---

## 🧠 Fila Genérica (void*)

A fila de prioridade é **genérica**, podendo armazenar qualquer tipo de dado.

Para isso:

- Na criação da fila (`fp_cria`), deve-se informar:
  - O tamanho (em bytes) de cada elemento
  - Uma função de comparação entre elementos

- Na inserção (`fp_insere`):
  - É passado um ponteiro (`void *`) para o dado
  - A fila copia os bytes desse endereço para sua estrutura interna

- Na remoção (`fp_remover`):
  - É passado um ponteiro de destino
  - A fila copia o elemento removido para esse endereço

Como o tipo dos dados é desconhecido, utiliza-se `void *`, que representa um ponteiro genérico em C.

---

## ⚖️ Função de Comparação

Para definir a prioridade dos elementos, a fila recebe uma função comparadora definida pelo usuário.

Essa função deve:

- Receber dois ponteiros `void *`
- Retornar:
  - `> 0` → primeiro elemento tem maior prioridade
  - `< 0` → segundo elemento tem maior prioridade
  - `0` → ambos possuem a mesma prioridade

O tipo dessa função é definido como `comparador_t`.

Exemplo de uso:

```c
fp_cria(tamanho, &funcao_comparacao);
