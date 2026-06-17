# 🔤 Manipulação de Strings em C (Projeto em 3 Partes)

Este projeto implementa diferentes níveis de manipulação de strings em C, evoluindo desde strings constantes até strings com gerenciamento dinâmico de memória.

---

## 📌 Parte I — Strings Constantes

Na primeira etapa, são implementadas funções para manipulação de **strings constantes**.

### 📁 Arquivos envolvidos:
- `bb1.h` → contém a especificação das funções
- `bb1.c` → implementação parcial (deve ser completada pelo aluno)
- `bb1_tst.c` → programa de teste das funções

### 🎯 Objetivo:
Implementar funções básicas para tratamento de strings imutáveis.

---

## 📌 Parte II — Strings Alteráveis (vetor fixo)

Nesta etapa, as strings passam a poder ser **modificadas**, mas ainda com armazenamento em vetor.

### 📁 Arquivos envolvidos:
- `bb2.h` → definição das funções para strings alteráveis

### 🎯 Características:
- Strings podem ser modificadas
- Armazenadas em um vetor de caracteres
- Ainda sem alocação dinâmica

---

## 📌 Parte III — Strings com Memória Dinâmica

Na terceira etapa, as strings passam a gerenciar sua própria memória.

### 📁 Arquivos envolvidos:
- `cc.h` → especificação das funções

### 🎯 Características:
- Uso de **alocação dinâmica**
- Strings ajustam automaticamente o uso de memória conforme necessário
- Maior flexibilidade e eficiência no uso de memória

---

## 🚀 Evolução do Projeto

O projeto evolui em três níveis:

1. Strings constantes (imutáveis)
2. Strings alteráveis com vetor fixo
3. Strings dinâmicas com gerenciamento de memória

Essa progressão ajuda a entender conceitos importantes de:
- Manipulação de strings em C
- Uso de ponteiros
- Gerenciamento de memória dinâmica
