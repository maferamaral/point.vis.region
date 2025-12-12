#ifndef POLIGONO_H
#define POLIGONO_H

#include "../geometria/geometria.h"
#include "../utils/lista/lista.h"

/**
 * TAD PoligonoVisibilidade - Ponteiro Opaco
 * Armazena a sequência ordenada de vértices que delimitam a região visível.
 * A struct é definida apenas no .c (encapsulamento).
 */
typedef void *PoligonoVisibilidade;

/**
 * Cria um novo polígono de visibilidade.
 * @param centro Fonte de luz/observador
 * @return Novo polígono, ou NULL em caso de erro
 */
PoligonoVisibilidade poligono_criar(Ponto centro);

/**
 * Adiciona um vértice ao polígono.
 * Aloca uma cópia dinâmica do ponto e insere no final da lista.
 * @param pol Polígono
 * @param pt Ponto a adicionar (será copiado)
 */
void poligono_adicionar_vertice(PoligonoVisibilidade pol, Ponto pt);

/**
 * Retorna a lista de vértices para iteração.
 * @param pol Polígono
 * @return LinkedList de Ponto*
 */
LinkedList poligono_get_vertices(PoligonoVisibilidade pol);

/**
 * Retorna o centro (fonte de luz).
 * @param pol Polígono
 * @return Ponto centro
 */
Ponto poligono_get_centro(PoligonoVisibilidade pol);

/**
 * Destrói o polígono e libera toda memória.
 * Realiza Deep Free: libera cada Ponto* da lista.
 * @param pol Polígono a destruir
 */
void poligono_destruir(PoligonoVisibilidade pol);

#endif
