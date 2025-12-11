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

// Definição da estrutura opaca do polígono
struct PoligonoVisibilidade_st
{
    LinkedList vertices; // Lista de Ponto*
    Ponto centro;
};

// Variáveis globais para o contexto da comparação da árvore
static Ponto g_centro;
static double g_angulo;
static char g_sort_method = 'q';

void visibilidade_set_sort_method(char method)
{
    g_sort_method = method;
}

// Auxiliar para pegar o mínimo da árvore
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

// --- CORREÇÃO PRINCIPAL: Comparador da Árvore ---
// Se as distâncias forem iguais, desempatamos pelo endereço de memória.
// Isso impede que a árvore considere segmentos diferentes como "iguais" e descarte um deles.
int comparar_segmentos_ativos(const void *a, const void *b)
{
    Segmento *s1 = (Segmento *)a;
    Segmento *s2 = (Segmento *)b;
    
    if (s1 == s2) return 0;

    double d1 = distancia_raio_segmento(g_centro, g_angulo, *s1);
    double d2 = distancia_raio_segmento(g_centro, g_angulo, *s2);

    if (fabs(d1 - d2) < EPSILON) {
        // Desempate estável por endereço
        return (s1 < s2) ? -1 : 1;
    }
    return (d1 < d2) ? -1 : 1;
}

typedef struct
{
    double angulo;
    int tipo; // 0 = INICIO, 1 = FIM
    Segmento *seg;
    Ponto p;
} Evento;

// Comparador de Eventos para o Sort
int comparar_eventos(const void *a, const void *b)
{
    Evento *e1 = *(Evento **)a;
    Evento *e2 = *(Evento **)b;

    // 1. Ângulo
    if (e1->angulo < e2->angulo - EPSILON) return -1;
    if (e1->angulo > e2->angulo + EPSILON) return 1;

    // 2. Tipo (INICIO antes de FIM para fechar buracos)
    if (e1->tipo != e2->tipo)
    {
        return (e1->tipo == 0) ? -1 : 1;
    }

    // 3. Distância (Processar o mais próximo primeiro)
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

        // Normaliza ângulos para [0, 2PI]
        if (ang1 < 0) ang1 += 2 * M_PI;
        if (ang2 < 0) ang2 += 2 * M_PI;

        // Se cruza a linha de corte (diferença > PI), divide o segmento
        if (fabs(ang1 - ang2) > M_PI)
        {
            Ponto inter = interseccao_raio_segmento(centro, 0.0, *s);

            if (!isnan(inter.x))
            {
                Segmento *s1 = malloc(sizeof(Segmento));
                Segmento *s2 = malloc(sizeof(Segmento));

                // Segmento 1: do ponto original até a intersecção (ângulo 0)
                // Segmento 2: da intersecção até o outro ponto
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

    // 1. Preparar segmentos (dividir os que cruzam o ângulo 0)
    LinkedList barreiras = preparar_segmentos(centro, barreiras_input);

    // 2. Criar Eventos
    int qtd_barreiras = list_size(barreiras);
    int qtd_eventos = qtd_barreiras * 2;
    Evento **eventos = malloc(sizeof(Evento *) * qtd_eventos);
    int k = 0;

    for (int i = 0; i < qtd_barreiras; i++)
    {
        Segmento *seg = (Segmento *)list_get_at(barreiras, i);
        double ang1 = angulo_polar(centro, seg->p1);
        double ang2 = angulo_polar(centro, seg->p2);

        // Normaliza para garantir ordem correta na criação
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

    // 3. Ordenar Eventos
    sort(eventos, qtd_eventos, sizeof(Evento *), comparar_eventos, g_sort_method, 10);

    // 4. Varredura (Sweep Line)
    BinaryTree ativos = tree_create(comparar_segmentos_ativos);
    Segmento *biombo_atual = NULL;

    // Inicializa g_angulo com o primeiro evento para inserções iniciais consistentes
    if (qtd_eventos > 0) g_angulo = eventos[0]->angulo;

    for (int i = 0; i < qtd_eventos; i++)
    {
        Evento *e = eventos[i];
        g_angulo = e->angulo; // Atualiza o "tempo" da varredura

        if (e->tipo == 0) // INICIO
        {
            tree_insert(ativos, e->seg);
            Segmento *mais_prox = (Segmento *)tree_get_min(ativos);

            // Se o novo segmento passou a ser o mais próximo (novo biombo)
            if (mais_prox == e->seg)
            {
                // Se havia um biombo anterior, fechar a visibilidade até ele
                if (biombo_atual && biombo_atual != e->seg)
                {
                    Ponto inter = interseccao_raio_segmento(centro, e->angulo, *biombo_atual);
                    if (!isnan(inter.x))
                    {
                        Ponto *p = malloc(sizeof(Ponto));
                        *p = inter;
                        list_insert_back(vertices_poly, p);
                    }
                }
                // Adiciona o vértice inicial do novo biombo
                Ponto *p = malloc(sizeof(Ponto));
                *p = e->p;
                list_insert_back(vertices_poly, p);
                biombo_atual = e->seg;
            }
        }
        else // FIM
        {
            // Se o segmento que terminou era o biombo atual
            if (e->seg == biombo_atual)
            {
                // Adiciona o vértice final
                Ponto *p = malloc(sizeof(Ponto));
                *p = e->p;
                list_insert_back(vertices_poly, p);

                tree_remove(ativos, e->seg);

                // Descobre o novo biombo (quem estava atrás)
                Segmento *novo_prox = (Segmento *)tree_get_min(ativos);
                if (novo_prox)
                {
                    Ponto inter = interseccao_raio_segmento(centro, e->angulo, *novo_prox);
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
                // Apenas remove da árvore (estava escondido)
                tree_remove(ativos, e->seg);
            }
        }
    }

    // 5. Limpeza
    for (int i = 0; i < qtd_eventos; i++) free(eventos[i]);
    free(eventos);
    tree_destroy(ativos, NULL);

    while (!list_is_empty(barreiras)) free(list_remove_front(barreiras));
    list_destroy(barreiras);

    // 6. Retorno
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
    if (ang < 0) ang += 2 * M_PI; // Normalizar para busca consistente

    int n = list_size(pol->vertices);
    if (n < 3) return false;

    // Ray casting simplificado usando a propriedade estelar do polígono de visibilidade
    // Basta checar se o ponto está mais próximo do centro do que a borda do polígono naquele ângulo
    
    // Encontrar a aresta do polígono que intercepta o raio do ângulo 'ang'
    // Como os vértices estão ordenados angularmente (natural do sweep line), 
    // podemos iterar ou fazer busca binária. Iteração é O(N).
    
    for (int i = 0; i < n; i++)
    {
        Ponto *v1 = (Ponto *)list_get_at(pol->vertices, i);
        Ponto *v2 = (Ponto *)list_get_at(pol->vertices, (i + 1) % n);

        // Verifica se o ângulo do ponto p está entre os ângulos de v1 e v2
        // Nota: isso é uma simplificação. A forma robusta é interseccao_raio_segmento
        Segmento aresta = {*v1, *v2};
        Ponto inter = interseccao_raio_segmento(pol->centro, ang, aresta);

        if (!isnan(inter.x))
        {
            double dist_borda = distancia(pol->centro, inter);
            // Se o ponto p está mais perto (ou igual) que a borda, foi atingido
            if (dist_p <= dist_borda + 1e-5) // Tolerância um pouco maior para bordas
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