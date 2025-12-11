#ifndef VISIBILIDADE_H
#define VISIBILIDADE_H

#include "../geometria/geometria.h"
#include "../utils/lista/lista.h"

// Polígono que define a área iluminada
typedef struct
{
    LinkedList vertices; // Lista de Ponto*
    Ponto centro;        // Centro da explosão (útil para verificações)
} PoligonoVisibilidade;

/**
 * Calcula a região de visibilidade usando varredura angular.
 * @param centro Ponto da bomba.
 * @param barreiras Lista de Segmentos (paredes).
 * @return Polígono com a região visível.
 */
PoligonoVisibilidade *visibilidade_calcular(Ponto centro, LinkedList barreiras);

/**
 * Verifica se um ponto está dentro da área atingida.
 */
bool visibilidade_ponto_atingido(PoligonoVisibilidade *pol, Ponto p);

void visibilidade_destruir(PoligonoVisibilidade *pol);

#endif