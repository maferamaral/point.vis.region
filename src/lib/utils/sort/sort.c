/* sort.c
 *
 * Implementação dos algoritmos de ordenação.
 */

#include <stdlib.h>
#include <string.h>
#include "sort.h"



/* ============================================================================
 * Algoritmos Auxiliares
 * ============================================================================ */

/**
 * Insertion Sort para pequenos arrays/subarrays.
 */
static void insertion_sort(char *base, size_t nmemb, size_t size, FuncaoComparacao compar)
{
    char *temp = (char*)malloc(size);
    if (temp == NULL) return;

    for (size_t i = 1; i < nmemb; i++)
    {
        memcpy(temp, base + i * size, size);
        size_t j = i;
        
        while (j > 0 && compar(temp, base + (j - 1) * size) < 0)
        {
            memcpy(base + j * size, base + (j - 1) * size, size);
            j--;
        }
        
        memcpy(base + j * size, temp, size);
    }
    
    free(temp);
}

/**
 * Função auxiliar para merge do MergeSort.
 */
static void merge(char *base, size_t left, size_t mid, size_t right, 
                  size_t size, FuncaoComparacao compar, char *aux)
{
    size_t i = left;
    size_t j = mid;
    size_t k = left;
    
    /* Copia arrays ordenados para vetor auxiliar */
    while (i < mid && j < right)
    {
        if (compar(base + i * size, base + j * size) <= 0)
        {
            memcpy(aux + k * size, base + i * size, size);
            i++;
        }
        else
        {
            memcpy(aux + k * size, base + j * size, size);
            j++;
        }
        k++;
    }
    
    /* Copia restantes */
    while (i < mid)
    {
        memcpy(aux + k * size, base + i * size, size);
        i++;
        k++;
    }
    
    while (j < right)
    {
        memcpy(aux + k * size, base + j * size, size);
        j++;
        k++;
    }
    
    /* Copia de volta para o array original */
    memcpy(base + left * size, aux + left * size, (right - left) * size);
}

/**
 * Implementação recursiva do MergeSort Híbrido.
 */
static void mergesort_recursivo(char *base, size_t left, size_t right, 
                                size_t size, FuncaoComparacao compar, char *aux, int limiar)
{
    size_t n = right - left;
    
    /* Caso base: array pequeno, usa Insertion Sort */
    if ((int)n <= limiar)
    {
        insertion_sort(base + left * size, n, size, compar);
        return;
    }
    
    size_t mid = left + n / 2;
    
    mergesort_recursivo(base, left, mid, size, compar, aux, limiar);
    mergesort_recursivo(base, mid, right, size, compar, aux, limiar);
    
    merge(base, left, mid, right, size, compar, aux);
}

/**
 * Wrapper para o MergeSort.
 */
static void mergesort_hibrido(void *base, size_t nmemb, size_t size, FuncaoComparacao compar, int limiar)
{
    if (nmemb < 2) return;
    
    /* Aloca array auxiliar uma única vez */
    char *aux = (char*)malloc(nmemb * size);
    if (aux == NULL)
    {
        /* Fallback para qsort em caso de falta de memória ??
           Ou tenta fazer insertion sort in-place (muito lento para arrays grandes).
           Vamos assumir qsort como fallback seguro. */
        qsort(base, nmemb, size, compar);
        return;
    }
    
    mergesort_recursivo((char*)base, 0, nmemb, size, compar, aux, limiar);
    
    free(aux);
}

/* ============================================================================
 * Interface Pública
 * ============================================================================ */

void ordenar(void *base, size_t nmemb, size_t size, 
             FuncaoComparacao compar, AlgoritmoOrdenacao alg, int limiar)
{
    if (alg == ALG_MERGESORT)
    {
        mergesort_hibrido(base, nmemb, size, compar, limiar);
    }
    else
    {
        /* Default: QSort padrão da libc */
        qsort(base, nmemb, size, compar);
    }
}
