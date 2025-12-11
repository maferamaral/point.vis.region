#include "visibilidade.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "../tree/tree.h"
#include "../utils/lista/lista.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Definição concreta do polígono (opaco no .h)
struct PoligonoVisibilidade_st {
    LinkedList vertices; // Lista de Ponto*
    Ponto centro;
};

// Variáveis estáticas para o comparador da árvore
static Ponto g_centro;
static double g_angulo;

// Helper: encontra o menor elemento da árvore (mais à esquerda)
static void* tree_get_min(BinaryTree tree) {
    if (tree_is_empty(tree)) return NULL;
    TreeNode node = tree_get_root(tree);
    while (tree_node_left(node) != NULL) {
        node = tree_node_left(node);
    }
    return tree_node_get_data(node);
}

// Comparador da Árvore: ordena segmentos pela distância ao centro no ângulo atual
int comparar_segmentos_ativos(const void* a, const void* b) {
    Segmento* s1 = (Segmento*)a;
    Segmento* s2 = (Segmento*)b;
    if (s1 == s2) return 0;

    double d1 = distancia_raio_segmento(g_centro, g_angulo, *s1);
    double d2 = distancia_raio_segmento(g_centro, g_angulo, *s2);

    if (fabs(d1 - d2) < EPSILON) return 0;
    return (d1 < d2) ? -1 : 1;
}

typedef struct {
    double angulo;
    int tipo; // 0 = INICIO, 1 = FIM
    Segmento* seg;
    Ponto p;
} Evento;

// Ordena eventos angularmente
int comparar_eventos(const void* a, const void* b) {
    Evento* e1 = *(Evento**)a;
    Evento* e2 = *(Evento**)b;
    if (e1->angulo < e2->angulo - EPSILON) return -1;
    if (e1->angulo > e2->angulo + EPSILON) return 1;
    // Se ângulo igual, INICIO antes de FIM
    return (e1->tipo - e2->tipo);
}

PoligonoVisibilidade visibilidade_calcular(Ponto centro, LinkedList barreiras) {
    g_centro = centro;
    LinkedList vertices_poly = list_create();

    // 1. Criar Eventos (2 por segmento)
    int qtd_barreiras = list_size(barreiras);
    int qtd_eventos = qtd_barreiras * 2;
    Evento** eventos = malloc(sizeof(Evento*) * qtd_eventos);
    int k = 0;

    for (int i = 0; i < qtd_barreiras; i++) {
        Segmento* seg = (Segmento*)list_get_at(barreiras, i);
        double ang1 = angulo_polar(centro, seg->p1);
        double ang2 = angulo_polar(centro, seg->p2);

        // Define ordem angular simples (-PI a PI)
        // Correção para cruzar o eixo -PI (não implementada aqui para simplificar, 
        // assume-se que o "biombo" mitiga problemas de borda)
        
        bool p1_primeiro = (ang1 < ang2);
        if (fabs(ang1 - ang2) > M_PI) p1_primeiro = !p1_primeiro; // Corrige volta completa

        eventos[k] = malloc(sizeof(Evento));
        eventos[k]->angulo = ang1;
        eventos[k]->seg = seg;
        eventos[k]->p = seg->p1;
        eventos[k]->tipo = p1_primeiro ? 0 : 1;
        k++;

        eventos[k] = malloc(sizeof(Evento));
        eventos[k]->angulo = ang2;
        eventos[k]->seg = seg;
        eventos[k]->p = seg->p2;
        eventos[k]->tipo = p1_primeiro ? 1 : 0;
        k++;
    }

    qsort(eventos, qtd_eventos, sizeof(Evento*), comparar_eventos);

    // 2. Varredura
    BinaryTree ativos = tree_create(comparar_segmentos_ativos);
    
    for (int i = 0; i < qtd_eventos; i++) {
        Evento* e = eventos[i];
        g_angulo = e->angulo;

        // Atualiza árvore
        if (e->tipo == 0) tree_insert(ativos, e->seg);
        else tree_remove(ativos, e->seg);

        // Quem é o mais próximo agora?
        Segmento* mais_prox = (Segmento*)tree_get_min(ativos);

        if (mais_prox) {
            // Intersecção do raio atual com o segmento mais próximo
            Ponto impacto = interseccao_raio_segmento(centro, e->angulo, *mais_prox);
            if (!isnan(impacto.x)) {
                Ponto* p = malloc(sizeof(Ponto));
                *p = impacto;
                list_insert_back(vertices_poly, p);
            }
        }
    }

    // Limpeza
    for(int i=0; i<qtd_eventos; i++) free(eventos[i]);
    free(eventos);
    tree_destroy(ativos, NULL);

    PoligonoVisibilidade poly = malloc(sizeof(struct PoligonoVisibilidade_st));
    poly->vertices = vertices_poly;
    poly->centro = centro;
    return poly;
}

bool visibilidade_ponto_atingido(PoligonoVisibilidade pol, Ponto p) {
    if (!pol) return false;
    
    // Verifica se a distância até P é menor que a distância até a borda do polígono no mesmo ângulo
    double ang = angulo_polar(pol->centro, p);
    double dist_p = distancia(pol->centro, p);
    
    // Procura aresta que intercepta o ângulo
    int n = list_size(pol->vertices);
    for (int i = 0; i < n; i++) {
        Ponto* v1 = (Ponto*)list_get_at(pol->vertices, i);
        Ponto* v2 = (Ponto*)list_get_at(pol->vertices, (i + 1) % n);
        
        // Verifica se o raio no ângulo 'ang' bate nesta aresta (v1-v2)
        Segmento aresta = {*v1, *v2};
        Ponto inter = interseccao_raio_segmento(pol->centro, ang, aresta);
        
        if (!isnan(inter.x)) {
            // Se encontrou intersecção, compara distâncias
            double dist_borda = distancia(pol->centro, inter);
            // Se o ponto está mais perto que a parede de luz, foi atingido
            if (dist_p <= dist_borda + EPSILON) return true;
        }
    }
    return false;
}

void visibilidade_destruir(PoligonoVisibilidade pol) {
    if (!pol) return;
    // Libera todos os pontos da lista
    while (!list_is_empty(pol->vertices)) {
        free(list_remove_front(pol->vertices));
    }
    list_destroy(pol->vertices);
    free(pol);
}