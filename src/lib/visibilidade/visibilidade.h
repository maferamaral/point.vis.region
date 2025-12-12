#ifndef VISIBILIDADE_H
#define VISIBILIDADE_H

#include "../geometria/geometria.h"
#include "../poligono/poligono.h"
#include <stdbool.h>

// Forward declaration para LinkedList
typedef void* LinkedList;

/**
 * Calcula o polígono de visibilidade a partir de um ponto.
 * Implementa o algoritmo de varredura angular.
 * 
 * @param centro Ponto de observação (fonte de luz)
 * @param barreiras LinkedList de Segmento* que bloqueiam a visão
 * @return Polígono de visibilidade, ou NULL em caso de erro
 */
PoligonoVisibilidade visibilidade_calcular(Ponto centro, LinkedList barreiras);

/**
 * Verifica se um ponto está dentro do polígono de visibilidade.
 * @param pol Polígono de visibilidade
 * @param p Ponto a verificar
 * @return true se o ponto foi "atingido" pela luz
 */
bool visibilidade_ponto_atingido(PoligonoVisibilidade pol, Ponto p);

/**
 * Verifica se um segmento é atingido pelo polígono.
 * @param pol Polígono de visibilidade
 * @param p1 Primeiro extremo do segmento
 * @param p2 Segundo extremo do segmento
 * @return true se qualquer parte do segmento está visível
 */
bool visibilidade_segmento_atingido(PoligonoVisibilidade pol, Ponto p1, Ponto p2);

/**
 * Destrói o polígono e libera memória.
 */
void visibilidade_destruir(PoligonoVisibilidade pol);

/**
 * Retorna a lista de vértices do polígono (para desenho SVG).
 */
LinkedList visibilidade_obter_vertices(PoligonoVisibilidade pol);

/**
 * Retorna o centro usado para calcular a visibilidade.
 */
Ponto visibilidade_obter_centro(PoligonoVisibilidade pol);

/**
 * Define o método de ordenação: 'q' = qsort, 'm' = merge, 'i' = insertion
 */
void visibilidade_set_sort_method(char method);

#endif