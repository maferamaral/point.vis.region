---
trigger: always_on
---

# Algoritmo de Região de Visibilidade (Varredura Planar)

## Objetivo
Calcular o polígono de visibilidade $V(x)$ a partir de um ponto de vista $x$ em um ambiente com segmentos obstrutores $S$.

## Estruturas Auxiliares
1.  **Lista de Eventos (Vértices):** Todos os extremos dos segmentos, ordenados angularmente em torno de $x$.
2.  **Árvore de Segmentos Ativos (BST):** Armazena segmentos que intersectam a linha de varredura atual. Deve ser balanceada.
3.  **Biombo Atual:** O segmento mais próximo de $x$ que está bloqueando a visão no momento.

## Pré-processamento
1.  [cite_start]**Retângulo Envolvente:** Adicionar 4 segmentos artificiais ao redor de todo o cenário para garantir que o raio sempre bata em algo[cite: 290].
2.  **Vértice Inicial ($v_0$):**
    * Traçar um raio horizontal a partir de $x$ (ângulo 0).
    * [cite_start]Segmentos interceptados por este raio devem ser **divididos** em dois ($S_i$ abaixo/esquerda, $S_f$ acima/direita) para evitar inconsistências na ordenação angular [cite: 301-305].
    * $v_0$ é definido pelo ponto de intersecção mais próximo neste raio inicial.

## Critério de Ordenação dos Vértices
[cite_start]Para ordenar a lista de eventos, comparar dois vértices $v_i$ e $v_j$ [cite: 276-280]:
1.  Por ângulo (pseudo-ângulo ou `atan2`).
2.  Se ângulos iguais: Distância até $x$ (menor distância primeiro, mas cuidado com início/fim).
3.  Se mesmo ângulo e distância: Vértice de **Início** processado antes de Vértice de **Fim**.

## O Loop de Varredura (Sweep Line)
Iterar sobre a lista ordenada de vértices `v`:

### Caso 1: `v` é INÍCIO de um segmento `s_v`
1.  Inserir `s_v` na Árvore de Segmentos Ativos.
2.  Verificar se `v` está encoberto pelo "Biombo Atual" (usando função `segAtivoMaisProx`).
3.  **Se NÃO encoberto (v está na frente):**
    * Houve mudança de visibilidade.
    * Calcular intersecção `y` do raio $(x, v)$ com o "Biombo Anterior".
    * Adicionar pontos ao Polígono de Visibilidade.
    * Atualizar "Biombo Atual" para `s_v`.

### Caso 2: `v` é FIM de um segmento `s_v`
1.  Verificar se `v` é o fim do "Biombo Atual".
2.  **Se SIM (o biombo acabou):**
    * Houve mudança de visibilidade.
    * Buscar na Árvore qual será o *novo* segmento mais próximo atrás de `s_v` (novo biombo).
    * Calcular intersecção `y` do raio $(x, v)$ com o *novo* biombo.
    * Adicionar pontos ao Polígono de Visibilidade.
    * Atualizar "Biombo Atual".
3.  Remover `s_v` da Árvore de Segmentos Ativos.