#ifndef VISIBILIDADE_H
#define VISIBILIDADE_H

#include "../geometria/geometria.h"
#include "../utils/lista/lista.h"
#include "../poligono/poligono.h"

/**
 * Calcula o polígono de visibilidade a partir de um ponto central,
 * considerando uma lista de segmentos como barreiras.
 * 
 * @param centro Ponto de origem da visibilidade
 * @param barreiras LinkedList de Segmento* que bloqueiam a visão
 * @return PoligonoVisibilidade representando a região visível
 */
PoligonoVisibilidade visibilidade_calcular(Ponto centro, LinkedList barreiras);

/**
 * Verifica se um ponto está dentro do polígono de visibilidade.
 * 
 * @param pol Polígono de visibilidade
 * @param p Ponto a verificar
 * @return true se o ponto está dentro ou na borda do polígono
 */
bool visibilidade_ponto_atingido(PoligonoVisibilidade pol, Ponto p);

/**
 * Libera a memória alocada para um polígono de visibilidade.
 */
void visibilidade_destruir_poly(PoligonoVisibilidade pol);

/**
 * Configura parâmetros globais de ordenação para o algoritmo.
 * @param type Tipo de ordenação ('q' para quicksort, 'i' para insertion sort, 'h' para hybrid)
 * @param threshold Limite para trocar de quicksort para insertion sort no modo híbrido
 */
void visibilidade_set_sort_params(char type, int threshold);

#endif
