#include "svg.h"
#include "../poligono/poligono.h"
#include <stdio.h>

void svg_desenhar_poligono(FILE *f, PoligonoVisibilidade pol, const char *cor, double opacidade) {
    if (!f || !pol) return;
    
    LinkedList vertices = poligono_get_vertices(pol);
    int n = list_size(vertices);
    
    if (n < 3) return; // Polígono precisa de pelo menos 3 vértices
    
    fprintf(f, "<polygon points=\"");
    
    for (int i = 0; i < n; i++) {
        Ponto *p = (Ponto *)list_get_at(vertices, i);
        fprintf(f, "%.2f,%.2f ", p->x, p->y);
    }
    
    fprintf(f, "\" fill=\"%s\" fill-opacity=\"%.2f\" stroke=\"none\" />\n", cor, opacidade);
}
