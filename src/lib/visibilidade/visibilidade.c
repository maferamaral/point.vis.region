/**
 * visibilidade.c
 * 
 * Implementação do algoritmo de visibilidade por varredura angular.
 * Segue fielmente a especificação do professor (visibilidade.md).
 */

#include "visibilidade.h"
#include "../arvore_seg/arvore_seg.h"
#include "../utils/lista/lista.h"
#include "../utils/sort/sort.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define EPSILON 1e-9
#define MARGEM_BBOX 10.0

/* Método de ordenação global */
static char g_sort_method = 'q';

void visibilidade_set_sort_method(char method)
{
    g_sort_method = method;
}

/* ============================================================================
 * Estrutura de Evento
 * ============================================================================ */

typedef enum {
    EVENTO_INICIO = 0,
    EVENTO_FIM = 1
} TipoEvento;

typedef struct {
    Ponto p;           // Ponto do evento (vértice do segmento)
    double angulo;     // Ângulo polar em relação ao centro
    double distancia;  // Distância ao centro
    TipoEvento tipo;   // INICIO ou FIM
    Segmento *seg;     // Ponteiro para o segmento associado
} Evento;

/* ============================================================================
 * Funções Auxiliares
 * ============================================================================ */

/**
 * Compara dois pontos para verificar se são iguais (com tolerância).
 */
static bool pontos_iguais(Ponto a, Ponto b)
{
    return fabs(a.x - b.x) < EPSILON && fabs(a.y - b.y) < EPSILON;
}

/**
 * Compara eventos para ordenação.
 * Critérios (do professor):
 * 1. Menor ângulo
 * 2. Menor distância ao centro (processar oclusões próximas primeiro)
 * 3. INICIO antes de FIM (se ângulos idênticos)
 */
static int comparar_eventos(const void *a, const void *b)
{
    Evento *e1 = *(Evento **)a;
    Evento *e2 = *(Evento **)b;

    // 1. Por ângulo
    if (fabs(e1->angulo - e2->angulo) > EPSILON)
    {
        return (e1->angulo < e2->angulo) ? -1 : 1;
    }

    // 2. Por distância (mais perto primeiro)
    if (fabs(e1->distancia - e2->distancia) > EPSILON)
    {
        return (e1->distancia < e2->distancia) ? -1 : 1;
    }

    // 3. INICIO antes de FIM
    if (e1->tipo != e2->tipo)
    {
        return (e1->tipo == EVENTO_INICIO) ? -1 : 1;
    }

    return 0;
}

/**
 * Verifica se o segmento cruza o raio no ângulo 0 (eixo X positivo).
 * Usado para inicializar a árvore antes do loop.
 */
static bool segmento_cruza_angulo_zero(Ponto centro, Segmento *seg)
{
    double d = distancia_raio_segmento(centro, 0.0, *seg);
    return !isinf(d) && !isnan(d) && d > EPSILON && d < 1e9;
}

/**
 * Insere vértice no polígono apenas se diferente do último.
 * Evita vértices duplicados consecutivos.
 */
static void inserir_vertice_unico(PoligonoVisibilidade pol, Ponto novo, Ponto *ultimo)
{
    if (ultimo != NULL && pontos_iguais(*ultimo, novo))
    {
        return;
    }
    
    poligono_adicionar_vertice(pol, novo);
    *ultimo = novo;
}

/* ============================================================================
 * Função Principal: visibilidade_calcular
 * ============================================================================ */

