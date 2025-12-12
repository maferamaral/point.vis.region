#include "svg.h"
#include "../utils/lista/lista.h"
#include "../poligono/poligono.h"
#include <stdlib.h>

void svg_iniciar(FILE* f, double x, double y, double w, double h) {
    if (!f) return;
    // Padrão: viewbox e dimensões
    fprintf(f, "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" viewBox=\"%.2f %.2f %.2f %.2f\" width=\"100%%\" height=\"100%%\">\n", x, y, w, h);
}

void svg_finalizar(FILE* f) {
    if (!f) return;
    fprintf(f, "</svg>\n");
}

void svg_desenhar_cidade(FILE* f, Geo cidade) {
    if (!f || !cidade) return;
    geo_escrever_svg(cidade, f);
}

void svg_desenhar_poligono(FILE* f, PoligonoVisibilidade pol, const char* cor, double opacidade) {
    if (!pol) return;
    fprintf(f, "<polygon points=\"");
    
    // Use Polygon API to access vertices (as array for performance)
    // Needs cast to Poligono as PoligonoVisibilidade is void*
    int n = 0;
    double *coords = poligono_get_vertices_ref((Poligono)pol, &n);
    
    if (coords) {
        for (int i = 0; i < n; i++) {
            fprintf(f, "%.2f,%.2f ", coords[2*i], coords[2*i+1]);
        }
    }
    
    fprintf(f, "\" fill=\"%s\" fill-opacity=\"%.2f\" stroke=\"none\" />\n", cor, opacidade);
}