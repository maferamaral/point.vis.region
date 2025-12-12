---
trigger: always_on
---

# Especificação Técnica: Transformação em Anteparos (Comando `a`)

## 1. Contexto e Objetivo
A transformação em anteparos é o mecanismo que converte formas geométricas decorativas (círculos, retângulos, textos) em **obstáculos físicos** (segmentos de reta) que bloqueiam a "luz" no Algoritmo de Visibilidade.

Esta operação é disparada exclusivamente pelo comando `a` (anteparo) no arquivo `.qry`.

## 2. O Comando Gatilho
* [cite_start]**Sintaxe:** `a i j [v|h]` [cite: 35]
    * `i`, `j`: IDs inicial e final (inclusive). Todas as formas neste intervalo devem ser processadas.
    * `[v|h]`: Parâmetro de orientação (obrigatório se houver círculos no intervalo).
        * `v`: Vertical.
        * `h`: Horizontal.

## 3. Lógica de Substituição
Ao processar o comando, o sistema deve:
1.  Localizar a forma original na estrutura de armazenamento (ex: Lista ou Hash).
2.  Remover (ou marcar como inativa) a forma original para fins de visualização futura (ela deixa de ser desenhada como forma cheia).
3.  Gerar 1 ou mais **Segmentos** baseados na geometria da forma.
4.  Inserir esses segmentos na lista de entrada para o Algoritmo de Visibilidade.

---

## 4. Fórmulas de Conversão por Tipo

### 4.1. Retângulos (`r`)
[cite_start]Um retângulo definido por âncora $(x, y)$, largura $w$ e altura $h$ deve ser decomposto em **4 segmentos** correspondentes aos seus lados[cite: 46]:

1.  **Base:** $(x, y) \to (x+w, y)$
2.  **Topo:** $(x, y+h) \to (x+w, y+h)$
3.  **Esquerda:** $(x, y) \to (x, y+h)$
4.  **Direita:** $(x+w, y) \to (x+w, y+h)$

### 4.2. Círculos (`c`)
Um círculo definido por centro $(x, y)$ e raio $r$ é convertido em **1 segmento** (diâmetro) passando pelo centro. [cite_start]A direção depende do parâmetro do comando `a` [cite: 47-50].

* **Se parâmetro for `h` (Horizontal):**
    * $P_{ini} = (x - r, y)$
    * $P_{fim} = (x + r, y)$
* **Se parâmetro for `v` (Vertical):**
    * $P_{ini} = (x, y - r)$
    * $P_{fim} = (x, y + r)$

> **Nota:** O parser deve garantir que a flag `v` ou `h` seja repassada corretamente para a função de transformação.

### 4.3. Textos (`t`)
Textos são convertidos em **1 segmento horizontal**. O comprimento é calculado baseando-se no número de caracteres da string.

* [cite_start]**Fator de Escala:** Cada caractere ocupa `10.0` unidades[cite: 61].
* **Comprimento Total (`cl`):** `cl = 10.0 * strlen(texto)`.

[cite_start]As coordenadas do segmento dependem da âncora do texto (`i`, `m`, `f`) definida no arquivo `.geo` [cite: 55-60]:

* **Início (`i`):** O texto cresce para a direita.
    * $x_1 = x_t$
    * $x_2 = x_t + cl$
    * $y_1 = y_2 = y_t$
* **Fim (`f`):** O texto termina na âncora (cresce para a esquerda).
    * $x_1 = x_t - cl$
    * $x_2 = x_t$
    * $y_1 = y_2 = y_t$
* **Meio (`m`):** O texto é centralizado na âncora.
    * $x_1 = x_t - (cl / 2.0)$
    * $x_2 = x_t + (cl / 2.0)$
    * $y_1 = y_2 = y_t$

### 4.4. Linhas (`l`)
[cite_start]Uma linha definida por $(x_1, y_1)$ e $(x_2, y_2)$ mantém sua geometria exata[cite: 45].
* Transformação: Apenas altera seu "status" para atuar como anteparo no algoritmo de visibilidade. Ela **não** é removida, mas passa a ser considerada um bloqueio.

---

## 5. Integração com Estruturas de Dados
* Os segmentos gerados devem ser encapsulados em uma struct `Segmento` que possua:
    * Ponto Inicial $(x, y)$.
    * Ponto Final $(x, y)$.
    * ID da forma original (para rastreamento nos relatórios).
* Esses segmentos alimentarão a **Árvore de Segmentos Ativos** (BST) durante a execução da varredura planar.