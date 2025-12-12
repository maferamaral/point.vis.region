---
trigger: always_on
description: Use these rules to understand the project requirements when implementing any functionality.
---

# Estrutura de Dados I - Trabalho II (2025)

[cite_start]**Alienígenas invadiram o planeta Terra!** [cite: 4] [cite_start]Eles lançam 3 tipos de bombas: destruição, pintura e clonagem. [cite: 4, 5] [cite_start]Os alunos do Depto de Física da UEL inventaram uma tinta que, quando usada em anteparos, bloqueia a propagação da explosão da bomba. [cite: 5] [cite_start]Uma das tarefas dos alunos de Estrutura de Dados é determinar a região afetada pela explosão de uma bomba. [cite: 6]

---

## 1. A Entrada

> [cite_start]**ATENÇÃO:** Não esqueça de ler a descrição geral dos projetos (Sala de Aula). [cite: 8]

[cite_start]A entrada do algoritmo será basicamente um conjunto de formas geométricas básicas (retângulos, círculos, etc.) dispostos numa região do plano cartesiano. [cite: 9]

* **Coordenadas e Âncoras:**
    * Considere a Ilustração 1 (no PDF original). [cite_start]Cada forma geométrica é definida por uma coordenada âncora e por suas dimensões. [cite: 10]
    * [cite_start]**Círculo:** A âncora é o seu centro e sua dimensão é definida pelo raio ($r$). [cite: 11]
    * [cite_start]**Retângulo:** A âncora é o seu canto inferior esquerdo (note que o plano cartesiano está "de ponta-cabeça" em relação à representação usual, estilo SVG) e suas dimensões são largura ($w$) e altura ($h$). [cite: 12, 30]
    * [cite_start]**Texto:** A âncora normalmente é o início do texto, mas pode ser definida como o meio ou o fim. [cite: 13]
    * [cite_start]**Linha:** Determinada por duas âncoras em suas extremidades. [cite: 14]
    * [cite_start]As coordenadas são valores reais e cada forma é identificada por um número inteiro. [cite: 15, 16]

### Formato dos Arquivos de Entrada (.geo e .qry)

[cite_start]Os parâmetros mais comuns nos comandos são: [cite: 25]
* [cite_start]`i`, `j`, `k`: número inteiro $\ge 1$ (Identificador da forma). [cite: 26]
* [cite_start]`r`: número real (Raio). [cite: 27]
* [cite_start]`x`, `y`: números reais (Coordenadas). [cite: 28]
* [cite_start]`cor`: string (Cor válida no padrão SVG). [cite: 29]

#### Comandos do Arquivo `.geo`

| Comando | Parâmetros | Descrição |
| :--- | :--- | :--- |
| **c** | `i x y r corb corp` | Cria um **círculo** com id `i`: `(x,y)` é o centro; `r` é o raio; [cite_start]`corb` é a cor da borda e `corp` é a cor do preenchimento. [cite: 33] |
| **r** | `i x y w h corb corp` | Cria um **retângulo** com id `i`: `(x,y)` é a âncora; `w` é a largura e `h` a altura. [cite_start]`corb` (borda) e `corp` (preenchimento). [cite: 33] |
| **l** | `i x1 y1 x2 y2 cor` | [cite_start]Cria uma **linha** com id `i`, extremidades em `(x1,y1)` e `(x2,y2)`, com a `cor` especificada. [cite: 33] |
| **t** | `i x y corb corp a txto` | Cria um **texto** `txto` com id `i` em `(x,y)`. `a` determina a âncora: `i` (início), `m` (meio), `f` (fim). [cite_start]`txto` é o último parâmetro e pode conter espaços. [cite: 33] |
| **ts** | `fFamily fWeight fSize` | Muda o **estilo dos textos** subsequentes. `fFamily`: sans, serif, cursive. [cite_start]`fWeight`: n (normal), b (bold), b+ (bolder), l (lighter). [cite: 33] |

#### Comandos do Arquivo `.qry`

[cite_start]O programa executará os comandos contidos no arquivo `.qry`. [cite: 34]

