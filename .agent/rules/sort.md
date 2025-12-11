

### Arquivo: `08_implementacao_sort.md`

````markdown
# Implementação do Módulo de Ordenação (sort.c / sort.h)

Este documento instrui a criação do módulo responsável por ordenar os vetores de eventos (vértices) e segmentos no projeto. A implementação deve ser genérica e atender aos requisitos de desempenho e flags especificadas.

## 1. Requisitos do Projeto
* **Algoritmos Obrigatórios:**
    1.  [cite_start]**MergeSort:** Implementação própria exigida[cite: 110].
    2.  **InsertionSort:** Implementação própria exigida. [cite_start]Deve ser usado quando o subvetor for pequeno[cite: 111].
    3.  [cite_start]**QSort:** Deve-se saber invocar o `qsort` da biblioteca padrão (`stdlib.h`)[cite: 110].
* **Polimorfismo:** As funções devem trabalhar com `void*` e receber ponteiros de função para comparação, permitindo ordenar qualquer tipo de dado (eventos, inteiros, etc.).
* [cite_start]**Hibridismo:** O `MergeSort` deve verificar o tamanho do subvetor e trocar para `InsertionSort` quando o tamanho for menor que o limiar definido (`-in`)[cite: 111].

## 2. Contrato (`sort.h`)
O arquivo de cabeçalho deve ser limpo e expor apenas a interface pública unificada.

```c
#ifndef SORT_H
#define SORT_H

#include <stddef.h> // para size_t

// Definição do tipo ponteiro de função para comparação.
// Deve retornar:
// < 0 se a < b
//   0 se a == b
// > 0 se a > b
typedef int (*Comparator)(const void* a, const void* b);

/**
 * Interface unificada para ordenação.
 * * @param base Ponteiro para o início do vetor.
 * @param nmemb Número de elementos.
 * @param size Tamanho de cada elemento em bytes.
 * @param cmp Função de comparação.
 * [cite_start]@param type Tipo de algoritmo: 'q' (QSort) ou 'm' (MergeSort)[cite: 133].
 * [cite_start]@param insertion_threshold Limite para ativar InsertionSort (usado apenas no Merge)[cite: 133].
 */
void sort(void* base, size_t nmemb, size_t size, Comparator cmp, char type, int insertion_threshold);

// Funções auxiliares (podem ser expostas para testes unitários)
void insertionSort(void* base, size_t nmemb, size_t size, Comparator cmp);
void mergeSort(void* base, size_t nmemb, size_t size, Comparator cmp, int threshold);

#endif
````

## 3\. Detalhes de Implementação (`sort.c`)

### 3.1. Aritmética de Ponteiros `void*`

Como `void*` não possui tamanho aritmético definido em C padrão, é necessário fazer *cast* para `char*` (que tem tamanho de 1 byte) para calcular endereços de memória.

  * **Acessar elemento `i`:** `(char*)base + (i * size)`

### 3.2. Insertion Sort

  * **Objetivo:** Ordenar vetores pequenos de forma eficiente e estável.
  * **Lógica:**
      * Percorrer o vetor do segundo elemento até o fim.
      * Para cada elemento, "voltar" comparando com os anteriores e trocando (ou deslocando) até achar a posição correta.
      * **Memória:** Utilize um buffer temporário (`malloc` de `size` bytes) para armazenar o elemento pivô durante as comparações.

### 3.3. Merge Sort Híbrido

  * **Lógica Recursiva:**
    1.  Verificar tamanho `n` do subvetor.
    2.  [cite_start]**Otimização:** Se `n <= threshold` (valor da flag `-in`), chame `insertionSort` e retorne[cite: 111].
    3.  Caso contrário (`n > threshold`):
          * Dividir o vetor ao meio.
          * Chamar `mergeSort` para a metade esquerda.
          * Chamar `mergeSort` para a metade direita.
          * Executar o `merge` (intercalação) dos dois subvetores ordenados.
  * **Gerenciamento de Memória:** O Merge Sort exige um vetor auxiliar. Para eficiência, aloque este vetor auxiliar apenas uma vez na função wrapper e passe-o como parâmetro para as funções recursivas, evitando múltiplos `malloc/free`.

### 3.4. Wrapper `qsort`

  * Simplesmente repasse os parâmetros `base`, `nmemb`, `size` e `cmp` para a função `qsort` da `<stdlib.h>`.

## 4\. Integração no `main.c` (Flags e Uso)

O programa deve selecionar o algoritmo baseado nos argumentos de linha de comando.

### 4.1. [cite_start]Flags Obrigatórias [cite: 133]

  * `-to [q|m]`: Tipo de ordenação.
      * `q`: QuickSort (Padrão).
      * `m`: MergeSort.
  * `-in`: Limiar para Insertion Sort.
      * Default: **10** (se a flag não for fornecida).

### 4.2. Exemplo de Parsing e Chamada

```c
// Valores padrão
char sort_type = 'q';      // Default conforme PDF
int insertion_limit = 10;  // Default conforme PDF

// Parsing simplificado de argv
for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-to") == 0 && i+1 < argc) {
        sort_type = argv[i+1][0]; // 'q' ou 'm'
    }
    else if (strcmp(argv[i], "-in") == 0 && i+1 < argc) {
        insertion_limit = atoi(argv[i+1]);
    }
    // ... outros argumentos (-f, -o, -e, etc)
}

// Momento de ordenar os eventos no Algoritmo de Visibilidade
// vetor_eventos: array de structs Evento/Vertice
// qtd_eventos: total de vértices
sort(vetor_eventos, qtd_eventos, sizeof(Evento), comparaAnguloEventos, sort_type, insertion_limit);
```

## 5\. Testes Unitários

Como exigido nas regras de avaliação, crie um arquivo `test_sort.c`:

1.  Crie vetores de `int` e `double` desordenados.
2.  Chame `sort` com a opção `m` e limite pequeno (ex: 3).
3.  Verifique se o vetor está ordenado.
4.  Chame `sort` com a opção `q`.
5.  Verifique a ordenação.

<!-- end list -->

```
```