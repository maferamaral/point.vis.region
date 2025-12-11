#ifndef SORT_H
#define SORT_H

#include <stddef.h>

/*
    Função de ordenação Merge Sort implementation.
*/
void merge_sort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void *));

/*
    Função de ordenação Insertion Sort implementation.
*/
void insertion_sort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void *));

/*
    Função wrapper de ordenação genérica.
    method: 'm' (merge), 'i' (insertion), 'q' (quicksort do sistema)
    threshold: limiar para algoritmos híbridos (pode ser ignorado se não usado)
*/
void sort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void *), char method, int threshold);

#endif // SORT_H
