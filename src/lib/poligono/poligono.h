#ifndef POLIGONO_H
#define POLIGONO_H

#include "../geometria/geometria.h"
#include "../utils/lista/lista.h"

typedef void *PoligonoVisibilidade;

// Cria um polígono de visibilidade
PoligonoVisibilidade poligono_criar(Ponto centro);

// Adiciona um vértice ao polígono (faz cópia do ponto se necessário, dependendo da impl)
void poligono_adicionar_vertice(PoligonoVisibilidade pol, Ponto pt);

// Retorna a lista de vértices (Ponto*)
LinkedList poligono_get_vertices(PoligonoVisibilidade pol);

// Retorna o centro do polígono
Ponto poligono_get_centro(PoligonoVisibilidade pol);

// Destrói o polígono
void poligono_destruir(PoligonoVisibilidade pol);

#endif
