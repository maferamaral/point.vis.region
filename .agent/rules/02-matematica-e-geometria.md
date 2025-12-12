---
trigger: always_on
---

# Especificações Matemáticas e Geométricas

## 1. Transformação em Anteparos (Segmentos)
Quando formas são atingidas pelo comando `a` (tinta bloqueante), elas viram segmentos de reta (exceto a própria linha, que vira ela mesma, mas bloqueante).

### Retângulos
[cite_start]Devem ser decompostos em 4 segmentos correspondentes aos seus lados[cite: 46].

### Círculos
São substituídos por **um** segmento (diametral) passando pelo centro.
* [cite_start]Pode ser horizontal (`h`) ou vertical (`v`) conforme parâmetro do comando[cite: 47].

### Textos
Textos são convertidos em segmentos baseados no comprimento da string `|t|`.
Fator de escala: `10.0` por caractere.
[cite_start]Comprimento total (`cl`): `10.0 * |t|`[cite: 61].

[cite_start]Cálculo das coordenadas `(x1, y1)` e `(x2, y2)` baseadas na âncora `(xt, yt)` e alinhamento [cite: 55-60]:
* **Início ('i'):**
    * $x1 = xt, y1 = yt$
    * $x2 = xt + cl, y2 = yt$
* **Fim ('f'):**
    * $x1 = xt - cl, y1 = yt$
    * $x2 = xt, y2 = yt$
* **Meio ('m'):**
    * $x1 = xt - cl/2, y1 = yt$
    * $x2 = xt + cl/2, y2 = yt$

## 2. Predicados Geométricos
Para o algoritmo de visibilidade, precisamos de funções robustas.

### Virada à Esquerda/Direita (Determinante)
Dada a sequência de pontos $x, y, z$. [cite_start]Calcular a área orientada $2 \cdot A(T)$[cite: 234]:
$$Det = (x.x \cdot y.y) + (x.y \cdot 1 \cdot z.x) + (1 \cdot y.x \cdot z.y) - (1 \cdot y.y \cdot z.x) - (x.x \cdot 1 \cdot z.y) - (x.y \cdot y.x \cdot 1)$$
* Se $Det > 0$: Virada à Esquerda.
* Se $Det < 0$: Virada à Direita.
* Se $Det == 0$: Colineares.

### Intersecção
Calcular o ponto exato de intersecção entre:
1.  Dois segmentos.
2.  Um raio (semirreta iniciando em `x` passando por `v`) e um segmento.

### Ponto no Polígono
[cite_start]Necessário implementar algoritmo para verificar se uma forma está dentro (ou intersecta) o polígono de visibilidade gerado[cite: 82, 103].
* **Bounding Box:** Primeiro teste rápido.
* **Teste detalhado:** Ray casting ou Winding number.