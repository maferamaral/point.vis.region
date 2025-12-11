#include "sort.h"
#include <stdlib.h>
#include <string.h>

// Função auxiliar para trocar dois elementos de tamanho arbitrário (usada no insertion)
static void swap(void *a, void *b, size_t size) {
    char temp[size];
    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);
}

// ==== Insertion Sort ====
void insertion_sort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void *)) {
    char *arr = (char *)base;
    for (size_t i = 1; i < nitems; i++) {
        char temp[size];
        memcpy(temp, arr + i * size, size);
        
        int j = (int)i - 1;
        
        while (j >= 0 && compar(arr + j * size, temp) > 0) {
            memcpy(arr + (j + 1) * size, arr + j * size, size);
            j--;
        }
        
        memcpy(arr + (j + 1) * size, temp, size);
    }
}

// ==== Merge Sort ====

static void merge(char *base, size_t size, int left, int mid, int right, int (*compar)(const void *, const void *)) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    char *L = malloc(n1 * size);
    char *R = malloc(n2 * size);

    memcpy(L, base + left * size, n1 * size);
    memcpy(R, base + (mid + 1) * size, n2 * size);

    int i = 0;
    int j = 0;
    int k = left;

    while (i < n1 && j < n2) {
        if (compar(L + i * size, R + j * size) <= 0) {
            memcpy(base + k * size, L + i * size, size);
            i++;
        } else {
            memcpy(base + k * size, R + j * size, size);
            j++;
        }
        k++;
    }

    while (i < n1) {
        memcpy(base + k * size, L + i * size, size);
        i++;
        k++;
    }

    while (j < n2) {
        memcpy(base + k * size, R + j * size, size);
        j++;
        k++;
    }

    free(L);
    free(R);
}

static void merge_sort_recursive(char *base, size_t size, int left, int right, int (*compar)(const void *, const void *)) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        merge_sort_recursive(base, size, left, mid, compar);
        merge_sort_recursive(base, size, mid + 1, right, compar);

        merge(base, size, left, mid, right, compar);
    }
}

void merge_sort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void *)) {
    if (nitems < 2) return;
    merge_sort_recursive((char *)base, size, 0, (int)nitems - 1, compar);
}

// ==== Generic Sort Wrapper ====
void sort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void *), char method, int threshold) {
    switch (method) {
        case 'm':
            merge_sort(base, nitems, size, compar);
            break;
        case 'i':
            insertion_sort(base, nitems, size, compar);
            break;
        case 'q':
        default:
            qsort(base, nitems, size, compar);
            break;
    }
}