| Comando | Parâmetros | Descrição |
| :--- | :--- | :--- |
| **a** | `i j [v h]` | **Transformação em Anteparos:** As formas com ids na faixa `[i,j]` são pintadas com tinta bloqueante. Círculos viram segmentos horizontais (`h`) ou verticais (`v`). Segmentos devem ter ids únicos. Cor do segmento = cor da borda original. [cite_start]**TXT:** reportar id/tipo original e id/extremos dos segmentos. [cite: 35] |
| **d** | `x y sfx` | **Bomba de destruição** lançada em `(x,y)`. **TXT:** reportar id/tipo das formas destruídas. [cite_start]**SVG:** desenhar a região de visibilidade (sufixo `sfx` no arquivo ou no próprio svg final se `sfx` for "-"). [cite: 41] |
| **p** | `x y cor sfx` | **Bomba de pintura** em `(x,y)`. Formas na região de visibilidade têm cores (borda/preenchimento) alteradas para `cor`. **TXT:** reportar id/tipo das pintadas. [cite_start]**SVG:** semelhante ao comando `d`. [cite: 41] |
| **cln** | `x y dx dy sfx` | **Bomba de clonagem** em `(x,y)`. Clones são transladados em `dx, dy`. Clones devem ter ids únicos. **TXT:** id/tipo das originais e clones. [cite_start]**SVG:** semelhante ao comando `d`. [cite: 41] |

---

## 2. Transformação em Anteparos

[cite_start]Quando pintadas com tinta bloqueante, as figuras tornam-se anteparos (linhas) e a figura original deixa de existir (exceto linhas que já eram linhas). [cite: 42, 44]

* [cite_start]**Retângulos:** Substituídos por segmentos correspondentes aos seus lados. [cite: 45]
* [cite_start]**Círculos:** Substituídos por um segmento horizontal ou vertical (passando pelo centro) conforme o diâmetro. [cite: 46]
* **Textos:** O texto é tratado como um segmento $(x_1, y_1) - (x_2, y_2)$ dependendo da âncora ($a$) e do número de caracteres ($|t|$). [cite_start]O fator de escala por caractere é 10.0. [cite: 50, 51, 52]

**Cálculo do segmento de texto:**
Sendo $(x_t, y_t)$ a âncora e $|t|$ o tamanho do texto:
* Âncora **'i'** (início): $x_1 = x_t$, $y_1 = y_t$; [cite_start]$x_2 = x_t + 10.0 \times |t|$, $y_2 = y_t$. [cite: 56, 57]
* Âncora **'f'** (fim): $x_1 = x_t - 10.0 \times |t|$, $y_1 = y_t$; [cite_start]$x_2 = x_t$, $y_2 = y_t$. [cite: 58, 59]
* Âncora **'m'** (meio): $x_1 = x_t - 10.0 \times \frac{|t|}{2}$, $y_1 = y_t$; [cite_start]$x_2 = x_t + 10.0 \times \frac{|t|}{2}$, $y_2 = y_t$. [cite: 60]

---

## 3. Saída e Visibilidade

* [cite_start]**Saída Textual:** Arquivo texto contendo as informações solicitadas pelo `.qry`. [cite: 75]
* **Saída Gráfica (SVG):**
    1.  [cite_start]Ao final do processamento do `.geo`, gerar um `.svg` com todas as formas. [cite: 76]
    2.  [cite_start]Ao final do processamento do `.qry`, gerar outro `.svg` com as formas remanescentes e as anotações dos comandos. [cite: 77]
    * [cite_start]*Nota:* Instruções do `.qry` podem remover, modificar ou criar novas formas. [cite: 78]

### Região de Visibilidade e Sobreposição

A região de visibilidade é um polígono. [cite_start]Para testar se uma forma está dentro da região (ver Figura 3 do PDF), considera-se: [cite: 79, 80]

1.  [cite_start]**Segmento:** Se qualquer extremidade está dentro do polígono ou se intersecta qualquer aresta do polígono. [cite: 90]
2.  [cite_start]**Retângulo:** Se qualquer vértice do retângulo está no polígono; se qualquer vértice do polígono está no retângulo; ou se qualquer aresta do retângulo intersecta aresta do polígono. [cite: 92, 93, 94]
3.  [cite_start]**Círculo:** Se o centro está dentro; se existe aresta do polígono com distância ao centro $\le r$; ou se algum vértice do polígono está dentro do círculo. [cite: 96, 97, 98, 99]

