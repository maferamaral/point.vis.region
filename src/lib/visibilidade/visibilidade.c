#include "visibilidade.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "../tree/tree.h" // Sua árvore genérica
#include "../utils/lista/lista.h"

// Variáveis globais para o comparador da árvore (estado da varredura)
static Ponto g_centro;
static double g_angulo;

// Comparador para a Árvore de Segmentos Ativos
// Retorna -1 se A está mais perto que B, 1 se mais longe.
int comparar_segmentos_tree(const void *a, const void *b)
{
    Segmento *s1 = (Segmento *)a;
    Segmento *s2 = (Segmento *)b;

    // Se forem o mesmo ponteiro, são iguais
    if (s1 == s2)
        return 0;

    double d1 = distancia_raio_segmento(g_centro, g_angulo, *s1);
    double d2 = distancia_raio_segmento(g_centro, g_angulo, *s2);

    if (fabs(d1 - d2) < EPSILON)
        return 0;
    return (d1 < d2) ? -1 : 1;
}

// Estrutura interna de Evento
typedef struct
{
    double angulo;
    int tipo; // 0 = INICIO (Inserir), 1 = FIM (Remover)
    Segmento *seg;
} Evento;

// Comparador para o qsort dos eventos
int comparar_eventos(const void *a, const void *b)
{
    Evento *e1 = *(Evento **)a;
    Evento *e2 = *(Evento **)b;

    if (e1->angulo < e2->angulo - EPSILON)
        return -1;
    if (e1->angulo > e2->angulo + EPSILON)
        return 1;

    // Se ângulos iguais, processa INICIO antes de FIM para não ficar "sem parede" por um instante
    return (e1->tipo - e2->tipo);
}

PoligonoVisibilidade visibilidade_calcular(Ponto centro, LinkedList barreiras)
{
    g_centro = centro; // Configura contexto global

    // 1. Criar Eventos
    // Cada segmento gera 2 eventos. A ordem (p1 vs p2) depende do ângulo.
    // Precisamos normalizar ângulos entre -PI e PI ou 0 e 2PI.
    // Aqui usamos atan2 direto (-PI a PI).
    
    int qtd_barreiras = list_size(barreiras);
    int qtd_eventos = qtd_barreiras * 2;
    Evento **eventos = malloc(sizeof(Evento *) * qtd_eventos);
    int k = 0;

    for (int idx = 0; idx < qtd_barreiras; idx++)
    {
        Segmento *seg = (Segmento *)list_get_at(barreiras, idx);
        double ang1 = angulo_polar(centro, seg->p1);
        double ang2 = angulo_polar(centro, seg->p2);

        // Evento 1
        eventos[k] = malloc(sizeof(Evento));
        eventos[k]->angulo = ang1;
        eventos[k]->seg = seg;
        eventos[k]->tipo = 0; // Tenta inserir
        k++;

        // Evento 2
        eventos[k] = malloc(sizeof(Evento));
        eventos[k]->angulo = ang2;
        eventos[k]->seg = seg;
        eventos[k]->tipo = 1; // Tenta remover
        k++;
    }

    // Ordenar eventos
    qsort(eventos, qtd_eventos, sizeof(Evento *), comparar_eventos);

    // 2. Varredura
    BinaryTree ativos = tree_create(comparar_segmentos_tree);
    
    // Instanciar Poligono usando TAD opaco
    PoligonoVisibilidade poly = poligono_criar(centro);

    for (int i = 0; i < qtd_eventos; i++)
    {
        Evento *e = eventos[i];
        g_angulo = e->angulo; // Atualiza ângulo atual

        // Processa evento simplificado
        tree_insert(ativos, e->seg);

        // Consultamos o mais próximo
        Segmento *seg_mais_prox = NULL;
        TreeNode root = tree_get_root(ativos);
        TreeNode cur = root;
        while (cur && tree_node_left(cur))
            cur = tree_node_left(cur);
        if (cur)
            seg_mais_prox = (Segmento *)tree_node_get_data(cur);

        if (seg_mais_prox)
        {
            Ponto impacto = interseccao_raio_segmento(centro, e->angulo, *seg_mais_prox);
            if (!isnan(impacto.x))
            {
                poligono_adicionar_vertice(poly, impacto);
            }
        }
        
        // Remoção simplificada não implementada robustamente no exemplo original, mantemos assim
    }

    // Limpeza temporária
    for (int i = 0; i < qtd_eventos; i++)
        free(eventos[i]);
    free(eventos);
    // tree_destroy(ativos); // Nota: implementar destrutor da arvore quando possivel

    return poly;
}

bool visibilidade_ponto_atingido(PoligonoVisibilidade pol, Ponto p)
{
    if (!pol)
        return false;

    Ponto centro = poligono_get_centro(pol);
    double ang = angulo_polar(centro, p);
    double dist_p = distancia(centro, p);

    // Encontrar a aresta do polígono que cobre este ângulo
    LinkedList vertices = poligono_get_vertices(pol);
    int n = list_size(vertices);
    if (n == 0)
        return false;

    for (int i = 0; i < n; i++)
    {
        Ponto *v1 = (Ponto *)list_get_at(vertices, i);
        Ponto *v2 = (Ponto *)list_get_at(vertices, (i + 1) % n);

        Segmento aresta = {*v1, *v2};
        Ponto inter = interseccao_raio_segmento(centro, ang, aresta);

        if (!isnan(inter.x))
        {
            double dist_borda = distancia(centro, inter);
            // Se o ponto está mais perto que a borda (ou na borda), foi atingido
            if (dist_p <= dist_borda + EPSILON)
                return true;
        }
    }

    return false;
}

void visibilidade_destruir_poly(PoligonoVisibilidade pol)
{
    poligono_destruir(pol);
}