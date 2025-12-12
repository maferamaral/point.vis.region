---
trigger: always_on
---

# Formatos de Arquivos de Entrada (.geo e .qry)

Este documento especifica o formato dos arquivos de entrada que o sistema deve processar.

## 1. Arquivo de Geometria (`.geo`)
Este arquivo define o estado inicial do cenário. Cada linha contém um comando que define uma forma geométrica ou um estilo.

### [cite_start]Comandos Disponíveis [cite: 33]

#### Círculo (`c`)
* **Sintaxe:** `c i x y r corb corp`
* **Parâmetros:**
    * `i`: Inteiro (ID único da forma).
    * `x`, `y`: Reais (Coordenadas do centro).
    * `r`: Real (Raio).
    * `corb`: String (Cor da borda/traço).
    * `corp`: String (Cor do preenchimento).

#### Retângulo (`r`)
* **Sintaxe:** `r i x y w h corb corp`
* **Parâmetros:**
    * `i`: Inteiro (ID).
    * `x`, `y`: Reais (Coordenada âncora - canto inferior esquerdo).
    * `w`: Real (Largura/Width).
    * `h`: Real (Altura/Height).
    * `corb`, `corp`: Strings (Cores).

#### Linha (`l`)
* **Sintaxe:** `l i x1 y1 x2 y2 cor`
* **Parâmetros:**
    * `i`: Inteiro (ID).
    * `x1`, `y1`: Reais (Ponto inicial).
    * `x2`, `y2`: Reais (Ponto final).
    * `cor`: String (Cor da linha).

#### Texto (`t`)
* **Sintaxe:** `t i x y corb corp a txto`
* **Parâmetros:**
    * `i`: Inteiro (ID).
    * `x`, `y`: Reais (Coordenadas da âncora).
    * `corb`, `corp`: Strings (Cores).
    * `a`: Caractere (Âncora: `i`=início, `m`=meio, `f`=fim).
    * `txto`: String (Conteúdo do texto).
    * **Atenção:** O texto é o último parâmetro, pode conter espaços em branco e vai até o final da linha (`\n`). O parser deve ler tudo após o caractere de âncora.

#### Estilo de Texto (`ts`)
* **Sintaxe:** `ts fFamily fWeight fSize`
* **Parâmetros:**
    * `fFamily`: String (ex: `sans`, `serif`, `cursive`).
    * `fWeight`: String (ex: `n`, `b`, `b+`, `l`).
    * `fSize`: String/Número (Tamanho da fonte).
* **Comportamento:** Este comando altera o estado global de estilo de texto. Todos os comandos `t` lidos **após** este comando devem adotar este estilo até que um novo `ts` apareça.

---

## 2. Arquivo de Consultas (`.qry`)
Este arquivo contém comandos que modificam o cenário ou disparam o algoritmo de visibilidade (bombas).

### [cite_start]Comandos Disponíveis [cite: 35, 40]

#### Transformar em Anteparo (`a`)
* **Sintaxe:** `a i j [v|h]`
* **Parâmetros:**
    * `i`, `j`: Inteiros (Faixa de IDs, de `i` até `j`).
    * `[v|h]`: Caractere Opcional (Obrigatório apenas se houver círculos na faixa). Define se o círculo vira segmento vertical (`v`) ou horizontal (`h`).
* **Ação:** Todas as formas com ID entre `i` e `j` (inclusive) são transformadas em segmentos de reta "bloqueantes" (anteparos) e perdem sua forma original (exceto linhas que já são segmentos).

#### Bomba de Destruição (`d`)
* **Sintaxe:** `d x y sfx`
* **Parâmetros:**
    * `x`, `y`: Reais (Coordenadas da bomba/ponto de vista).
    * `sfx`: String (Sufixo para nomear arquivos de saída).
* **Ação:** Calcula visibilidade a partir de `(x,y)`.
    * Relatório TXT: Listar IDs e tipos das formas atingidas (visíveis).
    * Saída SVG: Desenhar o polígono de visibilidade. Se `sfx` for `"-"`, desenhar no SVG principal; caso contrário, criar novo arquivo.

#### Bomba de Pintura (`P`)
* **Sintaxe:** `P x y cor sfx`
* **Parâmetros:**
    * `x`, `y`: Reais (Coordenadas da bomba).
    * `cor`: String (Nova cor).
    * `sfx`: String (Sufixo).
* **Ação:** Semelhante à destruição, mas formas visíveis têm suas cores alteradas para `cor`. Relatar formas pintadas no TXT.

#### Bomba de Clonagem (`cln`)
* **Sintaxe:** `cln x y dx dy sfx`
* **Parâmetros:**
    * `x`, `y`: Reais (Coordenadas da bomba).
    * `dx`, `dy`: Reais (Vetor de deslocamento).
    * `sfx`: String (Sufixo).
* **Ação:** Formas visíveis são clonadas. Os clones são transladados por `(dx, dy)` e recebem novos IDs únicos. Relatar clones no TXT.

---

## 3. Notas Técnicas para o Parser (Leitura)

1.  **Tipagem:** IDs são sempre inteiros (`int`). Coordenadas e dimensões são reais (`double`). Cores e textos são strings (`char[]` ou `char*`).
2.  **Leitura de Texto:** Para o comando `t` do `.geo`, não use `fscanf` simples para o último parâmetro, pois ele para no primeiro espaço. Recomenda-se ler o restante da linha com `fgets` ou similar após ler a âncora.
3.  **Argumentos Opcionais:** O comando `a` no `.qry` tem um parâmetro que pode ou não existir dependendo do contexto, mas geralmente é fornecido se houver ambiguidade. O parser deve ser robusto para verificar se o caractere `v` ou `h` está presente.
4.  **Sufixo de Arquivos:** Quando o comando `.qry` pede sufixo `sfx`:
    * Nome do SVG gerado: `nomebase-consultas-sufixo.svg`.
    * Nome do TXT gerado: `nomebase-consultas.txt` (append dos relatórios).