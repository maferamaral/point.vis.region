#ifndef VISIBILIDADE_H
#define VISIBILIDADE_H

#include "../geometria/geometria.h"
#include <stdbool.h>

// Forward declaration - using existing LinkedList API
typedef void* LinkedList;

// Ponteiro opaco para o polígono de visibilidade
typedef struct PoligonoVisibilidade_st* PoligonoVisibilidade;

// Calcula o polígono de visibilidade
PoligonoVisibilidade visibilidade_calcular(Ponto centro, LinkedList barreiras);

// Verifica se um ponto está dentro do polígono (para saber se foi atingido)
bool visibilidade_ponto_atingido(PoligonoVisibilidade pol, Ponto p);

void visibilidade_destruir(PoligonoVisibilidade pol);


// Define o método de ordenação a ser usado:
// 'q' = qsort (default)
// 'm' = merge_sort
// 'i' = insertion_sort
void visibilidade_set_sort_method(char method);

#endif