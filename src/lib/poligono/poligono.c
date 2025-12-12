/* poligono.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "poligono.h"
#include "../utils/lista/lista.h"
#include "../geometria/ponto/ponto.h"

#define INITIAL_CAPACITY 16

/* Definição concreta da struct baseada em Array Dinâmico */
typedef struct poligono_st {
    double *coords;     /* [x0, y0, x1, y1, ...] */
    int num_vertices;
    int capacity;       /* Capacidade atual do array em número de vértices */
    LinkedList lista_cache;  /* Cache para uso legado, invalidada ao alterar */
} PoligonoStruct;

Poligono poligono_criar() {
    PoligonoStruct *p = (PoligonoStruct*)malloc(sizeof(PoligonoStruct));
    if (p == NULL) return NULL;

    p->coords = (double*)malloc(2 * INITIAL_CAPACITY * sizeof(double));
    if (p->coords == NULL) {
        free(p);
        return NULL;
    }

    p->num_vertices = 0;
    p->capacity = INITIAL_CAPACITY;
    p->lista_cache = NULL;
    
    return (Poligono)p;
}

static void limpar_cache(PoligonoStruct *ps) {
    if (ps->lista_cache != NULL) {
        /* Precisamos de uma func que destrói ponto, mas aqui os pontos são cópias? */
        /* Sim, lista_cache terá pontos alocados */
        // Assuming list_destroy takes a function pointer to free elements
        // In src, list_destroy signature is `void list_destroy(LinkedList l)`?
        // Wait, looking at src: `list_destroy(LinkedList l)` usually just frees the nodes, but NOT the data if data is void*.
        // `src/lib/utils/lista/lista.h` check needed?
        // srcAndre used `destruir_lista` with callback.
        // I will assume for now list_destroy doesn't free data or I need to free manually.
        // Let's assume manual iteration to free.
        
        while(!list_is_empty(ps->lista_cache)) {
            void* el = list_remove_front(ps->lista_cache);
            if (el) destruir_ponto(el);
        }
        list_destroy(ps->lista_cache);
        ps->lista_cache = NULL;
    }
}

void poligono_destruir(Poligono p) {
    PoligonoStruct *ps = (PoligonoStruct*)p;
    if (ps != NULL) {
        if (ps->coords != NULL) {
            free(ps->coords);
        }
        limpar_cache(ps);
        free(ps);
    }
}

void poligono_inserir_vertice(Poligono p, double x, double y) {
    PoligonoStruct *ps = (PoligonoStruct*)p;
    if (ps == NULL) return;

    if (ps->num_vertices >= ps->capacity) {
        int new_capacity = ps->capacity * 2;
        double *new_coords = (double*)realloc(ps->coords, 2 * new_capacity * sizeof(double));
        if (new_coords == NULL) return; /* Falha na alocação */
        
        ps->coords = new_coords;
        ps->capacity = new_capacity;
    }

    ps->coords[2 * ps->num_vertices] = x;
    ps->coords[2 * ps->num_vertices + 1] = y;
    ps->num_vertices++;
    
    limpar_cache(ps); /* Invalida cache legado */
}

int poligono_qtd_vertices(Poligono p) {
    PoligonoStruct *ps = (PoligonoStruct*)p;
    if (ps == NULL) return 0;
    return ps->num_vertices;
}

Ponto poligono_get_vertice(Poligono p, int indice) {
    PoligonoStruct *ps = (PoligonoStruct*)p;
    if (ps == NULL || indice < 0 || indice >= ps->num_vertices) return NULL;

    /* Retorna uma copia fresca */
    return criar_ponto(ps->coords[2*indice], ps->coords[2*indice+1]);
}

double* poligono_get_vertices_ref(Poligono p, int *num_vertices) {
    PoligonoStruct *ps = (PoligonoStruct*)p;
    if (ps == NULL) {
        if(num_vertices) *num_vertices = 0;
        return NULL;
    }
    
    if(num_vertices) *num_vertices = ps->num_vertices;
    return ps->coords;
}

int poligono_obter_vertices_array(Poligono p, double **vertices_out) {
    PoligonoStruct *ps = (PoligonoStruct*)p;
    if (ps == NULL || vertices_out == NULL || ps->num_vertices == 0) {
        *vertices_out = NULL;
        return 0;
    }

    double *arr = (double*)malloc(2 * ps->num_vertices * sizeof(double));
    if (arr == NULL) return 0;

    memcpy(arr, ps->coords, 2 * ps->num_vertices * sizeof(double));

    *vertices_out = arr;
    return ps->num_vertices;
}

LinkedList poligono_obter_lista(Poligono p) {
    PoligonoStruct *ps = (PoligonoStruct*)p;
    if (ps == NULL) return NULL;
    
    if (ps->lista_cache == NULL) {
        ps->lista_cache = list_create();
        for (int i = 0; i < ps->num_vertices; i++) {
            Ponto pt = criar_ponto(ps->coords[2*i], ps->coords[2*i+1]);
            list_insert_back(ps->lista_cache, pt);
        }
    }
    
    return ps->lista_cache;
}
