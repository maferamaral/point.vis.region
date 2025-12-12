/* sort.h
 *
 * Módulo de ordenação genérica.
 * Suporta QSort e MergeSort Híbrido (Merge + Insertion).
 */

#ifndef SORT_H
#define SORT_H

#include <stddef.h>

/* Tipos de algoritmos de ordenação */
typedef enum {
    ALG_QSORT,
    ALG_MERGESORT
} AlgoritmoOrdenacao;

/* Tipo para função de comparação (estilo qsort) */
typedef int (*FuncaoComparacao)(const void*, const void*);

/**
 * Ordena um array usando o algoritmo especificado.
 * 
 * @param base Ponteiro para o início do array
 * @param nmemb Número de elementos
 * @param size Tamanho de cada elemento
 * @param compar Função de comparação
 * @param alg Algoritmo a ser utilizado (ALG_QSORT ou ALG_MERGESORT)
 * @param limiar Limiar para Insertion Sort (apenas para ALG_MERGESORT)
 */
void ordenar(void *base, size_t nmemb, size_t size, 
             FuncaoComparacao compar, AlgoritmoOrdenacao alg, int limiar);

#endif /* SORT_H */
