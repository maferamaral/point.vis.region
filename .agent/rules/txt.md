---
trigger: always_on
---

# Regras de Formatação do Relatório Textual (.txt)

Este documento define o padrão exato de formatação para o arquivo de saída `nomegeo-nomeqry.txt`. O arquivo deve registrar a execução dos comandos e seus efeitos sobre as formas.

## 1. Diretrizes Gerais
* **Arquivo de Saída:** O conteúdo deve ser acumulado e salvo no arquivo `nomegeo-nomeqry.txt` ao final da execução.
* **Separadores:**
    * O cabeçalho de cada comando começa com `[*]`.
    * As linhas de dados (formas afetadas) devem iniciar com uma tabulação (`\t`).
* **Tipos de Formas:** Ao imprimir o "tipo", utilize os códigos originais: `c` (círculo), `r` (retângulo), `l` (linha), `t` (texto).

---

## 2. Formatação por Comando

### 2.1. Comando `a` (Anteparos)
Registra a transformação de uma forma original em um ou mais segmentos de reta (anteparos).
* **Cabeçalho:** `[*] a`
* **Corpo (para cada segmento gerado):**
    ```text
    \t<id_original> (<tipo_original>) -> <id_novo_anteparo> (anteparo)
    ```
    [cite_start]*Nota: Se o PDF exigir coordenadas dos extremos[cite: 566], adicione-as após "(anteparo)", mas mantenha a estrutura base acima.*

### 2.2. Comando `cln` (Clonagem)
Registra a bomba de clonagem e as novas formas criadas.
* **Cabeçalho:** `[*] cln x=<val_x> y=<val_y> dx=<val_dx> dy=<val_dy>`
* **Corpo (para cada clone criado):**
    ```text
    \t<id_novo> <tipo_novo> (clone do <id_original> <tipo_original>)
    ```

### 2.3. Comando `d` (Destruição)
Registra a bomba de destruição e as formas removidas.
* **Cabeçalho:** `[*] d x=<val_x> y=<val_y>`
* **Corpo (para cada forma destruída):**
    ```text
    \t<id_destruido> <tipo_destruido>
    ```

### 2.4. Comando `p` (Pintura)
Registra a bomba de pintura e as formas que tiveram a cor alterada.
* **Cabeçalho:** `[*] p x=<val_x> y=<val_y> <cor>`
* **Corpo (para cada forma pintada):**
    ```text
    \t<id_pintado> <tipo_pintado>
    ```

---

## 3. Exemplo Prático de Conteúdo do Arquivo

```text
[*] a
    10 (c) -> 501 (anteparo)
    12 (r) -> 502 (anteparo)
    12 (r) -> 503 (anteparo)
    12 (r) -> 504 (anteparo)
    12 (r) -> 505 (anteparo)
[*] d x=100.5 y=200.0
    5 r
    8 c
[*] p x=50.0 y=50.0 blue
    3 t
    9 l
[*] cln x=10.0 y=10.0 dx=5.0 dy=5.0
    601 r (clone do 15 r)
    602 c (clone do 18 c)