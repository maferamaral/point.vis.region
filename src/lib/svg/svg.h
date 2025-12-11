#ifndef SVG_H
#define SVG_H

#include <stdio.h>
#include "../visibilidade/visibilidade.h"

// Desenha um polígono de visibilidade no arquivo SVG (versão simples para debug)
void svg_desenhar_poligono(FILE *svg, PoligonoVisibilidade poly, const char *color, double opacity);

#endif // SVG_H
