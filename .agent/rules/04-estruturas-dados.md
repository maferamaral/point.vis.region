---
trigger: always_on
---

# Especificação das Estruturas de Dados

## 1. Lista Encadeada Dupla (Genérica)
* **Interface:** `List`.
* **Funções:** `insert`, `remove`, `get`, `map`, `sort`.
* **Uso:** Armazenar as formas do arquivo `.geo`, armazenar os vértices ordenados do algoritmo de visibilidade, armazenar os pontos do polígono resultante.

## 2. Árvore Binária de Busca Balanceada (Específica para Segmentos)
* **Interface:** `SegTree` (ou similar).
* **Chave:** A ordem na árvore não é por um valor estático, mas pela posição relativa dos segmentos em relação ao raio de varredura.
* [cite_start]**Comparação na Árvore:** Dados dois segmentos $s_t$ e $s_r$, $s_r$ está à direita de $s_t$ na árvore se, geometricamente, $s_r$ estiver "atrás" ou "à direita" de $s_t$ na perspectiva do raio atual[cite: 244].
* **Operações:**
    * `insert(Segmento)`
    * `remove(Segmento)`
    * `getFirst()`: Retorna o segmento mais próximo de `x` (o biombo).
    * `getNext(Segmento)`: Retorna o segmento imediatamente "atrás" de um dado segmento.

## 3. Polígono (Módulo Sugerido)
* Deve armazenar uma sequência ordenada de pontos.
* Deve possuir função para calcular `BoundingBox` (min X, min Y, max X, max Y).
* Deve implementar a função `isInside(Ponto)` ou `intersecta(Forma)`.