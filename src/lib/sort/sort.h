#ifndef SORT_H
#define SORT_H

#include <stddef.h>

/*
    Função de ordenação Merge Sort implementation.
    base: array to be sorted
    nitems: number of elements in the array
    size: size in bytes of each element
    compar: comparison function
*/
void merge_sort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void *));

/*
    Função de ordenação Insertion Sort implementation.
    base: array to be sorted
    nitems: number of elements in the array
    size: size in bytes of each element
    compar: comparison function
*/
void insertion_sort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void *));

#endif // SORT_H
