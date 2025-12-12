---
trigger: always_on
---

# Regras Gerais e Arquitetura do Projeto (TADs)

## Visão Geral
Este é um projeto de Estrutura de Dados em C (C99) que simula o cálculo de visibilidade e destruição de formas geométricas em um plano 2D. O sistema lê arquivos `.geo` (definição de cenário) e `.qry` (consultas/bombas) e gera saídas em `.svg` e `.txt`.

## Regras Críticas de Implementação (Lei Suprema)
1.  **Ponteiros Opacos (Opaque Pointers):**
    * É **terminantemente proibido** definir `structs` dentro de arquivos `.h`.
    * O arquivo `.h` deve conter apenas `typedef`, protótipos de funções e documentação detalhada.
    * A definição concreta da `struct` deve ficar **apenas** no arquivo `.c`.
    * Exemplo no `.h`: `typedef void* Ponto;` ou `typedef struct Ponto* Ponto;`
2.  **Modularização:**
    * Cada Estrutura de Dados deve ter seu próprio par de arquivos `.h` e `.c`.
3.  **Documentação:**
    * Arquivos `.h`: Devem ser **muito bem documentados** (explicar parâmetros, retorno, pré-condições). Este é o contrato público.
    * Arquivos `.c`: Comentários mínimos, apenas se a lógica for obscura.
4.  **Compilação:**
    * Flags obrigatórias: `-std=c99 -fstack-protector-all`.
    * Deve-se usar um `Makefile`.

## Entidades Principais
O projeto lida com formas que podem ser:
1.  **Círculos (`c`):** Raio e centro.
2.  **Retângulos (`r`):** Âncora (canto inf. esquerdo), largura, altura.
3.  **Linhas (`l`):** Ponto 1 e Ponto 2.
4.  **Textos (`t`):** Âncora, conteúdo, alinhamento (i, m, f).

## Fluxo de Execução
1.  Ler `.geo`: Instanciar formas e armazená-las em uma Lista Genérica.
2.  Gerar SVG inicial.
3.  Ler `.qry`: Processar comandos.
    * Comando `a`: Transforma formas em **Anteparos** (segmentos de reta bloqueantes).
    * Comandos `d` (destruição), `P` (pintura), `cln` (clonagem): Disparam o algoritmo de visibilidade a partir de um ponto `(x,y)`.
    * Formas (e anteparos) dentro do polígono de visibilidade sofrem a ação.

## Requisitos de Teste
Cada módulo deve ter um `main` de teste unitário isolado (ex: `teste_lista.c`, `teste_arvore.c`).