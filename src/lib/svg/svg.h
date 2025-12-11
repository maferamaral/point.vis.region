#ifndef SVG_H
#define SVG_H

#include <stdio.h>
#include "../poligono/poligono.h"

/**
 * Desenha um polígono de visibilidade em um arquivo SVG.
 * 
 * @param f Arquivo SVG aberto para escrita
 * @param pol Polígono de visibilidade a desenhar
 * @param cor Cor de preenchimento (nome ou código hex)
 * @param opacidade Opacidade do preenchimento (0.0 a 1.0)
 */
void svg_desenhar_poligono(FILE *f, PoligonoVisibilidade pol, const char *cor, double opacidade);

#endif
