#ifndef VISIBILIDADE_H
#define VISIBILIDADE_H

#include "../geometria/geometria.h"
#include "../utils/lista/lista.h"

// Representa o polígono de luz gerado pela bomba
typedef struct
{
    LinkedList pontos; // Lista de 'Ponto' (vértices do polígono)
} PoligonoVisibilidade;

/**
 * Calcula a região de visibilidade.
 * @param centro Ponto da explosão (bomba)
 * @param barreiras LinkedList de 'Segmento' (todas as paredes da cidade)
 * @return PoligonoVisibilidade contendo os vértices da área afetada
 */
PoligonoVisibilidade *visibilidade_calcular(Ponto centro, LinkedList barreiras);

/**
 * Verifica se um ponto está dentro do polígono de visibilidade.
 * Usado para saber se uma forma foi atingida.
 */
bool visibilidade_ponto_dentro(PoligonoVisibilidade *pol, Ponto p);

void visibilidade_destruir(PoligonoVisibilidade *pol);

#endif