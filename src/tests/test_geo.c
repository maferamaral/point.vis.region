#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../lib/geo/geo.h"
#include "../lib/utils/lista/lista.h"

void create_sample_geo(const char *filename) {
    FILE *f = fopen(filename, "w");
    assert(f != NULL);
    fprintf(f, "c 1 10 10 5 red blue\n");
    fprintf(f, "r 2 20 20 10 5 green yellow\n");
    fprintf(f, "l 3 0 0 100 100 black\n");
    fprintf(f, "t 4 50 50 purple orange i HelloTest\n");
    fclose(f);
}

void test_geo_lifecycle() {
    printf("Testing geo lifecycle...\n");
    const char *geo_file = "test_sample.geo";
    create_sample_geo(geo_file);

    Geo g = geo_criar();
    assert(g != NULL);

    geo_ler(g, geo_file);

    LinkedList shapes = geo_get_formas(g);
    assert(shapes != NULL);
    // Depending on implementation, shapes might be in the list.
    // list_size(shapes) should be 4.
    assert(list_size(shapes) == 4);

    // Test removing a shape
    geo_remover_forma(g, 1);
    assert(list_size(shapes) == 3);

    geo_destruir(g);
    remove(geo_file);
    printf("Geo passes.\n");
}

int main() {
    test_geo_lifecycle();
    printf("ALL TESTS PASSED for Geo.\n");
    return 0;
}
