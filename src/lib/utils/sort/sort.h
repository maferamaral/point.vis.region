#ifndef SORT_H
#define SORT_H

#include <stddef.h>

// Definição do tipo ponteiro de função para comparação.
// Deve retornar:
// < 0 se a < b
//   0 se a == b
// > 0 se a > b
typedef int (*Comparator)(const void* a, const void* b);

/**
 * Interface unificada para ordenação.
 * @param base Ponteiro para o início do vetor.
 * @param nmemb Número de elementos.
 * @param size Tamanho de cada elemento em bytes.
 * @param cmp Função de comparação.
 * @param type Tipo de algoritmo: 'q' (QSort) ou 'm' (MergeSort).
 * @param insertion_threshold Limite para ativar InsertionSort (usado apenas no Merge).
 */
void sort(void* base, size_t nmemb, size_t size, Comparator cmp, char type, int insertion_threshold);

// Funções auxiliares (expostas para testes unitários conforme solicitado)
void insertionSort(void* base, size_t nmemb, size_t size, Comparator cmp);
void mergeSort(void* base, size_t nmemb, size_t size, Comparator cmp, int threshold);

#endif
