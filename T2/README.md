# 📝 Editor de Texto com Suporte a UTF-8

Este projeto consiste na implementação de um **editor de texto** em C com suporte a codificação **UTF-8**.

---

## 📦 Estrutura do Projeto

O sistema é composto pelos seguintes módulos:

### 🔤 Suporte a UTF-8
- Os arquivos `str` implementam o suporte à codificação **UTF-8**, permitindo manipulação correta de caracteres multibyte.

### 📚 Listas
- Os arquivos `lstr` implementam estruturas de lista:
  - Lista com alocação contígua
  - Lista encadeada

Essas estruturas são utilizadas para gerenciamento do conteúdo do editor.

### 🧑‍💻 Editor de Texto
- O editor foi implementado no arquivo:
  - `ed.c`

---

## ⚙️ Objetivo

O objetivo do trabalho é construir um editor de texto funcional, capaz de manipular caracteres UTF-8 corretamente e utilizar estruturas de lista para gerenciamento eficiente do conteúdo.

---

## 🧩 Dependências

Para compilar o projeto, é necessário incluir os módulos:

- `str` (suporte a UTF-8)
- `lstr` (implementação de listas)

---
