# Regras de Transformação em Anteparos (Comando `a`)

Este documento define a lógica matemática e procedural para converter formas geométricas em segmentos de reta (anteparos) que bloqueiam a luz no algoritmo de visibilidade.

## 1. Definição Geral
[cite_start]O comando `a i j [v|h]` transforma todas as formas cujos identificadores estejam no intervalo `[i, j]` em anteparos[cite: 250].

* [cite_start]**Substituição:** A figura original deixa de existir (é removida da lista de formas vivas) e é substituída por um ou mais segmentos de reta na lista de segmentos ativos[cite: 260].
* [cite_start]**Exceção:** Se a forma original já for uma LINHA, ela não é removida, mas passa a atuar também como anteparo[cite: 260].
* [cite_start]**Atributos:** Os segmentos gerados herdam a **cor da borda** (`corb`) da figura original[cite: 250].
* [cite_start]**IDs:** Os segmentos produzidos devem ter identificadores únicos[cite: 250].

---

## 2. Regras por Tipo de Forma

### 2.1. Círculo (`c`)
O círculo é substituído por um único segmento de reta que corresponde ao seu diâmetro, passando pelo centro.
* **Entrada:** Centro $(x, y)$, Raio $r$.
* [cite_start]**Parâmetro do comando `a`:** Pode ser `v` (vertical) ou `h` (horizontal)[cite: 250].
* **Fórmulas de Transformação:**
    * **Caso Horizontal (`h`):**
        * Ponto Inicial: $(x - r, y)$
        * Ponto Final: $(x + r, y)$
        [cite_start]* [cite: 263]
    * **Caso Vertical (`v`):**
        * Ponto Inicial: $(x, y - r)$
        * Ponto Final: $(x, y + r)$
        [cite_start]* [cite: 264]

### 2.2. Retângulo (`r`)
O retângulo é decomposto nos 4 segmentos que formam suas arestas.
* **Entrada:** Âncora $(x, y)$, Largura $w$, Altura $h$.
* [cite_start]**Transformação:** Gera 4 segmentos[cite: 261].
* **Coordenadas dos Vértices:**
    Considerando $(x, y)$ como a âncora (canto inferior esquerdo conforme nota de rodapé 1, mas visualmente `y` cresce para baixo em SVG/Computação Gráfica, verifique a implementação do eixo Y):
    * $V_1 = (x, y)$
    * $V_2 = (x + w, y)$
    * $V_3 = (x + w, y + h)$
    * $V_4 = (x, y + h)$
* **Segmentos Gerados:**
    1.  $[V_1, V_2]$
    2.  $[V_2, V_3]$
    3.  $[V_3, V_4]$
    4.  $[V_4, V_1]$

### 2.3. Texto (`t`)
O texto é tratado como um único segmento de reta horizontal. O comprimento é calculado com base no número de caracteres.
* **Entrada:** Âncora $(xt, yt)$, Texto $txt$, Posição da Âncora (`i`, `m`, `f`).
* [cite_start]**Constante:** O comprimento por caractere é fixado em **10.0** unidades[cite: 271].
* **Fórmulas de Transformação:**
    Seja $|t|$ o número de caracteres da string (incluindo espaços).
    As coordenadas Y são constantes: $y_1 = y_2 = yt$.

    * **Âncora 'i' (início/start):**
        * $x_1 = xt$
        * $x_2 = xt + 10.0 \times |t|$
        [cite_start]* [cite: 270-271]

    * **Âncora 'f' (fim/end):**
        * $x_1 = xt - 10.0 \times |t|$
        * $x_2 = xt$
        [cite_start]* [cite: 272-273]

    * **Âncora 'm' (meio/middle):**
        * $x_1 = xt - 10.0 \times \frac{|t|}{2}$
        * $x_2 = xt + 10.0 \times \frac{|t|}{2}$
        [cite_start]* [cite: 275]

### 2.4. Linha (`l`)
* **Entrada:** $(x_1, y_1)$ e $(x_2, y_2)$.
* [cite_start]**Transformação:** A própria linha é inserida na lista de segmentos ativos/barreiras usando suas coordenadas originais. [cite: 260]

---

## 3. Saída de Relatório (TXT)
Ao processar o comando `a`, o programa deve escrever no arquivo `.txt` de saída:
1.  O **ID** e o **Tipo** da figura original que foi transformada.
2.  O **ID** e os **Extremos** $(x_1, y_1) \rightarrow (x_2, y_2)$ de cada segmento produzido.
[cite_start][cite: 250]