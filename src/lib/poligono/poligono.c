#include "poligono.h"
#include <stdlib.h>

typedef struct
{
    LinkedList vertices; // Lista de Ponto*
    Ponto centro;        
} PoligonoVisibilidadeImpl;

PoligonoVisibilidade poligono_criar(Ponto centro)
{
    PoligonoVisibilidadeImpl *pol = malloc(sizeof(PoligonoVisibilidadeImpl));
    pol->vertices = list_create();
    pol->centro = centro;
    return (PoligonoVisibilidade)pol;
}

void poligono_adicionar_vertice(PoligonoVisibilidade p, Ponto pt)
{
    PoligonoVisibilidadeImpl *pol = (PoligonoVisibilidadeImpl *)p;
    // Precisamos alocar o ponto pois a lista armazena void* e quem consome costuma liberar/usar
    // Mas note a lib geometria. Ponto é struct ou pointer?
    // Em geometria.h (que irei checar em breve), geralmente Ponto é struct de valor ou pointer.
    // Baseado no codigo antigo main.c do geo_handler: 
    // Segmento *s = malloc(sizeof(Segmento)); *s = segmento_criar(...);
    // Mas Ponto parece ser passado por valor em `ponto_criar`.
    // Vamos assumir que a lista guarda ponteiros para Ponto.
    
    Ponto *novo_pt = malloc(sizeof(Ponto));
    *novo_pt = pt;
    list_insert_back(pol->vertices, novo_pt);
}

LinkedList poligono_get_vertices(PoligonoVisibilidade p)
{
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
    PoligonoVisibilidadeImpl *pol = (PoligonoVisibilidadeImpl *)p;
    if (pol)
    {
        // Precisamos destruir os pontos da lista? 
        // Geralmente sim.
        // list_destroy(pol->vertices); // Se tiver destructor callback seria melhor.
        // Assumindo limpeza manual ou leak (melhorar depois se der)
        
        // Simplesmente destroi a lista por enquanto (codigos antigos nao mostravam deep free claro)
        // Mas vamos fazer um loop para garantir
        int n = list_size(pol->vertices);
        for(int i=0; i<n; i++) {
            free(list_get_at(pol->vertices, i));
        }
        list_destroy(pol->vertices);
        free(pol);
    }
}
