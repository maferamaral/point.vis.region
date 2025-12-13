#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../lib/utils/lista/lista.h"

// Helper to destroy integers (though we are just storing pointers to stack vars or malloced vars)
void destroy_int(void *data) {
    if (data) free(data);
}

void test_create_destroy() {
    printf("Testing create and destroy...\n");
    LinkedList l = list_create();
    assert(l != NULL);
    assert(list_is_empty(l) == 1);
    assert(list_size(l) == 0);
    list_destroy(l);
    printf("Create and destroy passed.\n");
}

void test_insert_remove_front() {
    printf("Testing insert/remove front...\n");
    LinkedList l = list_create();
    
    int *v1 = malloc(sizeof(int)); *v1 = 10;
    int *v2 = malloc(sizeof(int)); *v2 = 20;

    list_insert_front(l, v1);
    assert(list_size(l) == 1);
    assert(list_is_empty(l) == 0);
    assert(*(int*)list_front(l) == 10);

    list_insert_front(l, v2);
    assert(list_size(l) == 2);
    assert(*(int*)list_front(l) == 20);

    int *r1 = (int*)list_remove_front(l);
    assert(*r1 == 20);
    assert(list_size(l) == 1);

    int *r2 = (int*)list_remove_front(l);
    assert(*r2 == 10);
    assert(list_size(l) == 0);

    free(v1);
    free(v2);
    list_destroy(l);
    printf("Insert/remove front passed.\n");
}

void test_insert_remove_back() {
    printf("Testing insert/remove back...\n");
    LinkedList l = list_create();
    
    int *v1 = malloc(sizeof(int)); *v1 = 10;
    int *v2 = malloc(sizeof(int)); *v2 = 20;

    list_insert_back(l, v1);
    assert(list_size(l) == 1);
    assert(*(int*)list_back(l) == 10);

    list_insert_back(l, v2);
    assert(list_size(l) == 2);
    assert(*(int*)list_back(l) == 20);
    assert(*(int*)list_front(l) == 10); // Front should still be 10

    int *r1 = (int*)list_remove_back(l);
    assert(*r1 == 20);
    assert(list_size(l) == 1);

    int *r2 = (int*)list_remove_back(l);
    assert(*r2 == 10);
    assert(list_size(l) == 0);

    free(v1);
    free(v2);
    list_destroy(l);
    printf("Insert/remove back passed.\n");
}

void test_get_remove_at() {
    printf("Testing get/remove at...\n");
    LinkedList l = list_create();
    
    int *v1 = malloc(sizeof(int)); *v1 = 10;
    int *v2 = malloc(sizeof(int)); *v2 = 20;
    int *v3 = malloc(sizeof(int)); *v3 = 30;

    list_insert_back(l, v1);
    list_insert_back(l, v2);
    list_insert_back(l, v3);

    assert(*(int*)list_get_at(l, 0) == 10);
    assert(*(int*)list_get_at(l, 1) == 20);
    assert(*(int*)list_get_at(l, 2) == 30);
    assert(list_get_at(l, 3) == NULL);

    void *removed = list_remove_at(l, 1); // Remove 20
    assert(*(int*)removed == 20);
    assert(list_size(l) == 2);
    assert(*(int*)list_get_at(l, 1) == 30); // 30 shifts to index 1

    free(v1);
    free(v2);
    free(v3);
    list_destroy(l);
    printf("Get/remove at passed.\n");
}

int main() {
    test_create_destroy();
    test_insert_remove_front();
    test_insert_remove_back();
    test_get_remove_at();
    printf("ALL TESTS PASSED for LinkedList.\n");
    return 0;
}
