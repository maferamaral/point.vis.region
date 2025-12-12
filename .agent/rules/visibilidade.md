---
trigger: always_on
---

# Especificação Técnica: Algoritmo de Visibilidade por Varredura Angular

## 1. Visão Geral
[cite_start]O objetivo é determinar o polígono $V(x)$ visível a partir de um ponto de observação $x$, dado um conjunto de segmentos de barreira $S$ [cite: 22-23]. O algoritmo utiliza uma **Linha de Varredura (Sweep Line)** que gira 360º em torno de $x$, processando eventos (início e fim de segmentos) para manter o estado dos segmentos que interceptam a linha de visão atual.

## 2. Estruturas de Dados Principais

### 2.1. Lista de Eventos
Uma lista linear contendo todos os pontos extremos dos segmentos de barreira.
* [cite_start]**Conteúdo:** Cada evento possui um ângulo polar (em relação a $x$), um tipo (INÍCIO ou FIM de segmento) e um ponteiro para o segmento original[cite: 65].
* **Ordenação:** Os eventos devem ser processados em ordem angular crescente. [cite_start]Critérios de desempate são cruciais [cite: 453-457]:
    1.  Menor ângulo.
    2.  Menor distância ao centro (para processar oclusões próximas primeiro).
    3.  Tipo: Eventos de INÍCIO processados antes de FIM (se ângulos forem idênticos).

### 2.2. Árvore de Segmentos Ativos (SegsAtvs)
[cite_start]Uma **Árvore AVL** (Balanceada) que armazena os segmentos que estão, naquele exato momento, sendo interceptados pela linha de varredura[cite: 420].
* [cite_start]**Função:** Permitir inserção, remoção e busca do "segmento mais próximo" em tempo $O(\log n)$[cite: 421].
* **O "Pulo do Gato" (Comparação):** A árvore **não** ordena por distância numérica (pois a distância muda conforme a linha gira). Ela ordena por **Topologia Relativa**.
    * [cite_start]Regra: $S_r$ fica na sub-árvore direita de $S_t$ se $S_r$ estiver "à direita" (ou "atrás") de $S_t$ em relação ao ponto de observação $x$[cite: 421].
    * [cite_start]Isso é calculado usando **Predicados Geométricos** (Determinante/Área Orientada), verificando se um segmento oclui o outro [cite: 411-412].

### 2.3. O "Biombo" (Current Edge)
Uma variável que aponta para o segmento ativo mais próximo de $x$ no momento atual. [cite_start]Este segmento é o único que está efetivamente desenhando a borda da região de visibilidade[cite: 68, 349].

---

## 3. Pré-Processamento (Antes do Loop)

1.  **Criação de Barreiras:** Transforme todas as formas (círculos, retângulos, textos) em segmentos de reta puros.
2.  [cite_start]**Retângulo Envolvente:** Adicione 4 segmentos "infinitos" (borda do universo) para garantir que o raio sempre bata em algo[cite: 467].
3.  **Normalização Angular (Corte do Eixo 0):** Segmentos que cruzam o eixo positivo X (ângulo 0 para 360) devem ser divididos em dois segmentos distintos para evitar descontinuidade angular na ordenação.
4.  **Vértice Inicial Artificial ($v_0$):**
    * Trace um raio no ângulo 0.
    * Encontre o segmento mais próximo interceptado por este raio.
    * Insira este segmento na árvore de ativos antes de começar o loop. [cite_start]Este será o `biombo` inicial[cite: 274, 286].

---

## 4. O Ciclo de Varredura (O Loop)

Para cada evento na lista ordenada:

### Cenário A: O Evento é um Vértice de INÍCIO ($v$)
Um novo segmento ($S_v$) entra no campo de visão.

1.  [cite_start]**Inserção:** Insira $S_v$ na Árvore de Segmentos Ativos[cite: 77].
2.  **Verificação de Oclusão:**
    * Consulte a árvore para saber quem é o segmento mais próximo ($S_{min}$).
    * **Caso 1: $S_v$ está atrás do biombo atual.**
        * Nada visual acontece. [cite_start]O segmento é inserido na árvore, mas a luz não chega nele[cite: 76, 352].
    * **Caso 2: $S_v$ está na frente do biombo atual.**
        * O segmento $S_v$ passa a bloquear a visão do antigo biombo.
        * Calcule a intersecção do raio (ângulo de $v$) com o biombo antigo ($y$).
        * [cite_start]**Ação:** Adicione $y$ e $v$ ao Polígono de Visibilidade[cite: 305].
        * [cite_start]**Atualização:** O `biombo` passa a ser $S_v$ [cite: 96-97].

### Cenário B: O Evento é um Vértice de FIM ($v_f$)
Um segmento ($S_v$) sai do campo de visão.

1.  **Verificação de Visibilidade:**
    * **Caso 1: $S_v$ não é o biombo atual.**
        * Ele estava escondido atrás de outro. [cite_start]Apenas remova-o da árvore[cite: 118, 376].
    * **Caso 2: $S_v$ É o biombo atual.**
        * A parede acabou. A visão se abre para o que estava atrás dela.
        * Remova $S_v$ da árvore.
        * Consulte a árvore para encontrar o *novo* segmento mais próximo ($S_{prox}$) — aquele que estava escondido por $S_v$.
        * Calcule a intersecção do raio (ângulo de $v_f$) com $S_{prox}$ ($y$).
        * [cite_start]**Ação:** Adicione $v_f$ e $y$ ao Polígono de Visibilidade[cite: 311, 385].
        * [cite_start]**Atualização:** O `biombo` passa a ser $S_{prox}$[cite: 139].

---

## 5. Saída
O resultado é uma lista ordenada de vértices que formam um polígono simples (estrelado em relação a $x$). Este polígono pode ser desenhado diretamente no SVG.