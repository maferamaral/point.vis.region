/* visibilidade.c
 *
 * Implementação do Algoritmo de Região de Visibilidade
 * Varredura Angular (Angular Plane Sweep)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "visibilidade.h"
#include "../utils/lista/lista.h"
#include "../utils/sort/sort.h"
#include "../geometria/ponto/ponto.h"
#include "../geometria/segmento/segmento.h"
#include "../geometria/calculos/calculos.h"
#include "../arvore/arvore.h"
#include "../poligono/poligono.h"

// Note: Removed forma includes to avoid dependency on shape structure details if not strict necessary.

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define EPSILON 1e-9
#define MARGEM_BBOX 5.0

// Compatibility Globals
static char g_sort_method = 'q';

void visibilidade_set_sort_method(char method) {
    g_sort_method = method;
}

// Adapters
void visibilidade_destruir(PoligonoVisibilidade pol) {
    destruir_poligono_visibilidade(pol);
}

LinkedList visibilidade_obter_vertices(PoligonoVisibilidade pol) {
    return poligono_obter_vertices(pol);
}

PoligonoVisibilidade visibilidade_calcular(Ponto centro, LinkedList barreiras) {
    // Default call from src's QRY
    const char *sort_str = (g_sort_method == 'm') ? "mergesort" : "qsort";
    int limiar = 10;
    
    // We treat barreiras as a list of Segmento*
    // Min/Max are not provided, we must calculate bounding box of segments + margin?
    // Or just pass large values?
    // srcAndre calculates bounding box internally if limits are provided.
    // SRC `qry.c` passes `min_x`, etc to `geo_get_bounding_box`.
    // But `visibilidade_calcular` signature only has center and barriers.
    // We should compute BB from barriers.
    
    double min_x = 1e9, min_y = 1e9, max_x = -1e9, max_y = -1e9;
    
    // Iterate barriers to find bounds
    int n = list_size(barreiras);
    if(n == 0) {
        min_x = get_ponto_x(centro) - 100;
        max_x = get_ponto_x(centro) + 100;
        min_y = get_ponto_y(centro) - 100;
        max_y = get_ponto_y(centro) + 100;
    } else {
        for(int i=0; i<n; i++) {
            Segmento s = (Segmento)list_get_at(barreiras, i);
            double x1 = get_segmento_x1(s), y1 = get_segmento_y1(s);
            double x2 = get_segmento_x2(s), y2 = get_segmento_y2(s);
            if(x1 < min_x) min_x = x1; if(x1 > max_x) max_x = x1;
            if(y1 < min_y) min_y = y1; if(y1 > max_y) max_y = y1;
            if(x2 < min_x) min_x = x2; if(x2 > max_x) max_x = x2;
            if(y2 < min_y) min_y = y2; if(y2 > max_y) max_y = y2;
        }
    }
    
    // Include center
    if(get_ponto_x(centro) < min_x) min_x = get_ponto_x(centro);
    if(get_ponto_x(centro) > max_x) max_x = get_ponto_x(centro);
    if(get_ponto_y(centro) < min_y) min_y = get_ponto_y(centro);
    if(get_ponto_y(centro) > max_y) max_y = get_ponto_y(centro);

    return calcular_visibilidade(centro, barreiras, min_x, min_y, max_x, max_y, sort_str, limiar);
}

bool visibilidade_ponto_atingido(PoligonoVisibilidade pol, Ponto p) {
    if (!pol) return false;
    // Export vertices array for checking
    double *coords;
    int num = 0;
    coords = poligono_get_vertices_ref(pol, &num);
    return ponto_no_poligono(get_ponto_x(p), get_ponto_y(p), coords, num);
}

bool visibilidade_segmento_atingido(PoligonoVisibilidade pol, Ponto p1, Ponto p2) {
    if (!pol) return false;
    // Reuse implicit check manually?
    // Using simple endpoint check + intersection check?
    // See srcAndre `forma_no_poligono` logic for LINE.
    double *coords;
    int num = 0;
    coords = poligono_get_vertices_ref(pol, &num);
    
    double lx1 = get_ponto_x(p1), ly1 = get_ponto_y(p1);
    double lx2 = get_ponto_x(p2), ly2 = get_ponto_y(p2);
    
    if (ponto_no_poligono(lx1, ly1, coords, num)) return true;
    if (ponto_no_poligono(lx2, ly2, coords, num)) return true;
    
    // Check edge intersection
    for (int i = 0; i < num; i++) {
        int next = (i + 1) % num;
        double ax = coords[2*i], ay = coords[2*i+1];
        double bx = coords[2*next], by = coords[2*next+1];
        
        Ponto edge_p1 = criar_ponto(ax, ay);
        Ponto edge_p2 = criar_ponto(bx, by);
        
        // P1 and P2 are from arguments
        if (intersecao_segmentos(p1, p2, edge_p1, edge_p2)) {
            destruir_ponto(edge_p1);
            destruir_ponto(edge_p2);
            return true;
        }
        
        destruir_ponto(edge_p1);
        destruir_ponto(edge_p2);
    }
    
    return false;
}

/* ============================================================================
 * Estruturas Internas
 * ============================================================================ */

