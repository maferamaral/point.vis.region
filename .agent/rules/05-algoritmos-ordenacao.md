---
trigger: always_on
---

# Requisitos de Ordenação (Sorting)

O projeto exige flexibilidade e performance na ordenação dos vetores/listas.

## 1. Assinatura Padrão
As funções de ordenação devem aceitar ponteiros para função de comparação, similar ao padrão `stdlib.h`:
`int (*compar)(const void*, const void*)`

## 2. Algoritmos Exigidos
O código deve ser capaz de selecionar (via parâmetro de linha de comando ou configuração) entre:

1.  **QSort:** Usar o `qsort` nativo da biblioteca `<stdlib.h>`.
2.  **MergeSort Customizado:**
    * Você deve implementar o algoritmo MergeSort.
    * [cite_start]**Otimização Híbrida:** Quando o subvetor/partição for "pequeno" (tamanho definido por parâmetro, default geralmente 10 ou 15), o algoritmo deve alternar para **InsertionSort** [cite: 110-111].

## 3. Aplicação
Esta ordenação será usada principalmente para ordenar a **Lista de Eventos (Vértices)** baseada no ângulo polar em relação ao ponto `x` da bomba.