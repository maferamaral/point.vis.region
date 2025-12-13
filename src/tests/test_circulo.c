#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../lib/formas/circulo/circulo.h"

void test_circulo_lifecycle() {
    printf("Testing circulo lifecycle...\n");
    int id = 1;
    double x = 10.5, y = 20.5, r = 5.0;
    const char *cb = "red";
    const char *cp = "blue";

    Circulo c = circulo_criar(id, x, y, r, cb, cp);
    assert(c != NULL);

    assert(circulo_get_id(c) == id);
    assert(circulo_get_x(c) == x);
    assert(circulo_get_y(c) == y);
    assert(circulo_get_raio(c) == r);
    assert(strcmp(circulo_get_cor_borda(c), cb) == 0);
    assert(strcmp(circulo_get_cor_preenchimento(c), cp) == 0);

    // Test setters if available (based on header viewed previously)
    circulo_set_cor_borda(c, "green");
    circulo_set_cor_preenchimento(c, "yellow");
    assert(strcmp(circulo_get_cor_borda(c), "green") == 0);
    assert(strcmp(circulo_get_cor_preenchimento(c), "yellow") == 0);

    circulo_destruir(c);
    printf("Circulo passes.\n");
}

int main() {
    test_circulo_lifecycle();
    printf("ALL TESTS PASSED for Circulo.\n");
    return 0;
}
