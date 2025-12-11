#ifndef VISIBILIDADE_H
#define VISIBILIDADE_H

#include "../poligono/poligono.h"
#include "../geo/geo.h" // Se precisar de algo do geo

/**
 * Calcula a região de visibilidade usando varredura angular.
 * @param centro Ponto da bomba.
 * @param barreiras Lista de Segmentos (paredes).
 * @return Polígono com a região visível.
 */
PoligonoVisibilidade visibilidade_calcular(Ponto centro, LinkedList barreiras);

/**
 * Verifica se um ponto está dentro da área atingida.
 */
bool visibilidade_ponto_atingido(PoligonoVisibilidade pol, Ponto p);

void visibilidade_destruir_poly(PoligonoVisibilidade pol);

/**
 * Configure sort parameters for visibility algorithm.
 * @param type 'q' or 'm'
 * @param threshold Limit for insertion sort optimization
 */
void visibilidade_set_sort_params(char type, int threshold);

#endif