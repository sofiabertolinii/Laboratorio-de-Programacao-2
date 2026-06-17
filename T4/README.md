## 🧠 Descrição do Trabalho

Este trabalho consiste na implementação de estruturas de grafos em linguagem C, seguindo a interface definida no arquivo `grafo.h`. A implementação inclui também os principais algoritmos associados a grafos, como detecção de ciclos e ordenação topológica, conforme especificado na interface fornecida.

Além da estrutura de grafo, também foram implementadas estruturas auxiliares fundamentais, como listas e filas, de acordo com suas respectivas interfaces, sendo utilizadas como suporte para os algoritmos desenvolvidos.

O projeto é integrado a um sistema maior que simula o jogo do dinossauro, inspirado no jogo presente no navegador Google Chrome. Esse jogo pode ser executado de forma manual pelo usuário ou automaticamente por meio de uma rede neural.

A rede neural é implementada nos arquivos `rede.c` e `rede.h`, utilizando o grafo desenvolvido neste trabalho como estrutura base para suas operações. Também é fornecido um arquivo de exemplo de rede (`rede-exemplo`).

Além disso, o projeto inclui um programa de treinamento de redes neurais, que gera e evolui redes de forma iterativa. As redes são avaliadas com base no desempenho no jogo (executado sem interface gráfica), e as melhores são selecionadas para formar novas gerações. A melhor rede de cada geração é salva em arquivos no diretório `G/`.

O projeto inclui ainda um script de compilação (`compila`) responsável por gerar os executáveis dos programas `main` e `treina` utilizando o compilador GCC.

### Execução do programa

- `./main` → executa o jogo no modo manual (controle por teclado)
  - Controles: espaço ou seta para cima (pular), seta para baixo (agachar)
  - Tecla `q` encerra o programa quando o jogo estiver parado

- `./main rede-exemplo` → executa o jogo controlado por uma rede neural

- `./treina` → executa o processo de treinamento das redes neurais
