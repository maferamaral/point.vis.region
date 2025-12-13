#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../lib/formas/linha/linha.h"

void test_linha_lifecycle() {
    printf("Testing linha lifecycle...\n");
    int id = 3;
    double x1 = 0.0, y1 = 0.0, x2 = 10.0, y2 = 10.0;
    const char *c_color = "black";

    Line l = line_create(id, x1, y1, x2, y2, c_color);
    assert(l != NULL);

    assert(line_get_id(l) == id);
    assert(line_get_x1(l) == x1);
    assert(line_get_y1(l) == y1);
    assert(line_get_x2(l) == x2);
    assert(line_get_y2(l) == y2);
    assert(strcmp(line_get_color(l), c_color) == 0);

    line_set_color(l, "orange");
    assert(strcmp(line_get_color(l), "orange") == 0);

    line_destroy(l);
    printf("Linha passes.\n");
}

int main() {
    test_linha_lifecycle();
    printf("ALL TESTS PASSED for Linha.\n");
    return 0;
}
