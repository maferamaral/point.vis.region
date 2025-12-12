#include "visibilidade.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "../tree/tree.h"
#include "../utils/lista/lista.h"
#include "../utils/sort/sort.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Estrutura opaca do polígono
struct PoligonoVisibilidade_st
{
    LinkedList vertices;
    Ponto centro;
};

static Ponto g_centro;
static double g_angulo;
static char g_sort_method = 'q';

void visibilidade_set_sort_method(char method)
{
    g_sort_method = method;
}

static void *tree_get_min(BinaryTree tree)
{
    if (tree_is_empty(tree))
        return NULL;
    TreeNode node = tree_get_root(tree);
    while (tree_node_left(node) != NULL)
    {
        node = tree_node_left(node);
    }
    return tree_node_get_data(node);
}

// --- FUNÇÃO DE DISTÂNCIA ROBUSTA (CORREÇÃO DA COLINEARIDADE) ---
// Resolve o problema da luz vazando quando o raio está alinhado com a parede
static double distancia_raio_segmento_robusta(Ponto origem, double angulo, Segmento seg)
{
    // Tenta cálculo padrão
    double d = distancia_raio_segmento(origem, angulo, seg);
    
    // Se falhou (infinito/NaN), pode ser colinearidade
    if (isinf(d) || isnan(d)) {
        // Verifica se os pontos do segmento estão alinhados com o ângulo
        double ang1 = angulo_polar(origem, seg.p1);
        double ang2 = angulo_polar(origem, seg.p2);
        
        // Normaliza
        if (ang1 < 0) ang1 += 2 * M_PI;
        if (ang2 < 0) ang2 += 2 * M_PI;
        if (angulo < 0) angulo += 2 * M_PI;
        
        // Se algum dos pontos estiver no ângulo do raio (com tolerância)
        // a distância é a desse ponto.
        double diff1 = fabs(ang1 - angulo);
        double diff2 = fabs(ang2 - angulo);
        if (diff1 > M_PI) diff1 = 2*M_PI - diff1;
        if (diff2 > M_PI) diff2 = 2*M_PI - diff2;
        
        double tol = 1e-4;
        
        // Se ambos alinhados (colinear radial), retorna o mais próximo
        if (diff1 < tol && diff2 < tol) {
             return min_d(distancia(origem, seg.p1), distancia(origem, seg.p2));
        }
        if (diff1 < tol) return distancia(origem, seg.p1);
        if (diff2 < tol) return distancia(origem, seg.p2);
    }
    
    return d;
}

int comparar_segmentos_ativos(const void *a, const void *b)
{
    Segmento *s1 = (Segmento *)a;
    Segmento *s2 = (Segmento *)b;
    
    if (s1 == s2) return 0;

    // Usa a versão robusta para não ignorar paredes alinhadas
    double d1 = distancia_raio_segmento_robusta(g_centro, g_angulo, *s1);
    double d2 = distancia_raio_segmento_robusta(g_centro, g_angulo, *s2);

    if (fabs(d1 - d2) < EPSILON) {
        // Desempate estável por endereço
        return (s1 < s2) ? -1 : 1;
    }
    return (d1 < d2) ? -1 : 1;
}

typedef struct
{
    double angulo;
    int tipo;
    Segmento *seg;
    Ponto p;
} Evento;

int comparar_eventos(const void *a, const void *b)
{
    Evento *e1 = *(Evento **)a;
    Evento *e2 = *(Evento **)b;

    if (e1->angulo < e2->angulo - EPSILON) return -1;
    if (e1->angulo > e2->angulo + EPSILON) return 1;

    if (e1->tipo != e2->tipo)
    {
        return (e1->tipo == 0) ? -1 : 1; // INICIO antes de FIM
    }

    double d1 = distancia(g_centro, e1->p);
    double d2 = distancia(g_centro, e2->p);
    if (fabs(d1 - d2) < EPSILON) return 0;
    return (d1 < d2) ? -1 : 1;
}

static LinkedList preparar_segmentos(Ponto centro, LinkedList barreiras_originais)
{
    LinkedList processados = list_create();
    int n = list_size(barreiras_originais);

    for (int i = 0; i < n; i++)
    {
        Segmento *s = (Segmento *)list_get_at(barreiras_originais, i);

        double ang1 = angulo_polar(centro, s->p1);
        double ang2 = angulo_polar(centro, s->p2);

        if (ang1 < 0) ang1 += 2 * M_PI;
        if (ang2 < 0) ang2 += 2 * M_PI;

        if (fabs(ang1 - ang2) > M_PI)
        {
            Ponto inter = interseccao_raio_segmento(centro, 0.0, *s);

            if (!isnan(inter.x))
            {
                Segmento *s1 = malloc(sizeof(Segmento));
                Segmento *s2 = malloc(sizeof(Segmento));

                *s1 = segmento_criar(s->p1, inter);
                *s2 = segmento_criar(inter, s->p2);

                list_insert_back(processados, s1);
                list_insert_back(processados, s2);
                continue;
            }
        }

        Segmento *copia = malloc(sizeof(Segmento));
        *copia = *s;
        list_insert_back(processados, copia);
    }
    return processados;
}

