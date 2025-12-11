#include "sort.h"
#include <stdlib.h>
#include <string.h>

// Helper macros for void* arithmetic
#define AT(base, i, size) ((char*)(base) + (i) * (size))

// --- Implementation of Insertion Sort ---
void insertionSort(void* base, size_t nmemb, size_t size, Comparator cmp) {
    if (nmemb <= 1) return;

    // Buffer temporário para armazenar o elemento pivô
    void* temp = malloc(size);
    if (!temp) return; // Erro de alocação

    for (size_t i = 1; i < nmemb; i++) {
        // Copia o elemento atual para o temp (pivô)
        memcpy(temp, AT(base, i, size), size);
        
        int j = (int)i - 1;

        // Move os elementos maiores que o pivô para frente
        while (j >= 0 && cmp(AT(base, j, size), temp) > 0) {
            memcpy(AT(base, j + 1, size), AT(base, j, size), size);
            j--;
        }
        
        // Insere o pivô na posição correta
        memcpy(AT(base, j + 1, size), temp, size);
    }

    free(temp);
}

// --- Internal Recursive Merge Sort ---
static void merge_internal(void* base, size_t left, size_t mid, size_t right, size_t size, Comparator cmp, void* aux) {
    size_t i = left;
    size_t j = mid + 1;
    size_t k = left;

    // Intercalação
    while (i <= mid && j <= right) {
        if (cmp(AT(base, i, size), AT(base, j, size)) <= 0) {
            memcpy(AT(aux, k, size), AT(base, i, size), size);
            i++;
        } else {
            memcpy(AT(aux, k, size), AT(base, j, size), size);
            j++;
        }
        k++;
    }

    // Copia o restante da metade esquerda
    while (i <= mid) {
        memcpy(AT(aux, k, size), AT(base, i, size), size);
        i++; k++;
    }

    // Copia o restante da metade direita
    while (j <= right) {
        memcpy(AT(aux, k, size), AT(base, j, size), size);
        j++; k++;
    }

    // Copia de volta do auxiliar para o base
    for (size_t p = left; p <= right; p++) {
        memcpy(AT(base, p, size), AT(aux, p, size), size);
    }
}

static void mergeSortRecursive(void* base, size_t left, size_t right, size_t size, Comparator cmp, int threshold, void* aux) {
    if (left >= right) return;

    size_t n = right - left + 1;
    
    // Otimização: Insertion Sort para vetores pequenos
    if (n <= (size_t)threshold) {
        // InsertionSort espera nmemb e começa do base relativo
        insertionSort(AT(base, left, size), n, size, cmp);
        return;
    }

    size_t mid = left + (right - left) / 2;
    mergeSortRecursive(base, left, mid, size, cmp, threshold, aux);
    mergeSortRecursive(base, mid + 1, right, size, cmp, threshold, aux);
    merge_internal(base, left, mid, right, size, cmp, aux);
}


// --- Wrapper for Merge Sort ---
void mergeSort(void* base, size_t nmemb, size_t size, Comparator cmp, int threshold) {
    if (nmemb <= 1) return;

    // Aloca vetor auxiliar uma única vez
    void* aux = malloc(nmemb * size);
    if (!aux) return; // Erro fatal

    mergeSortRecursive(base, 0, nmemb - 1, size, cmp, threshold, aux);

    free(aux);
}

// --- Main Sort Function ---
void sort(void* base, size_t nmemb, size_t size, Comparator cmp, char type, int insertion_threshold) {
    if (type == 'm') {
        mergeSort(base, nmemb, size, cmp, insertion_threshold);
    } else {
        // Default to qsort ('q')
        qsort(base, nmemb, size, cmp);
    }
}
