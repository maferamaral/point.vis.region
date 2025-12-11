# Regras de Processamento de Consultas (.qry) e Saída

Este documento detalha o funcionamento dos comandos do arquivo `.qry`, a lógica do parâmetro `sfx` (sufixo) e a geração dos arquivos de saída.

## 1. Visão Geral
[cite_start]O arquivo `.qry` contém comandos que modificam o estado do "mundo" (formas geométricas) ou realizam consultas sobre ele[cite: 34].
* **Fluxo:** Os comandos são lidos e executados sequencialmente.
* [cite_start]**Interação:** As instruções podem remover formas, alterar atributos, criar novas formas ou transformar formas em anteparos (barreiras)[cite: 79].

## 2. Comando `a` (Transformação em Anteparos)
[cite_start]Transforma formas geométricas em segmentos de reta que bloqueiam a luz (barreiras) para os cálculos de visibilidade subsequentes[cite: 35, 45].

* [cite_start]**Sintaxe:** `a i j [v h]`[cite: 35].
    * `i`, `j`: Faixa de IDs das formas a serem transformadas.
    * `v` ou `h` (Opcional): Define se círculos viram segmentos Verticais ou Horizontais.
* **Comportamento:**
    1.  Identificar formas com ID entre `i` e `j` (inclusive).
    2.  [cite_start]**Transformação:** A forma original deixa de existir e é substituída por segmentos[cite: 45].
        * [cite_start]**Retângulo:** Substituído por 4 segmentos correspondentes aos seus lados[cite: 46].
        * **Círculo:** Substituído por 1 segmento (diâmetro) passando pelo centro. [cite_start]Se o parâmetro for `v` (vertical) ou `h` (horizontal)[cite: 47, 48, 49].
        * [cite_start]**Texto:** Substituído por 1 segmento linear calculado com base no número de caracteres e âncora (ver fórmulas específicas de deslocamento na pág. 4 do PDF)[cite: 51, 52].
        * [cite_start]**Linha:** A própria linha torna-se um anteparo[cite: 45].
    3.  [cite_start]**Cor:** Os segmentos herdam a cor da borda da figura original[cite: 35].
* [cite_start]**Relatório TXT:** Reportar ID e tipo da figura original, e os dados (ID e extremos) dos segmentos gerados[cite: 35].

---

## 3. Lógica Comum das Bombas (`d`, `p`, `cln`)
[cite_start]Todas as bombas requerem o cálculo da **Região de Visibilidade** a partir de um ponto `(x, y)`[cite: 40].

1.  [cite_start]**Cálculo da Visibilidade:** Executar o algoritmo de varredura (Sweep Line) usando todos os segmentos ativos (anteparos criados via comando `a` + bordas do universo) para gerar um **Polígono de Visibilidade**[cite: 81].
2.  **Teste de Inclusão:** Para cada forma viva na lista:
    * [cite_start]Verificar se ela está (parcialmente ou totalmente) dentro do Polígono de Visibilidade[cite: 80].
    * [cite_start]Critérios de sobreposição (Bounding Box, Vértices dentro, Intersecção de arestas) conforme Figura 3 do PDF [cite: 82, 94-101].

---

## 4. Comandos de Bombs Detalhados

### 4.1. Comando `d` (Destruição)
Remove formas atingidas pela "explosão".
* [cite_start]**Sintaxe:** `d x y sfx`[cite: 40].
* **Ação:**
    * Calcular visibilidade a partir de `(x, y)`.
    * [cite_start]Formas dentro da região são removidas da lista principal (destruídas)[cite: 40].
* **Relatório TXT:** Reportar ID e tipo das formas destruídas.

### 4.2. Comando `p` (Pintura)
Altera a cor das formas atingidas.
* [cite_start]**Sintaxe:** `p x y cor sfx`[cite: 40].
* **Ação:**
    * Calcular visibilidade a partir de `(x, y)`.
    * [cite_start]Formas dentro da região têm seus atributos de cor (borda e preenchimento) alterados para `cor`[cite: 40].
* **Relatório TXT:** Reportar ID e tipo das formas pintadas.

### 4.3. Comando `cln` (Clonagem)
Duplica e move formas atingidas.
* [cite_start]**Sintaxe:** `cln x y dx dy sfx`[cite: 40].
* **Ação:**
    * Calcular visibilidade a partir de `(x, y)`.
    * Para cada forma dentro da região:
        1.  Criar uma cópia da forma.
        2.  [cite_start]Transladar a cópia somando `dx` ao X e `dy` ao Y[cite: 40].
        3.  Atribuir um novo ID único ao clone.
        4.  Inserir o clone na lista principal.
* **Relatório TXT:** Reportar ID/tipo original e ID/tipo do clone.

---

## 5. O Parâmetro `sfx` e Geração de SVG

O parâmetro `sfx` controla como a **Região de Visibilidade** (o polígono de luz da bomba) é desenhada.

### 5.1. Regra do `sfx` (Sufixo)
Para os comandos `d`, `p`, `cln`:
* **Caso `sfx` seja uma string normal (ex: "passo1"):**
    * Deve-se criar um arquivo SVG separado contendo o estado atual das formas E o desenho da região de visibilidade (polígono).
    * [cite_start]Nome do arquivo: `nomegeo-nomeqry-sfx.svg`[cite: 135].
* **Caso `sfx` seja `"-"` (hífen):**
    * **NÃO** cria um arquivo separado imediato.
    * [cite_start]A região de visibilidade (polígono) deve ser desenhada/acumulada no arquivo SVG final da consulta (`nomegeo-nomeqry.svg`)[cite: 40].
    * *Nota de Implementação:* Isso implica que o programa deve manter uma lista de "polígonos de visibilidade para desenhar no final" ou desenhá-los em um buffer que será despejado no SVG final.

### 5.2. Nomes dos Arquivos de Saída
Assumindo entrada `-f cidade.geo` e `-q consultas.qry`:

1.  [cite_start]**SVG Inicial:** `cidade.svg` (após processar apenas o .geo)[cite: 77].
2.  [cite_start]**SVG Final da Consulta:** `cidade-consultas.svg`[cite: 78, 135].
    * Deve conter todas as formas remanescentes após a execução de todos os comandos.
    * Deve conter os desenhos das regiões de visibilidade das bombas que tiveram `sfx = "-"`.
3.  [cite_start]**SVGs Intermediários:** `cidade-consultas-sufixo.svg`[cite: 135].
    * Gerados apenas quando `sfx != "-"`.
4.  [cite_start]**Relatório Textual:** `cidade-consultas.txt`[cite: 135].
    * Contém os logs (reportar id, etc) concatenados de todos os comandos executados.

---

## 6. Resumo das Transformações Geométricas (Anteparos)
[cite_start]Fórmulas críticas para o comando `a` (Texto para Segmento)[cite: 55, 57, 60]:
Seja `(xt, yt)` a âncora e `|t|` o número de caracteres:
* **Âncora 'i' (início):** `x1 = xt`, `x2 = xt + 10.0 * |t|`.
* **Âncora 'f' (fim):** `x1 = xt - 10.0 * |t|`, `x2 = xt`.
* **Âncora 'm' (meio):** `x1 = xt - 10.0 * |t|/2`, `x2 = xt + 10.0 * |t|/2`.
* **Y:** `y1 = y2 = yt` (segmento sempre horizontal).