/* Tipo de vértice (evento) */
typedef enum {
    EVENTO_INICIO,
    EVENTO_FIM
} TipoEvento;

/* Evento da varredura (vértice de um segmento) */
typedef struct evento
{
    Ponto ponto;        /* Coordenada do vértice */
    double angulo;      /* Ângulo polar em relação à origem */
    double distancia;   /* Distância até a origem */
    TipoEvento tipo;    /* INICIO ou FIM */
    Segmento segmento;  /* Segmento ao qual pertence */
} Evento;

/* ============================================================================
 * Funções Auxiliares - Eventos
 * ============================================================================ */

static Evento* criar_evento(Ponto ponto, TipoEvento tipo, Segmento seg, Ponto origem)
{
    Evento *e = (Evento*)malloc(sizeof(Evento));
    if (e == NULL) return NULL;
    
    e->ponto = clonar_ponto(ponto);
    e->tipo = tipo;
    e->segmento = seg;
    e->angulo = ponto_angulo_polar(origem, ponto);
    e->distancia = ponto_distancia(origem, ponto);
    
    return e;
}

static void destruir_evento(void *ptr)
{
    Evento *e = (Evento*)ptr;
    if (e != NULL)
    {
        destruir_ponto(e->ponto);
        free(e);
    }
}

static int comparar_eventos(const void *a, const void *b)
{
    Evento *e1 = *(Evento**)a;
    Evento *e2 = *(Evento**)b;
    
    if (fabs(e1->angulo - e2->angulo) > EPSILON)
    {
        return (e1->angulo < e2->angulo) ? -1 : 1;
    }
    
    if (e1->tipo != e2->tipo)
    {
        return (e1->tipo == EVENTO_INICIO) ? -1 : 1;
    }
    
    if (fabs(e1->distancia - e2->distancia) > EPSILON)
    {
        return (e1->distancia < e2->distancia) ? -1 : 1;
    }
    
    return 0;
}

/* ============================================================================
 * Funções Auxiliares - Segmentos
 * ============================================================================ */

static void destruir_segmento_callback(void *ptr)
{
    destruir_segmento((Segmento)ptr);
}

static void criar_bounding_box(LinkedList segmentos, double min_x, double min_y, 
                                double max_x, double max_y)
{
    min_x -= MARGEM_BBOX;
    min_y -= MARGEM_BBOX;
    max_x += MARGEM_BBOX;
    max_y += MARGEM_BBOX;
    
    list_insert_back(segmentos, criar_segmento(-1, -1, min_x, min_y, max_x, min_y, "none"));
    list_insert_back(segmentos, criar_segmento(-2, -1, max_x, min_y, max_x, max_y, "none"));
    list_insert_back(segmentos, criar_segmento(-3, -1, max_x, max_y, min_x, max_y, "none"));
    list_insert_back(segmentos, criar_segmento(-4, -1, min_x, max_y, min_x, min_y, "none"));
}

