#ifndef SVG_H
#define SVG_H
#include <stdio.h>
#include "../geo/geo.h"
#include "../visibilidade/visibilidade.h"

void svg_iniciar(FILE* f, double x, double y, double w, double h);
void svg_finalizar(FILE* f);
void svg_desenhar_cidade(FILE* f, Geo cidade);
void svg_desenhar_poligono(FILE* f, PoligonoVisibilidade pol, const char* cor, double opacidade);

#endif