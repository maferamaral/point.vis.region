#include "svg.h"
#include "../utils/lista/lista.h"
#include "../poligono/poligono.h"
#include <stdlib.h>

void svg_iniciar(FILE* f, double x, double y, double w, double h) {
    fprintf(f, "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"%.2f %.2f %.2f %.2f\">\n", x, y, w, h);
}

void svg_finalizar(FILE* f) {
    fprintf(f, "</svg>");
}

void svg_desenhar_cidade(FILE* f, Geo cidade) {
    geo_escrever_svg(cidade, f);
}

void svg_desenhar_poligono(FILE* f, PoligonoVisibilidade pol, const char* cor, double opacidade) {
    if (!pol) return;
    
    // Usa API encapsulada (não acessa struct diretamente)
    LinkedList vertices = poligono_get_vertices(pol);
    if (vertices == NULL) return;
    
    fprintf(f, "<polygon points=\"");
    
    int n = list_size(vertices);
    for (int i = 0; i < n; i++) {
        Ponto* p = (Ponto*)list_get_at(vertices, i);
        fprintf(f, "%.2f,%.2f ", p->x, p->y);
    }
    
    fprintf(f, "\" fill=\"%s\" fill-opacity=\"%.2f\" stroke=\"none\" />\n", cor, opacidade);
}