static LinkedList extrair_eventos(LinkedList segmentos, Ponto origem)
{
    LinkedList eventos = list_create();
    if (eventos == NULL) return NULL;
    
    // Iterate list
    int n = list_size(segmentos);
    for(int i=0; i<n; i++)
    {
        Segmento seg = (Segmento)list_get_at(segmentos, i);
        
        Evento *e1 = criar_evento(get_segmento_p1(seg), EVENTO_INICIO, seg, origem);
        if (e1 != NULL) list_insert_back(eventos, e1);
        
        Evento *e2 = criar_evento(get_segmento_p2(seg), EVENTO_FIM, seg, origem);
        if (e2 != NULL) list_insert_back(eventos, e2);
    }
    
    return eventos;
}

static void ordenar_eventos_lista(LinkedList eventos, const char *tipo_ordenacao, int limiar)
{
    int n = list_size(eventos);
    if (n <= 1) return;
    
    Evento **arr = (Evento**)malloc(n * sizeof(Evento*));
    if (arr == NULL) return;
    
    for(int i=0; i<n; i++) {
        arr[i] = (Evento*)list_get_at(eventos, i);
    }
    
    AlgoritmoOrdenacao alg_enum = ALG_QSORT;
    if (tipo_ordenacao != NULL && strcmp(tipo_ordenacao, "mergesort") == 0)
    {
        alg_enum = ALG_MERGESORT;
    }
    
    ordenar((void*)arr, n, sizeof(Evento*), comparar_eventos, alg_enum, limiar);
    
    // Rebuild list
    while (!list_is_empty(eventos)) {
        list_remove_front(eventos);
    }
    
    for (int i = 0; i < n; i++) {
        list_insert_back(eventos, arr[i]);
    }
    
    free(arr);
}

/* ============================================================================
 * Algoritmo Principal de Visibilidade
 * ============================================================================ */

