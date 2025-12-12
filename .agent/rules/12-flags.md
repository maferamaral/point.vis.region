---
trigger: always_on
---

# Especificação de Argumentos de Execução (CLI)

O programa deve processar os seguintes argumentos via linha de comando (`argc`, `argv`).

## 1. Argumentos Obrigatórios
O programa não pode rodar sem estes. Se faltarem, deve-se exibir erro e encerrar.

* **`-f arg.geo`**
    * **Descrição:** Nome do arquivo de descrição geométrica (o "mundo" inicial).
    * **Comportamento:** Este arquivo deve ser procurado dentro do diretório base de entrada (definido por `-e`). Se `-e` não for fornecido, assume-se o diretório atual ou caminho relativo.
    * **Exemplo:** `-f cidade.geo`

* **`-o path`**
    * **Descrição:** Diretório-base de saída (BSD - Base Output Directory).
    * **Comportamento:** Todos os arquivos gerados (`.svg`, `.txt`) devem ser salvos dentro deste diretório. O programa deve garantir que os arquivos sejam criados lá.
    * **Exemplo:** `-o ./saida`

## 2. Argumentos Opcionais
Estes modificam o comportamento ou adicionam funcionalidades extras.

* **`-e path`**
    * **Descrição:** Diretório-base de entrada (BED - Base Entry Directory).
    * **Comportamento:** Define a pasta raiz onde o programa deve procurar pelos arquivos passados em `-f` e `-q`.
    * **Exemplo:** `-e ./entradas`

* **`-q arqcons.qry`**
    * **Descrição:** Arquivo de consultas.
    * **Comportamento:** Contém os comandos de bombas (`d`, `P`, `cln`) e anteparos (`a`). Se fornecido, o programa processa as modificações e gera os relatórios. Deve estar sob o diretório definido em `-e`.
    * **Exemplo:** `-q ataques.qry`

* **`-to [q|m]`**
    * **Descrição:** Tipo de Ordenação (Sort Type).
    * **Opções:**
        * `q`: Utilizar **QuickSort** (pode ser o `qsort` da `stdlib`).
        * `m`: Utilizar **MergeSort** (implementação própria obrigatória).
    * **Default:** Se não informado, o padrão é `q`.
    * **Uso no Código:** Define qual algoritmo será usado para ordenar os vértices no Algoritmo de Visibilidade.

* **`-in valor`**
    * **Descrição:** Limiar para Insertion Sort.
    * **Comportamento:** Define o tamanho máximo de um subvetor para que o algoritmo de ordenação (especificamente o MergeSort customizado) alterne para **Insertion Sort** para ganhar performance em vetores pequenos.
    * **Default:** Se não informado, o padrão é `10`.
    * **Exemplo:** `-in 15` (Vetores com 15 elementos ou menos usam Insertion Sort).

---

## 3. Exemplo Completo de Execução
```bash
./ted -e input_folder -f caso1.geo -o output_folder -q consultas.qry -to m -in 12