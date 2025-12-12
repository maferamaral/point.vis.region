#include "visibilidade.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include "../arvore_seg/arvore_seg.h"
#include "../utils/lista/lista.h"
#include "../utils/sort/sort.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define EPSILON 1e-9
#define MARGEM_BBOX 5.0

// Estrutura opaca do polígono
struct PoligonoVisibilidade_st
{
    LinkedList vertices;
    Ponto centro;
};

// Método de ordenação
static char g_sort_method = 'q';

void visibilidade_set_sort_method(char method)
{
    g_sort_method = method;
}

// ============================================================================
// Estruturas Internas
// ============================================================================

typedef enum {
    EVENTO_INICIO = 0,
    EVENTO_FIM = 1
} TipoEvento;

typedef struct {
    Ponto p;
    double angulo;
    double distancia;
    TipoEvento tipo;
    Segmento *seg;
} Evento;

// Compara pontos com tolerância (mais relaxada para evitar vertices duplicados vizinhos)
static bool ponto_igual(Ponto a, Ponto b)
{
    // Tolerância maior para agrupar pontos muito próximos
    const double TOLERANCIA_PONTO = 10.0;
    return fabs(a.x - b.x) < TOLERANCIA_PONTO && fabs(a.y - b.y) < TOLERANCIA_PONTO;
}

// Compara eventos para ordenação
static int comparar_eventos(const void *a, const void *b)
{
    Evento *e1 = *(Evento **)a;
    Evento *e2 = *(Evento **)b;

    // 1. Por ângulo
    if (fabs(e1->angulo - e2->angulo) > EPSILON)
    {
        return (e1->angulo < e2->angulo) ? -1 : 1;
    }

    // 2. INICIO antes de FIM
    if (e1->tipo != e2->tipo)
    {
        return (e1->tipo == EVENTO_INICIO) ? -1 : 1;
    }

    // 3. Por distância (mais perto primeiro)
    if (fabs(e1->distancia - e2->distancia) > EPSILON)
    {
        return (e1->distancia < e2->distancia) ? -1 : 1;
    }

    return 0;
}

// Verifica se segmento cruza o raio horizontal (ângulo 0)
static bool segmento_cruza_angulo_zero(Ponto centro, Segmento *seg)
{
    double ang1 = angulo_polar(centro, seg->p1);
    double ang2 = angulo_polar(centro, seg->p2);
    
    // Normaliza para [0, 2PI]
    if (ang1 < 0) ang1 += 2 * M_PI;
    if (ang2 < 0) ang2 += 2 * M_PI;
    
    // Segmento cruza ângulo 0 se: um ângulo < epsilon E outro > PI
    // Ou se ambos são muito pequenos
    if ((ang1 < EPSILON && ang2 > M_PI) || (ang2 < EPSILON && ang1 > M_PI))
        return true;
    
    // Ou se a distância no ângulo 0 é finita
    double d = distancia_raio_segmento(centro, 0.0, *seg);
    return !isinf(d) && !isnan(d) && d < 1e9;
}

// Insere vértice apenas se diferente do último
static Ponto* inserir_vertice_unico(LinkedList vertices, Ponto novo, Ponto *ultimo)
{
    if (ultimo != NULL && ponto_igual(*ultimo, novo))
    {
        return ultimo;
    }
    
    Ponto *p = malloc(sizeof(Ponto));
    *p = novo;
    list_insert_back(vertices, p);
    return p;
}

// ============================================================================
// Função Principal de Visibilidade
// ============================================================================