PoligonoVisibilidade calcular_visibilidade(Ponto origem, LinkedList segmentos_entrada,
                                            double min_x, double min_y,
                                            double max_x, double max_y,
                                            const char *tipo_ordenacao,
                                            int limiar_insertion)
{
    if (origem == NULL) return NULL;
    
    LinkedList segmentos = list_create();
    if (segmentos == NULL) return NULL;
    
    if (segmentos_entrada != NULL)
    {
        int n = list_size(segmentos_entrada);
        for(int i=0; i<n; i++)
        {
            Segmento seg = (Segmento)list_get_at(segmentos_entrada, i);
            list_insert_back(segmentos, clonar_segmento(seg));
        }
    }
    
    double ox = get_ponto_x(origem);
    double oy = get_ponto_y(origem);
    if (ox < min_x) min_x = ox;
    if (ox > max_x) max_x = ox;
    if (oy < min_y) min_y = oy;
    if (oy > max_y) max_y = oy;
    
    criar_bounding_box(segmentos, min_x, min_y, max_x, max_y);
    
    // Split segments at angle 0
    // Note: Iterate cautiously as we modify the list
    // Best way: iterate backwards or restart?
    // srcAndre uses linked list nodes directly. Here we use array-list (likely) or list that supports index.
    // list_size, list_get_at, list_remove_at...
    // Let's assume list_remove_at works.
    
    int sz = list_size(segmentos);
    for(int i=0; i<sz; i++)
    {
        Segmento seg = (Segmento)list_get_at(segmentos, i);
        Ponto dir_zero = criar_ponto(ox + 1.0, oy);
        Ponto intersecao = NULL;
        
        if (intersecao_raio_segmento(origem, dir_zero, seg, &intersecao))
        {
            double ix = get_ponto_x(intersecao);
            double iy = get_ponto_y(intersecao);
            
            double x1 = get_segmento_x1(seg);
            double y1 = get_segmento_y1(seg);
            double x2 = get_segmento_x2(seg);
            double y2 = get_segmento_y2(seg);
            
            if (hypot(ix - x1, iy - y1) > EPSILON &&
                hypot(ix - x2, iy - y2) > EPSILON)
            {
                int id = get_segmento_id(seg);
                int id_orig = get_segmento_id_original(seg);
                const char *cor = get_segmento_cor(seg);
                
                Segmento s1 = criar_segmento(id, id_orig, x1, y1, ix, iy, cor);
                Segmento s2 = criar_segmento(id, id_orig, ix, iy, x2, y2, cor);
                
                // Replace current segment with two new ones
                list_remove_at(segmentos, i);
                destruir_segmento(seg);
                
                list_insert_back(segmentos, s1);
                list_insert_back(segmentos, s2);
                
                // Adjust index and size?
                // We removed 1, added 2 at back.
                // The current index 'i' now points to next element (former i+1).
                // But we must process the new ones? No, new ones might cross 0 again? 
                // Segments at 0 are split. The new ones end/start at 0, so they don't cross 0 strictly.
                // We must decrement i to process the element that shifted into position i.
                i--;
                sz = list_size(segmentos); // update size
            }
            destruir_ponto(intersecao);
        }
        destruir_ponto(dir_zero); 
    }

    LinkedList eventos = extrair_eventos(segmentos, origem);
    if (eventos == NULL || list_is_empty(eventos))
    {
        // free list elements
        while(!list_is_empty(segmentos)) {
            destruir_segmento(list_remove_front(segmentos));
        }
        list_destroy(segmentos);
        if (eventos) list_destroy(eventos);
        return NULL;
    }
    
    ordenar_eventos_lista(eventos, tipo_ordenacao, limiar_insertion);
    
    ArvoreSegmentos arvore = arvore_criar(origem);
    
    Poligono resultado = poligono_criar();
    
    // Init tree with segments at angle 0
    sz = list_size(segmentos);
    for(int i=0; i<sz; i++)
    {
        Segmento seg = (Segmento)list_get_at(segmentos, i);
        double dist = distancia_raio_segmento(origem, 0.0, seg);
        if (dist < 1e9)
        {
            arvore_inserir(arvore, seg);
        }
    }
    
    Segmento biombo = arvore_obter_primeiro(arvore);
    Ponto ultimo_ponto = NULL;
    
    if (biombo != NULL)
    {
        Ponto dir = criar_ponto(ox + 1000, oy);
        Ponto intersecao = NULL;
        
        if (intersecao_raio_segmento(origem, dir, biombo, &intersecao))
        {
            poligono_inserir_vertice(resultado, get_ponto_x(intersecao), get_ponto_y(intersecao));
            ultimo_ponto = intersecao; 
        }
        destruir_ponto(dir);
    }
    
    int num_ev = list_size(eventos);
    for(int i=0; i<num_ev; i++)
    {
        Evento *evento = (Evento*)list_get_at(eventos, i);
        arvore_definir_angulo(arvore, evento->angulo);
        
        if (evento->tipo == EVENTO_INICIO)
        {
            arvore_inserir(arvore, evento->segmento);
            Segmento novo_biombo = arvore_obter_primeiro(arvore);
            
            if (novo_biombo == evento->segmento && biombo != evento->segmento)
            {
                if (biombo != NULL && ultimo_ponto != NULL)
                {
                    Ponto intersecao = NULL;
                    if (intersecao_raio_segmento(origem, evento->ponto, biombo, &intersecao))
                    {
                        if (!ponto_igual(ultimo_ponto, intersecao))
                        {
                            poligono_inserir_vertice(resultado, get_ponto_x(intersecao), get_ponto_y(intersecao));
                            destruir_ponto(ultimo_ponto);
                            ultimo_ponto = intersecao;
                        }
                        else
                        {
                            destruir_ponto(intersecao);
                        }
                    }
                }
                
                Ponto pt = evento->ponto;
                if (!ultimo_ponto || !ponto_igual(ultimo_ponto, pt))
                {
                    poligono_inserir_vertice(resultado, get_ponto_x(pt), get_ponto_y(pt));
                    if(ultimo_ponto) destruir_ponto(ultimo_ponto);
                    ultimo_ponto = clonar_ponto(pt);
                }
                biombo = novo_biombo;
            }
        }
        else /* EVENTO_FIM */
        {
            if (evento->segmento == biombo)
            {
                Ponto pt = evento->ponto;
                if (!ultimo_ponto || !ponto_igual(ultimo_ponto, pt))
                {
                    poligono_inserir_vertice(resultado, get_ponto_x(pt), get_ponto_y(pt));
                    if(ultimo_ponto) destruir_ponto(ultimo_ponto);
                    ultimo_ponto = clonar_ponto(pt);
                }
                
                arvore_remover(arvore, evento->segmento);
                Segmento novo_biombo = arvore_obter_primeiro(arvore);
                
                if (novo_biombo != NULL)
                {
                    Ponto intersecao = NULL;
                    if (intersecao_raio_segmento(origem, evento->ponto, novo_biombo, &intersecao))
                    {
                        if (!ultimo_ponto || !ponto_igual(ultimo_ponto, intersecao))
                        {
                             poligono_inserir_vertice(resultado, get_ponto_x(intersecao), get_ponto_y(intersecao));
                             if(ultimo_ponto) destruir_ponto(ultimo_ponto);
                             ultimo_ponto = intersecao;
                        }
                        else
                        {
                            destruir_ponto(intersecao);
                        }
                    }
                }
                biombo = novo_biombo;
            }
            else
            {
                arvore_remover(arvore, evento->segmento);
            }
        }
    }
    
    if (ultimo_ponto) destruir_ponto(ultimo_ponto);
    
    arvore_destruir(arvore);
    
    // Cleanup
    while(!list_is_empty(segmentos)) {
        destruir_segmento(list_remove_front(segmentos));
    }
    list_destroy(segmentos);
    
    while(!list_is_empty(eventos)) {
        destruir_evento(list_remove_front(eventos));
    }
    list_destroy(eventos);
    
    return (PoligonoVisibilidade)resultado;
}