---

## 4. Implementação

* [cite_start]**TADs:** Implementar os TADs postados na Sala de Aula. [cite: 102]
* **Modularização:** É **proibido** definir structs em arquivos `.h`. O `.h` deve ser bem documentado (é um contrato). [cite_start]Cada estrutura deve ter seu módulo `.c` e `.h` e um programa de teste unitário separado. [cite: 103, 104, 105, 106]
* [cite_start]**Estruturas de Dados:** Usar uma **árvore** para os segmentos ativos e **listas** para armazenar as formas. [cite: 107]
* [cite_start]**Ordenação:** O algoritmo deve estar preparado para usar o `qsort` (biblioteca padrão) e uma versão modificada do `mergesort` (implementada por você). [cite: 108]
    * [cite_start]Para subvetores "pequenos" (parâmetro `-i`), usar o `insertionsort`. [cite: 109]

---

## 5. Avaliação

A nota será proporcional aos testes bem sucedidos, com descontos aplicáveis. [cite_start]Fraude resulta em nota ZERO. [cite: 112, 113, 114]

| Critério de Desconto | Valor do Desconto |
| :--- | :--- |
| Definir struct em arquivo `.h` | [cite_start]2.5 [cite: 115] |
| [cite_start]Modularização pobre (`.h` mal projetado/documentado) | até 2.0 [cite: 115] |
| Não implementar estruturas pedidas | [cite_start]Gravíssimo [cite: 118] |
| [cite_start]Procedimentos extensos/complicados | até 1.0 [cite: 119] |
| [cite_start]Escolha/uso de estrutura pouco eficiente | até 2.0 [cite: 119] |
| [cite_start]Poucos commits ou concentrados no final | até 3.0 (pode indicar fraude) [cite: 119] |
| Implementação ineficiente/desidiosa | [cite_start]Até 1.5 [cite: 119] |
| Não usar o makefile provido | [cite_start]Sem desconto direto (salvo problemas graves decorrentes) [cite: 119] |
| Erro de compilação | [cite_start]Nota Zero (nenhum teste executado) [cite: 119] |

---

## 6. O Que Entregar

* [cite_start]Os arquivos devem estar em um repositório GIT. [cite: 122]
* [cite_start]Submeter no Classroom um arquivo `.txt` contendo uma linha no formato: `apelido url-do-repositorio`. [cite: 122, 124]
    * [cite_start]Exemplo: `joseMane https://github.com/zemane/t2.git` [cite: 126]

---

## 7. Parâmetros do Programa (`ted`)

[cite_start]O programa deve aceitar os seguintes parâmetros via linha de comando: [cite: 130]

| Parâmetro | Opcional | Descrição |
| :--- | :--- | :--- |
| `-e path` | Sim | [cite_start]Diretório-base de entrada (BED). [cite: 131] |
| `-f arg.geo` | Não | [cite_start]Arquivo de descrição da cidade (sob o BED). [cite: 131] |
| `-o path` | Não | [cite_start]Diretório-base de saída (BSD). [cite: 131] |
| `-q arqcons.qry` | Sim | [cite_start]Arquivo com consultas (sob o BED). [cite: 131] |
| `-to [gm]` | Sim | Tipo de ordenação: `q` (qsort) ou `m` (mergesort). [cite_start]Default: `q`. [cite: 131] |
| `-in` | Sim | Limite para insertionsort. [cite_start]Default: 10. [cite: 131] |

**Resumo dos Arquivos Produzidos:**
[cite_start]Dependendo dos parâmetros e comandos, o programa gera arquivos `.svg` e `.txt` com nomes compostos (ex: `arq-arqcons-sufx.svg`). [cite: 133]

> **ATENÇÃO:**
> * Os fontes devem ser compilados com a opção `-fstack-protector-all`.
> [cite_start]* Adotar padrão C99 (`-std=c99`). [cite: 135]