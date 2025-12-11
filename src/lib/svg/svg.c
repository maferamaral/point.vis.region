#include "svg.h"
#include <stdio.h>
#include "../utils/lista/lista.h"

void svg_desenhar_poligono(FILE *svg, PoligonoVisibilidade *poly, const char *color, double opacity)
{
    if (!svg || !poly)
        return;

    int n = list_size(poly->vertices);
    if (n == 0)
        return;

    fprintf(svg, "<polygon points=\"");
    for (int i = 0; i < n; i++)
    {
        Ponto *p = (Ponto *)list_get_at(poly->vertices, i);
        fprintf(svg, "%.2f,%.2f ", p->x, p->y);
    }
    fprintf(svg, "\" style=\"fill:%s;fill-opacity:%.2f;stroke:%s;stroke-width:1\" />\n", color, opacity, color);
}
