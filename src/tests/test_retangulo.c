#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../lib/formas/retangulo/retangulo.h"

void test_retangulo_lifecycle() {
    printf("Testing retangulo lifecycle...\n");
    int id = 2;
    double x = 5.0, y = 5.0, w = 10.0, h = 20.0;
    const char *cb = "black";
    const char *cp = "white";

    // Testing the Portuguese alias as it is consistent with project roots
    Retangulo r = retangulo_criar(id, x, y, w, h, cb, cp);
    assert(r != NULL);

    assert(retangulo_get_id(r) == id);
    assert(retangulo_get_x(r) == x);
    assert(retangulo_get_y(r) == y);
    assert(retangulo_get_largura(r) == w);
    assert(retangulo_get_altura(r) == h);
    assert(strcmp(retangulo_get_cor_borda(r), cb) == 0);
    assert(strcmp(retangulo_get_cor_preenchimento(r), cp) == 0);

    retangulo_set_cor_borda(r, "purple");
    retangulo_set_cor_preenchimento(r, "cyan");
    assert(strcmp(retangulo_get_cor_borda(r), "purple") == 0);
    assert(strcmp(retangulo_get_cor_preenchimento(r), "cyan") == 0);

    retangulo_destruir(r);
    printf("Retangulo passes.\n");
}

int main() {
    test_retangulo_lifecycle();
    printf("ALL TESTS PASSED for Retangulo.\n");
    return 0;
}