PoligonoVisibilidade visibilidade_calcular(Ponto centro, LinkedList barreiras_input)
{
    // Cria polígono de resultado
    PoligonoVisibilidade pol = poligono_criar(centro);
    
    if (list_size(barreiras_input) == 0)
    {
        return pol;
    }

    /* ========================================================================
     * FASE 1: Criar cópia dos segmentos e adicionar bounding box
     * ======================================================================== */
    LinkedList barreiras = list_create();
    
    double min_x = 1e9, min_y = 1e9, max_x = -1e9, max_y = -1e9;
    
    // Copia segmentos e calcula bounding box
    for (int i = 0; i < list_size(barreiras_input); i++)
    {
        Segmento *s = (Segmento *)list_get_at(barreiras_input, i);
        Segmento *copia = malloc(sizeof(Segmento));
        *copia = *s;
        list_insert_back(barreiras, copia);
        
        // Atualiza min/max
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
    
    // Adiciona margem
    min_x -= MARGEM_BBOX;
    min_y -= MARGEM_BBOX;
    max_x += MARGEM_BBOX;
    max_y += MARGEM_BBOX;
    
    // Cria 4 segmentos da bounding box (retângulo envolvente)
    Segmento *bbox[4];
    bbox[0] = malloc(sizeof(Segmento));
    bbox[0]->p1 = (Ponto){min_x, min_y};
    bbox[0]->p2 = (Ponto){max_x, min_y};
    bbox[0]->id = -1;
    
    bbox[1] = malloc(sizeof(Segmento));
    bbox[1]->p1 = (Ponto){max_x, min_y};
    bbox[1]->p2 = (Ponto){max_x, max_y};
    bbox[1]->id = -2;
    
    bbox[2] = malloc(sizeof(Segmento));
    bbox[2]->p1 = (Ponto){max_x, max_y};
    bbox[2]->p2 = (Ponto){min_x, max_y};
    bbox[2]->id = -3;
    
    bbox[3] = malloc(sizeof(Segmento));
    bbox[3]->p1 = (Ponto){min_x, max_y};
    bbox[3]->p2 = (Ponto){min_x, min_y};
    bbox[3]->id = -4;
    
    for (int i = 0; i < 4; i++)
    {
        list_insert_back(barreiras, bbox[i]);
    }

    /* ========================================================================
     * FASE 1.5: Normalização Angular - Dividir segmentos que cruzam ângulo 0
     * ======================================================================== */
    int n_barreiras = list_size(barreiras);
    LinkedList segmentos_novos = list_create();
    
    for (int i = 0; i < n_barreiras; i++)
    {
        Segmento *seg = (Segmento *)list_get_at(barreiras, i);
        
        // Verifica se cruza o raio no ângulo 0
        Ponto inter = interseccao_raio_segmento(centro, 0.0, *seg);
        
        if (!isnan(inter.x))
        {
            double d1 = distancia(inter, seg->p1);
            double d2 = distancia(inter, seg->p2);
            
            // Só divide se a interseção não está nos extremos
            if (d1 > EPSILON && d2 > EPSILON)
            {
                // Cria dois segmentos novos
                Segmento *s1 = malloc(sizeof(Segmento));
                s1->p1 = seg->p1;
                s1->p2 = inter;
                s1->id = seg->id;
                
                Segmento *s2 = malloc(sizeof(Segmento));
                s2->p1 = inter;
                s2->p2 = seg->p2;
                s2->id = seg->id;
                
                list_insert_back(segmentos_novos, s1);
                list_insert_back(segmentos_novos, s2);
                
                // Marca segmento original para remoção
                seg->id = -9999;
            }
        }
    }
    
    // Remove segmentos marcados e adiciona os novos
    LinkedList barreiras_final = list_create();
    while (!list_is_empty(barreiras))
    {
        Segmento *seg = (Segmento *)list_remove_front(barreiras);
        if (seg->id != -9999)
        {
            list_insert_back(barreiras_final, seg);
        }
        else
        {
            free(seg);
        }
    }
    list_destroy(barreiras);
    
    while (!list_is_empty(segmentos_novos))
    {
        list_insert_back(barreiras_final, list_remove_front(segmentos_novos));
    }
    list_destroy(segmentos_novos);
    
    barreiras = barreiras_final;

    /* ========================================================================
     * FASE 2: Criar lista de eventos
     * ======================================================================== */
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
        // Considera a diferença angular menor (pelo caminho mais curto)
        bool p1_inicio;
        
        double diff = ang2 - ang1;
        
        if (fabs(ang1 - ang2) < EPSILON)
        {
            // Ângulos iguais: o mais perto é INICIO
            p1_inicio = (dist1 < dist2);
        }
        else if (fabs(diff) > M_PI)
        {
            // Span angular > 180°: o maior ângulo é INICIO (pois cruza 0)
            // Quando a diferença é muito grande, significa que estamos
            // indo pelo caminho "longo" - devemos inverter
            p1_inicio = (ang1 > ang2);
        }
        else
        {
            // Caminho curto: menor ângulo é INICIO
            p1_inicio = (ang1 < ang2);
        }

        // Evento p1
        eventos[k] = malloc(sizeof(Evento));
        eventos[k]->angulo = ang1;
        eventos[k]->distancia = dist1;
        eventos[k]->seg = seg;
        eventos[k]->p = seg->p1;
        eventos[k]->tipo = p1_inicio ? EVENTO_INICIO : EVENTO_FIM;
        k++;

        // Evento p2
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

    /* ========================================================================
     * FASE 3: Inicialização - Vértice artificial v0 no ângulo 0
     * ======================================================================== */
    ArvoreSegmentos ativos = arvore_seg_criar(centro);
    arvore_seg_definir_angulo(ativos, 0.0);
    
    // Insere todos os segmentos que cruzam o ângulo 0
    for (int i = 0; i < qtd_barreiras; i++)
    {
        Segmento *seg = (Segmento *)list_get_at(barreiras, i);
        if (segmento_cruza_angulo_zero(centro, seg))
        {
            arvore_seg_inserir(ativos, seg);
        }
    }

    // Biombo inicial: segmento mais próximo no ângulo 0
    Segmento *biombo = arvore_seg_obter_primeiro(ativos);
    Ponto ultimo_ponto;
    bool tem_ultimo = false;
    
    // Adiciona primeiro vértice (interseção do raio 0 com biombo)
    if (biombo != NULL)
    {
        Ponto inter = interseccao_raio_segmento(centro, 0.0, *biombo);
        if (!isnan(inter.x))
        {
            poligono_adicionar_vertice(pol, inter);
            ultimo_ponto = inter;
            tem_ultimo = true;
        }
    }

    /* ========================================================================
     * FASE 4: Loop de varredura angular
     * ======================================================================== */
    for (int i = 0; i < qtd_eventos; i++)
    {
        Evento *e = eventos[i];
        arvore_seg_definir_angulo(ativos, e->angulo);

        if (e->tipo == EVENTO_INICIO)
        {
            /* ================================================================
             * Cenário A: Evento de INÍCIO (segmento entra)
             * ================================================================ */
            arvore_seg_inserir(ativos, e->seg);
            Segmento *mais_prox = arvore_seg_obter_primeiro(ativos);

            // Verifica se este segmento está conectado ao biombo atual
            // (compartilha vértice onde o biombo termina)
            bool segmento_conectado = false;
            if (biombo != NULL)
            {
                // Verifica se alguma extremidade do novo segmento coincide
                // com alguma extremidade do biombo
                if (pontos_iguais(e->p, biombo->p1) || pontos_iguais(e->p, biombo->p2))
                {
                    // Verifica se há um FIM do biombo logo após este evento
                    for (int j = i + 1; j < qtd_eventos && j < i + 5; j++)
                    {
                        Evento *evt = eventos[j];
                        if (fabs(evt->angulo - e->angulo) > EPSILON) break;
                        if (evt->tipo == EVENTO_FIM && evt->seg == biombo)
                        {
                            // O biombo termina neste vértice, novo segmento o substitui
                            segmento_conectado = true;
                            break;
                        }
                    }
                }
            }

            // Se o segmento inserido virou o mais próximo (está na frente)
            // OU está conectado ao biombo atual
            if ((mais_prox == e->seg && biombo != e->seg) || segmento_conectado)
            {
                // Calcula interseção com biombo anterior (se não conectado)
                if (biombo != NULL && !segmento_conectado)
                {
                    Ponto inter = interseccao_raio_segmento(centro, e->angulo, *biombo);
                    if (!isnan(inter.x))
                    {
                        if (tem_ultimo)
                            inserir_vertice_unico(pol, inter, &ultimo_ponto);
                        else
                        {
                            poligono_adicionar_vertice(pol, inter);
                            ultimo_ponto = inter;
                            tem_ultimo = true;
                        }
                    }
                }
                
                // Adiciona vértice do evento (se conectado, é o ponto de transição)
                if (tem_ultimo)
                    inserir_vertice_unico(pol, e->p, &ultimo_ponto);
                else
                {
                    poligono_adicionar_vertice(pol, e->p);
                    ultimo_ponto = e->p;
                    tem_ultimo = true;
                }
                
                // Atualiza biombo
                biombo = e->seg;
            }
            // Se está atrás do biombo: nada visual, só insere na árvore
        }
        else
        {
            /* ================================================================
             * Cenário B: Evento de FIM (segmento sai)
             * ================================================================ */
            if (e->seg == biombo)
            {
                // Adiciona vértice do evento (final do biombo)
                if (tem_ultimo)
                    inserir_vertice_unico(pol, e->p, &ultimo_ponto);
                else
                {
                    poligono_adicionar_vertice(pol, e->p);
                    ultimo_ponto = e->p;
                    tem_ultimo = true;
                }

                // Remove da árvore
                arvore_seg_remover(ativos, e->seg);

                // Busca segmento que COMEÇA neste vértice (está conectado)
                // Procura no próximo evento se é INICIO no mesmo ponto
                Segmento *segmento_conectado = NULL;
                if (i + 1 < qtd_eventos)
                {
                    Evento *prox = eventos[i + 1];
                    // Verifica se próximo evento é INICIO no mesmo ângulo e ponto
                    if (prox->tipo == EVENTO_INICIO && 
                        fabs(prox->angulo - e->angulo) < EPSILON &&
                        pontos_iguais(prox->p, e->p))
                    {
                        // Este segmento começa aqui - é o próximo biombo natural
                        segmento_conectado = prox->seg;
                    }
                }

                // Se encontrou segmento conectado, usa ele
                if (segmento_conectado != NULL)
                {
                    biombo = segmento_conectado;
                }
                else
                {
                    // Busca novo biombo na árvore
                    Segmento *novo_prox = arvore_seg_obter_primeiro(ativos);
                    
                    if (novo_prox != NULL)
                    {
                        Ponto inter = interseccao_raio_segmento(centro, e->angulo, *novo_prox);
                        if (!isnan(inter.x))
                        {
                            if (tem_ultimo)
                                inserir_vertice_unico(pol, inter, &ultimo_ponto);
                            else
                            {
                                poligono_adicionar_vertice(pol, inter);
                                ultimo_ponto = inter;
                                tem_ultimo = true;
                            }
                        }
                    }
                    
                    biombo = novo_prox;
                }
            }
            else
            {
                // Segmento não era o biombo: apenas remove
                arvore_seg_remover(ativos, e->seg);
            }
        }
    }

    /* ========================================================================
     * FASE 5: Limpeza
     * ======================================================================== */
    for (int i = 0; i < qtd_eventos; i++)
    {
        free(eventos[i]);
    }
    free(eventos);
    
    // Libera segmentos copiados
    while (!list_is_empty(barreiras))
    {
        free(list_remove_front(barreiras));
    }
    list_destroy(barreiras);
    
    arvore_seg_destruir(ativos);

    return pol;
}

/* ============================================================================
 * Funções de Verificação de Visibilidade
 * ============================================================================ */

bool visibilidade_ponto_atingido(PoligonoVisibilidade pol, Ponto p)
{
    if (pol == NULL) return false;
    
    LinkedList vertices = poligono_get_vertices(pol);
    Ponto centro = poligono_get_centro(pol);
    
    int n = list_size(vertices);
    if (n < 3) return false;
    
    double dist_p = distancia(centro, p);
    double ang = angulo_polar(centro, p);
    if (ang < 0) ang += 2 * M_PI;

    // Procura a aresta do polígono que o raio para p intercepta
    for (int i = 0; i < n; i++)
    {
        Ponto *v1 = (Ponto *)list_get_at(vertices, i);
        Ponto *v2 = (Ponto *)list_get_at(vertices, (i + 1) % n);

        Segmento aresta = {*v1, *v2, 0};
        double d_borda = distancia_raio_segmento(centro, ang, aresta);

        if (!isinf(d_borda) && !isnan(d_borda))
        {
            // Ponto está dentro se está antes da borda (ou muito perto)
            if (dist_p <= d_borda + 1.0)
                return true;
        }
    }
    
    return false;
}

bool visibilidade_segmento_atingido(PoligonoVisibilidade pol, Ponto p1, Ponto p2)
{
    if (pol == NULL) return false;
    
    // Teste 1: extremidades
    if (visibilidade_ponto_atingido(pol, p1)) return true;
    if (visibilidade_ponto_atingido(pol, p2)) return true;
    
    // Teste 2: ponto médio
    Ponto pm = {(p1.x + p2.x) / 2, (p1.y + p2.y) / 2};
    if (visibilidade_ponto_atingido(pol, pm)) return true;
    
    // Teste 3: interseção com arestas do polígono
    LinkedList vertices = poligono_get_vertices(pol);
    Segmento seg = {p1, p2, 0};
    int n = list_size(vertices);
    
    for (int i = 0; i < n; i++)
    {
        Ponto *v1 = (Ponto *)list_get_at(vertices, i);
        Ponto *v2 = (Ponto *)list_get_at(vertices, (i + 1) % n);
        
        Segmento aresta = {*v1, *v2, 0};
        
        if (segmentos_intersectam(seg, aresta))
        {
            return true;
        }
    }
    
    return false;
}

void visibilidade_destruir(PoligonoVisibilidade pol)
{
    poligono_destruir(pol);
}

LinkedList visibilidade_obter_vertices(PoligonoVisibilidade pol)
{
    return poligono_get_vertices(pol);
}

Ponto visibilidade_obter_centro(PoligonoVisibilidade pol)
{
    return poligono_get_centro(pol);
}