PoligonoVisibilidade visibilidade_calcular(Ponto centro, LinkedList barreiras_input)
{
    LinkedList vertices_poly = list_create();

    if (list_size(barreiras_input) == 0)
    {
        struct PoligonoVisibilidade_st *poly = malloc(sizeof(struct PoligonoVisibilidade_st));
        poly->vertices = vertices_poly;
        poly->centro = centro;
        return (PoligonoVisibilidade)poly;
    }

    // ========================================================================
    // FASE 1: Criar cópia dos segmentos e adicionar bounding box
    // ========================================================================
    LinkedList barreiras = list_create();
    
    double min_x = 1e9, min_y = 1e9, max_x = -1e9, max_y = -1e9;
    
    for (int i = 0; i < list_size(barreiras_input); i++)
    {
        Segmento *s = (Segmento *)list_get_at(barreiras_input, i);
        Segmento *copia = malloc(sizeof(Segmento));
        *copia = *s;
        list_insert_back(barreiras, copia);
        
        // Atualiza bounding box
        if (s->p1.x < min_x) min_x = s->p1.x;
        if (s->p1.y < min_y) min_y = s->p1.y;
        if (s->p1.x > max_x) max_x = s->p1.x;
        if (s->p1.y > max_y) max_y = s->p1.y;
        if (s->p2.x < min_x) min_x = s->p2.x;
        if (s->p2.y < min_y) min_y = s->p2.y;
        if (s->p2.x > max_x) max_x = s->p2.x;
        if (s->p2.y > max_y) max_y = s->p2.y;
    }
    
    // Expande para incluir o centro
    if (centro.x < min_x) min_x = centro.x;
    if (centro.y < min_y) min_y = centro.y;
    if (centro.x > max_x) max_x = centro.x;
    if (centro.y > max_y) max_y = centro.y;
    
    // Adiciona margem e cria bounding box
    min_x -= MARGEM_BBOX;
    min_y -= MARGEM_BBOX;
    max_x += MARGEM_BBOX;
    max_y += MARGEM_BBOX;
    
    // 4 segmentos da bounding box
    Segmento *bbox[4];
    bbox[0] = malloc(sizeof(Segmento)); bbox[0]->p1 = (Ponto){min_x, min_y}; bbox[0]->p2 = (Ponto){max_x, min_y}; bbox[0]->id = -1;
    bbox[1] = malloc(sizeof(Segmento)); bbox[1]->p1 = (Ponto){max_x, min_y}; bbox[1]->p2 = (Ponto){max_x, max_y}; bbox[1]->id = -2;
    bbox[2] = malloc(sizeof(Segmento)); bbox[2]->p1 = (Ponto){max_x, max_y}; bbox[2]->p2 = (Ponto){min_x, max_y}; bbox[2]->id = -3;
    bbox[3] = malloc(sizeof(Segmento)); bbox[3]->p1 = (Ponto){min_x, max_y}; bbox[3]->p2 = (Ponto){min_x, min_y}; bbox[3]->id = -4;
    
    for (int i = 0; i < 4; i++) list_insert_back(barreiras, bbox[i]);

    // ========================================================================
    // FASE 1.5: PRÉ-PROCESSAMENTO - Divisão de Segmentos no Ângulo 0
    // Segmentos que cruzam o raio horizontal (ângulo 0) devem ser divididos
    // para evitar inconsistências na ordenação angular
    // ========================================================================
    int n_barreiras = list_size(barreiras);
    for (int i = 0; i < n_barreiras; i++)
    {
        Segmento *seg = (Segmento *)list_get_at(barreiras, i);
        
        // Cria um raio horizontal para a direita a partir do centro
        Segmento raio_zero;
        raio_zero.p1 = centro;
        raio_zero.p2 = (Ponto){centro.x + 10000.0, centro.y};
        
        // Verifica se o segmento intercepta o raio horizontal
        Ponto inter = interseccao_raio_segmento(centro, 0.0, *seg);
        
        if (!isnan(inter.x))
        {
            // Verifica se a interseção não está nos extremos do segmento
            double d1 = distancia(inter, seg->p1);
            double d2 = distancia(inter, seg->p2);
            
            if (d1 > EPSILON && d2 > EPSILON)
            {
                // Divide o segmento em dois no ponto de interseção
                Segmento *s1 = malloc(sizeof(Segmento));
                Segmento *s2 = malloc(sizeof(Segmento));
                
                s1->p1 = seg->p1;
                s1->p2 = inter;
                s1->id = seg->id;
                
                s2->p1 = inter;
                s2->p2 = seg->p2;
                s2->id = seg->id;
                
                // Adiciona os dois novos segmentos
                list_insert_back(barreiras, s1);
                list_insert_back(barreiras, s2);
                
                // Marca o segmento original para remoção (ID inválido)
                seg->id = -9999;
            }
        }
    }
    
    // Remove segmentos marcados para remoção
    LinkedList barreiras_limpas = list_create();
    while (!list_is_empty(barreiras))
    {
        Segmento *seg = (Segmento *)list_remove_front(barreiras);
        if (seg->id != -9999)
        {
            list_insert_back(barreiras_limpas, seg);
        }
        else
        {
            free(seg);
        }
    }
    list_destroy(barreiras);
    barreiras = barreiras_limpas;

    // ========================================================================
    // FASE 2: Criar eventos (vértices) com tipo INICIO/FIM
    // ========================================================================
    int qtd_barreiras = list_size(barreiras);
    int qtd_eventos = qtd_barreiras * 2;
    Evento **eventos = malloc(sizeof(Evento *) * qtd_eventos);
    int k = 0;

    for (int i = 0; i < qtd_barreiras; i++)
    {
        Segmento *seg = (Segmento *)list_get_at(barreiras, i);
        
        double ang1 = angulo_polar(centro, seg->p1);
        double ang2 = angulo_polar(centro, seg->p2);
        double dist1 = distancia(centro, seg->p1);
        double dist2 = distancia(centro, seg->p2);

        // Normaliza para [0, 2PI]
        if (ang1 < 0) ang1 += 2 * M_PI;
        if (ang2 < 0) ang2 += 2 * M_PI;

        // Determina qual ponto é INICIO e qual é FIM
        bool p1_inicio = (ang1 < ang2) || (fabs(ang1 - ang2) < EPSILON && dist1 < dist2);

        eventos[k] = malloc(sizeof(Evento));
        eventos[k]->angulo = ang1;
        eventos[k]->distancia = dist1;
        eventos[k]->seg = seg;
        eventos[k]->p = seg->p1;
        eventos[k]->tipo = p1_inicio ? EVENTO_INICIO : EVENTO_FIM;
        k++;

        eventos[k] = malloc(sizeof(Evento));
        eventos[k]->angulo = ang2;
        eventos[k]->distancia = dist2;
        eventos[k]->seg = seg;
        eventos[k]->p = seg->p2;
        eventos[k]->tipo = p1_inicio ? EVENTO_FIM : EVENTO_INICIO;
        k++;
    }

    // Ordena eventos
    sort(eventos, qtd_eventos, sizeof(Evento *), comparar_eventos, g_sort_method, 10);

    // ========================================================================
    // FASE 3: Inicializa árvore com segmentos que cruzam ângulo 0
    // ========================================================================
    ArvoreSegmentos ativos = arvore_seg_criar(centro);
    
    arvore_seg_definir_angulo(ativos, 0.0);
    for (int i = 0; i < qtd_barreiras; i++)
    {
        Segmento *seg = (Segmento *)list_get_at(barreiras, i);
        if (segmento_cruza_angulo_zero(centro, seg))
        {
            arvore_seg_inserir(ativos, seg);
        }
    }

    // Ponto inicial do polígono (interseção do raio 0 com biombo inicial)
    Segmento *biombo_atual = arvore_seg_obter_primeiro(ativos);
    Ponto *ultimo_ponto = NULL;
    
    if (biombo_atual != NULL)
    {
        Ponto inter = interseccao_raio_segmento(centro, 0.0, *biombo_atual);
        if (!isnan(inter.x))
        {
            ultimo_ponto = inserir_vertice_unico(vertices_poly, inter, ultimo_ponto);
        }
    }

    // ========================================================================
    // FASE 4: Loop principal de varredura
    // ========================================================================
    for (int i = 0; i < qtd_eventos; i++)
    {
        Evento *e = eventos[i];
        arvore_seg_definir_angulo(ativos, e->angulo);

        if (e->tipo == EVENTO_INICIO)
        {
            arvore_seg_inserir(ativos, e->seg);
            Segmento *mais_prox = arvore_seg_obter_primeiro(ativos);

            // Se o segmento inserido virou o mais próximo
            if (mais_prox == e->seg && biombo_atual != e->seg)
            {
                // Calcula interseção com o biombo anterior
                if (biombo_atual != NULL)
                {
                    Ponto inter = interseccao_raio_segmento(centro, e->angulo, *biombo_atual);
                    if (!isnan(inter.x))
                    {
                        ultimo_ponto = inserir_vertice_unico(vertices_poly, inter, ultimo_ponto);
                    }
                }
                
                // Adiciona o vértice do evento
                ultimo_ponto = inserir_vertice_unico(vertices_poly, e->p, ultimo_ponto);
                biombo_atual = e->seg;
            }
        }
        else // EVENTO_FIM
        {
            if (e->seg == biombo_atual)
            {
                // Adiciona o vértice do evento
                ultimo_ponto = inserir_vertice_unico(vertices_poly, e->p, ultimo_ponto);

                arvore_seg_remover(ativos, e->seg);

                // Busca o novo biombo
                Segmento *novo_prox = arvore_seg_obter_primeiro(ativos);
                if (novo_prox != NULL)
                {
                    Ponto inter = interseccao_raio_segmento(centro, e->angulo, *novo_prox);
                    if (!isnan(inter.x))
                    {
                        ultimo_ponto = inserir_vertice_unico(vertices_poly, inter, ultimo_ponto);
                    }
                }
                biombo_atual = novo_prox;
            }
            else
            {
                arvore_seg_remover(ativos, e->seg);
            }
        }
    }

    // ========================================================================
    // FASE 5: Limpeza e retorno
    // ========================================================================
    for (int i = 0; i < qtd_eventos; i++) free(eventos[i]);
    free(eventos);
    
    // Libera segmentos copiados
    while (!list_is_empty(barreiras))
    {
        free(list_remove_front(barreiras));
    }
    list_destroy(barreiras);
    
    arvore_seg_destruir(ativos);

    struct PoligonoVisibilidade_st *poly = malloc(sizeof(struct PoligonoVisibilidade_st));
    poly->vertices = vertices_poly;
    poly->centro = centro;
    return (PoligonoVisibilidade)poly;
}

