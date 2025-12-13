#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../lib/formas/texto/texto.h"

void test_texto_lifecycle() {
    printf("Testing texto lifecycle...\n");
    int id = 4;
    double x = 50.0, y = 60.0;
    const char *cb = "gray";
    const char *cp = "pink";
    char anchor = 'i';
    const char *content = "Hello World";

    Text t = text_create(id, x, y, cb, cp, anchor, content);
    assert(t != NULL);

    assert(text_get_id(t) == id);
    assert(text_get_x(t) == x);
    assert(text_get_y(t) == y);
    assert(strcmp(text_get_border_color(t), cb) == 0);
    assert(strcmp(text_get_fill_color(t), cp) == 0);
    assert(text_get_anchor(t) == anchor);
    assert(strcmp(text_get_text(t), content) == 0);
    assert(text_get_length(t) == strlen(content));

    text_set_border_color(t, "brown");
    text_set_fill_color(t, "magenta");
    assert(strcmp(text_get_border_color(t), "brown") == 0);
    assert(strcmp(text_get_fill_color(t), "magenta") == 0);

    text_destroy(t);
    printf("Texto passes.\n");
}

int main() {
    test_texto_lifecycle();
    printf("ALL TESTS PASSED for Texto.\n");
    return 0;
}