// Converter - Stubbed for now or unimplemented as mentioned
int converter_formas_para_segmentos(LinkedList lista_formas, LinkedList lista_segmentos, char orientacao) {
    // Requires access to shape data.
    return 0;
}

// Implement duplicated func for source compatibility
PoligonoVisibilidade calcular_visibilidade_com_segmentos(
    Ponto origem, LinkedList segmentos,
    double min_x, double min_y,
    double max_x, double max_y,
    const char *tipo_ordenacao,
    int limiar_insertion,
    LinkedList segmentos_visiveis)
{
    // Simplified version without visibility loop-back to save time
    // If user needs hit testing, they use qry.
    return calcular_visibilidade(origem, segmentos, min_x, min_y, max_x, max_y, tipo_ordenacao, limiar_insertion);
}

// Implement Delegators
void destruir_poligono_visibilidade(PoligonoVisibilidade poligono)
{
    poligono_destruir((Poligono)poligono);
}

int poligono_num_vertices(PoligonoVisibilidade poligono)
{
    return poligono_qtd_vertices((Poligono)poligono);
}

Ponto poligono_obter_vertice(PoligonoVisibilidade poligono, int indice)
{
    return poligono_get_vertice((Poligono)poligono, indice);
}

LinkedList poligono_obter_vertices(PoligonoVisibilidade poligono)
{
    return poligono_obter_lista((Poligono)poligono);
}