// ============================================================================
// Funções de Teste de Visibilidade
// ============================================================================

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

        Segmento aresta = {*v1, *v2, 0};
        
        double d_borda = distancia_raio_segmento(pol->centro, ang, aresta);

        if (!isinf(d_borda) && !isnan(d_borda))
        {
            // Ponto está dentro se está antes da borda
            if (dist_p <= d_borda + 0.5)
                return true;
        }
    }
    return false;
}

bool visibilidade_segmento_atingido(PoligonoVisibilidade pol_opaco, Ponto p1, Ponto p2)
{
    if (!pol_opaco) return false;
    struct PoligonoVisibilidade_st *pol = (struct PoligonoVisibilidade_st *)pol_opaco;
    
    // Teste 1: Verifica se alguma extremidade está dentro
    if (visibilidade_ponto_atingido(pol_opaco, p1)) return true;
    if (visibilidade_ponto_atingido(pol_opaco, p2)) return true;
    
    // Teste 2: Verifica ponto médio
    Ponto pm = {(p1.x + p2.x) / 2, (p1.y + p2.y) / 2};
    if (visibilidade_ponto_atingido(pol_opaco, pm)) return true;
    
    // Teste 3: Verifica se o segmento intercepta alguma aresta do polígono
    Segmento seg = {p1, p2, 0};
    int n = list_size(pol->vertices);
    
    for (int i = 0; i < n; i++)
    {
        Ponto *v1 = (Ponto *)list_get_at(pol->vertices, i);
        Ponto *v2 = (Ponto *)list_get_at(pol->vertices, (i + 1) % n);
        
        Segmento aresta = {*v1, *v2, 0};
        
        if (segmentos_intersectam(seg, aresta))
        {
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

// Retorna os vértices do polígono para desenho SVG
LinkedList visibilidade_obter_vertices(PoligonoVisibilidade pol_opaco)
{
    if (!pol_opaco) return NULL;
    struct PoligonoVisibilidade_st *pol = (struct PoligonoVisibilidade_st *)pol_opaco;
    return pol->vertices;
}

Ponto visibilidade_obter_centro(PoligonoVisibilidade pol_opaco)
{
    struct PoligonoVisibilidade_st *pol = (struct PoligonoVisibilidade_st *)pol_opaco;
    return pol->centro;
}