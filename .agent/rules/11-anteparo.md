---
trigger: always_on
---

# Especificação Visual: Anteparos (Segmentos Bloqueantes)

## 1. Conceito Visual
Quando uma forma é transformada em anteparo (comando `a`), ela perde sua representação original (círculo cheio, retângulo preenchido, texto) e passa a ser representada visualmente apenas como **segmentos de reta**.

## 2. Atributos de Estilo (SVG)

### Cor (`stroke`)
* A cor dos segmentos gerados deve ser **idêntica à cor da BORDA (`corb`)** da forma original.
* Exemplo: Um círculo com borda vermelha e fundo azul se torna um segmento vermelho.
* Fonte da regra: "Cor do segmento é o mesmo da cor da borda da figura original".

### Preenchimento (`fill`)
* Segmentos não possuem preenchimento. A propriedade `fill` da forma original é descartada.

### Espessura (`stroke-width`)
* O projeto não especifica uma espessura mandatória, mas recomenda-se manter um padrão visível (ex: `1px` ou `2px`) ou herdar a espessura padrão do visualizador.

## 3. Representação no SVG Final
No arquivo `nomebase-arqcons.svg` (Estado Final), as formas que foram transformadas em anteparos devem aparecer como tags `<line>`:

```xml
<line x1="10" y1="10" x2="50" y2="10" stroke="blue" /> <line x1="10" y1="40" x2="50" y2="40" stroke="blue" /> <line x1="10" y1="10" x2="10" y2="40" stroke="blue" /> <line x1="50" y1="10" x2="50" y2="40" stroke="blue" /> <line x1="100" y1="80" x2="100" y2="120" stroke="red" />