PoligonoVisibilidade visibilidade_calcular(Ponto centro, LinkedList barreiras_input)
{
    g_centro = centro;
    LinkedList vertices_poly = list_create();

    LinkedList barreiras = preparar_segmentos(centro, barreiras_input);

    int qtd_barreiras = list_size(barreiras);
    int qtd_eventos = qtd_barreiras * 2;
    Evento **eventos = malloc(sizeof(Evento *) * qtd_eventos);
    int k = 0;

    for (int i = 0; i < qtd_barreiras; i++)
    {
        Segmento *seg = (Segmento *)list_get_at(barreiras, i);
        double ang1 = angulo_polar(centro, seg->p1);
        double ang2 = angulo_polar(centro, seg->p2);

        if (ang1 < 0) ang1 += 2 * M_PI;
        if (ang2 < 0) ang2 += 2 * M_PI;

        bool p1_inicio = (ang1 < ang2);

        eventos[k] = malloc(sizeof(Evento));
        eventos[k]->angulo = ang1;
        eventos[k]->seg = seg;
        eventos[k]->p = seg->p1;
        eventos[k]->tipo = p1_inicio ? 0 : 1;
        k++;

        eventos[k] = malloc(sizeof(Evento));
        eventos[k]->angulo = ang2;
        eventos[k]->seg = seg;
        eventos[k]->p = seg->p2;
        eventos[k]->tipo = p1_inicio ? 1 : 0;
        k++;
    }

    sort(eventos, qtd_eventos, sizeof(Evento *), comparar_eventos, g_sort_method, 10);

    BinaryTree ativos = tree_create(comparar_segmentos_ativos);
    Segmento *biombo_atual = NULL;

    if (qtd_eventos > 0) g_angulo = eventos[0]->angulo;

    for (int i = 0; i < qtd_eventos; i++)
    {
        Evento *e = eventos[i];
        g_angulo = e->angulo;

        if (e->tipo == 0) // INICIO
        {
            tree_insert(ativos, e->seg);
            Segmento *mais_prox = (Segmento *)tree_get_min(ativos);

            if (mais_prox == e->seg)
            {
                if (biombo_atual && biombo_atual != e->seg)
                {
                    Ponto inter = interseccao_raio_segmento(centro, e->angulo, *biombo_atual);
                    
                    if (isnan(inter.x)) {
                       // Fallback colinearidade: usar vértice do evento
                       inter = e->p; 
                    }

                    if (!isnan(inter.x))
                    {
                        Ponto *p = malloc(sizeof(Ponto));
                        *p = inter;
                        list_insert_back(vertices_poly, p);
                    }
                }
                Ponto *p = malloc(sizeof(Ponto));
                *p = e->p;
                list_insert_back(vertices_poly, p);
                biombo_atual = e->seg;
            }
        }
        else // FIM
        {
            if (e->seg == biombo_atual)
            {
                Ponto *p = malloc(sizeof(Ponto));
                *p = e->p;
                list_insert_back(vertices_poly, p);

                tree_remove(ativos, e->seg);

                Segmento *novo_prox = (Segmento *)tree_get_min(ativos);
                if (novo_prox)
                {
                    Ponto inter = interseccao_raio_segmento(centro, e->angulo, *novo_prox);
                    
                    if (isnan(inter.x)) {
                         inter = e->p;
                    }

                    if (!isnan(inter.x))
                    {
                        Ponto *pi = malloc(sizeof(Ponto));
                        *pi = inter;
                        list_insert_back(vertices_poly, pi);
                    }
                }
                biombo_atual = novo_prox;
            }
            else
            {
                tree_remove(ativos, e->seg);
            }
        }
    }

    for (int i = 0; i < qtd_eventos; i++) free(eventos[i]);
    free(eventos);
    tree_destroy(ativos, NULL);

    while (!list_is_empty(barreiras)) free(list_remove_front(barreiras));
    list_destroy(barreiras);

    struct PoligonoVisibilidade_st *poly = malloc(sizeof(struct PoligonoVisibilidade_st));
    poly->vertices = vertices_poly;
    poly->centro = centro;
    return (PoligonoVisibilidade)poly;
}

bool visibilidade_ponto_atingido(PoligonoVisibilidade pol_opaco, Ponto p)
{
    if (!pol_opaco) return false;
    struct PoligonoVisibilidade_st *pol = (struct PoligonoVisibilidade_st *)pol_opaco;
    
    double dist_p = distancia(pol->centro, p);
    double ang = angulo_polar(pol->centro, p);
    if (ang < 0) ang += 2 * M_PI;

    int n = list_size(pol->vertices);
    if (n < 3) return false;
    
    for (int i = 0; i < n; i++)
    {
        Ponto *v1 = (Ponto *)list_get_at(pol->vertices, i);
        Ponto *v2 = (Ponto *)list_get_at(pol->vertices, (i + 1) % n);

        Segmento aresta = {*v1, *v2};
        
        double d_borda = distancia_raio_segmento_robusta(pol->centro, ang, aresta);

        if (!isinf(d_borda) && !isnan(d_borda))
        {
            // Tolerância para garantir que bordas sejam pintadas
            if (dist_p <= d_borda + 0.1)
                return true;
        }
    }
    return false;
}

void visibilidade_destruir(PoligonoVisibilidade pol_opaco)
{
    if (!pol_opaco) return;
    struct PoligonoVisibilidade_st *pol = (struct PoligonoVisibilidade_st *)pol_opaco;
    
    while (!list_is_empty(pol->vertices))
    {
        free(list_remove_front(pol->vertices));
    }
    list_destroy(pol->vertices);
    free(pol);
}