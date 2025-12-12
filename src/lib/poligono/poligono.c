/**
 * poligono.c
 * 
 * Implementação do TAD PoligonoVisibilidade.
 * Segue a especificação do professor (poligono.md).
 */

#include "poligono.h"
#include <stdlib.h>

/**
 * Estrutura interna do polígono (ocultada do usuário).
 * Definida apenas aqui no .c para garantir encapsulamento.
 */
typedef struct {
    LinkedList vertices;  // Lista encadeada de (Ponto*)
    Ponto centro;         // Fonte de luz/observador
} PoligonoVisibilidadeImpl;

PoligonoVisibilidade poligono_criar(Ponto centro)
{
    PoligonoVisibilidadeImpl *pol = malloc(sizeof(PoligonoVisibilidadeImpl));
    if (pol == NULL) return NULL;
    
    pol->vertices = list_create();
    pol->centro = centro;
    
    return (PoligonoVisibilidade)pol;
}

void poligono_adicionar_vertice(PoligonoVisibilidade p, Ponto pt)
{
    if (p == NULL) return;
    
    PoligonoVisibilidadeImpl *pol = (PoligonoVisibilidadeImpl *)p;
    
    // Aloca cópia dinâmica do ponto (ownership transferido ao polígono)
    Ponto *novo_pt = malloc(sizeof(Ponto));
    if (novo_pt == NULL) return;
    
    *novo_pt = pt;
    list_insert_back(pol->vertices, novo_pt);
}

LinkedList poligono_get_vertices(PoligonoVisibilidade p)
{
    if (p == NULL) return NULL;
    
    PoligonoVisibilidadeImpl *pol = (PoligonoVisibilidadeImpl *)p;
    return pol->vertices;
}

Ponto poligono_get_centro(PoligonoVisibilidade p)
{
    PoligonoVisibilidadeImpl *pol = (PoligonoVisibilidadeImpl *)p;
    return pol->centro;
}

void poligono_destruir(PoligonoVisibilidade p)
{
    if (p == NULL) return;
    
    PoligonoVisibilidadeImpl *pol = (PoligonoVisibilidadeImpl *)p;
    
    // Deep Free: libera cada Ponto* armazenado na lista
    while (!list_is_empty(pol->vertices))
    {
        Ponto *pt = (Ponto *)list_remove_front(pol->vertices);
        free(pt);
    }
    
    // Destroi a lista vazia
    list_destroy(pol->vertices);
    
    // Libera a estrutura do polígono
    free(pol);
}
