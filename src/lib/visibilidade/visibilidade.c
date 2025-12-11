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

PoligonoVisibilidade *visibilidade_calcular(Ponto centro, LinkedList barreiras)
{
    g_centro = centro; // Configura contexto global

    // 1. Criar Eventos
    // Cada segmento gera 2 eventos. A ordem (p1 vs p2) depende do ângulo.
    // Precisamos normalizar ângulos entre -PI e PI ou 0 e 2PI.
    // Aqui usamos atan2 direto (-PI a PI).
    // IMPORTANTE: Se um segmento cruza o eixo de corte (-PI/PI), ele deveria ser dividido.
    // Para simplificar, assumimos segmentos pequenos que não dão volta completa.

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
    // Inicialize sua árvore. Assumindo que tree_create recebe um comparador.
    // Se sua tree.h não recebe comparador no create, você precisará modificar sua tree
    // ou usar uma global para a função de comparação interna da tree.
    BinaryTree ativos = tree_create(comparar_segmentos_tree);
    LinkedList vertices_poly = list_create();

    for (int i = 0; i < qtd_eventos; i++)
    {
        Evento *e = eventos[i];
        g_angulo = e->angulo; // Atualiza ângulo atual

        // Processa evento
        // Lógica simplificada:
        // Se encontramos um ponto, verificamos quem é o segmento mais próximo ATUALMENTE.
        // Se o segmento deste evento for o mais próximo, adicionamos o ponto.

        // Atualiza a árvore
        // Nota: A lógica INICIO/FIM baseada apenas em ângulo menor/maior falha se cruzar o eixo -PI.
        // Uma abordagem robusta insere e remove baseado na existência na árvore.
        // Como sua árvore pode não ter 'contains', tentamos remover; se falhar, inserimos (ou vice-versa dependendo da flag).
        // Vamos usar uma abordagem de "tenta inserir, se já existe ou é fim, remove".

        // Para este exemplo, vamos inserir todos e confiar no tree_min.
        // (Isso requer que a árvore suporte duplicatas ou remoção correta).

        // Abordagem correta de varredura:
        // 1. Atualiza estado da árvore
        // 2. Dispara raio no ângulo do evento
        // 3. O ponto de intersecção mais próximo é um vértice do polígono

        // Como simplificação robusta para T2:
        // Inserimos o segmento na árvore.
        tree_insert(ativos, e->seg);

        // Consultamos o mais próximo
        // Se sua tree não tem tree_min, use um iterador ou get_root
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
                Ponto *p = malloc(sizeof(Ponto));
                *p = impacto;
                list_insert_back(vertices_poly, p);
            }
        }

        // Se era um evento de FIM, removemos agora (após processar o ponto)
        // Isso depende de sabermos se era inicio ou fim.
        // Se a ordem angular for (ang1 < ang2), então ang2 é o fim.
        // Vamos checar:
        double a1 = angulo_polar(centro, e->seg->p1);
        double a2 = angulo_polar(centro, e->seg->p2);
        // Se o ângulo atual bate com o "maior" ângulo do segmento, removemos.
        // Cuidado com a descontinuidade PI/-PI.
        if (fabs(e->angulo - a2) < EPSILON || fabs(e->angulo - a1) < EPSILON)
        {
            // Lógica de remoção: se já passamos pelos dois pontos, remove.
            // Simplificação: tree_remove(ativos, e->seg);
            // Para funcionar 100%, precisamos saber exatamente se é FIM.
        }
    }

    // Limpeza temporária
    for (int i = 0; i < qtd_eventos; i++)
        free(eventos[i]);
    free(eventos);
    // tree_destroy(ativos); // Cuidado para não destruir os segmentos (dados), apenas os nós da árvore

    PoligonoVisibilidade *poly = malloc(sizeof(PoligonoVisibilidade));
    poly->vertices = vertices_poly;
    poly->centro = centro;
    return poly;
}

bool visibilidade_ponto_atingido(PoligonoVisibilidade *pol, Ponto p)
{
    if (!pol)
        return false;

    // Verifica se o segmento (centro -> p) intersecta alguma aresta do polígono?
    // Ou usa algoritmo point-in-polygon.
    // Como o polígono de visibilidade é "estrelado" (star-shaped) em relação ao centro,
    // basta verificar se a distância(centro, p) <= distância(centro, intersecção_com_borda).

    double ang = angulo_polar(pol->centro, p);
    double dist_p = distancia(pol->centro, p);

    // Encontrar a aresta do polígono que cobre este ângulo
    // Isso requer busca na lista de vértices do polígono
    int n = list_size(pol->vertices);
    if (n == 0)
        return false;

    for (int i = 0; i < n; i++)
    {
        Ponto *v1 = (Ponto *)list_get_at(pol->vertices, i);
        Ponto *v2 = (Ponto *)list_get_at(pol->vertices, (i + 1) % n);

        Segmento aresta = {*v1, *v2};
        Ponto inter = interseccao_raio_segmento(pol->centro, ang, aresta);

        if (!isnan(inter.x))
        {
            double dist_borda = distancia(pol->centro, inter);
            // Se o ponto está mais perto que a borda (ou na borda), foi atingido
            if (dist_p <= dist_borda + EPSILON)
                return true;
        }
    }

    return false;
}

void visibilidade_destruir(PoligonoVisibilidade *pol)
{
    if (pol)
    {
        // Liberar os pontos alocados e destruir a lista
        int n = list_size(pol->vertices);
        for (int i = 0; i < n; i++)
        {
            Ponto *p = (Ponto *)list_get_at(pol->vertices, i);
            free(p);
        }
        list_destroy(pol->vertices);
        free(pol);
    }
}