#include <stdio.h>
#include <stdlib.h>
#include "lib/utils/sort/sort.h"

// Comparators
int cmp_int(const void *a, const void *b) {
    int i = *(const int*)a;
    int j = *(const int*)b;
    return i - j;
}

int cmp_double(const void *a, const void *b) {
    double i = *(const double*)a;
    double j = *(const double*)b;
    if (i < j) return -1;
    if (i > j) return 1;
    return 0;
}

void print_int_array(int *arr, int n) {
    for(int i=0; i<n; i++) printf("%d ", arr[i]);
    printf("\n");
}

int main() {
    printf("Running Sort Tests...\n");

    // Test 1: Merge Sort with Small Threshold (triggers insertion)
    printf("\n[Test 1] Merge Sort (Int) threshold=3\n");
    int arr1[] = {5, 2, 9, 1, 5, 6, 7, 3, 8, 4};
    int n1 = 10;
    sort(arr1, n1, sizeof(int), cmp_int, 'm', 3);
    print_int_array(arr1, n1);
    
    // Check order
    for(int i=0; i<n1-1; i++) {
        if(arr1[i] > arr1[i+1]) { printf("FAIL!\n"); return 1; }
    }
    printf("PASS\n");

    // Test 2: Quick Sort
    printf("\n[Test 2] Quick Sort (Int)\n");
    int arr2[] = {10, -1, 0, 4, 3, 2, 8, 7, 6, 5};
    int n2 = 10;
    sort(arr2, n2, sizeof(int), cmp_int, 'q', 10);
    print_int_array(arr2, n2);
    
    for(int i=0; i<n2-1; i++) {
        if(arr2[i] > arr2[i+1]) { printf("FAIL!\n"); return 1; }
    }
    printf("PASS\n");

    // Test 3: Large Array Merge Sort
    printf("\n[Test 3] Merge Sort (Double) Large Array\n");
    int n3 = 100;
    double *arr3 = malloc(n3 * sizeof(double));
    for(int i=0; i<n3; i++) arr3[i] = (double)(rand() % 1000) / 10.0;
    
    sort(arr3, n3, sizeof(double), cmp_double, 'm', 10);
    
    for(int i=0; i<n3-1; i++) {
        if(arr3[i] > arr3[i+1]) { printf("FAIL at index %d\n", i); return 1; }
    }
    printf("PASS\n");
    free(arr3);

    return 0;
}
