/* calculos.c
 *
 * Implementação das funções de cálculos geométricos
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "calculos.h"

// Adjust includes for shapes (assuming they are in ../../formas/...)
// Need to check where forms are in src. 
// In src, forms are in ../../../formas usually if from lib.
// But we are in src/lib/geometria/calculos.
// So forms are in src/lib/formas -> ../../formas
#include "../../formas/formas.h"
#include "../../formas/circulo/circulo.h"
#include "../../formas/retangulo/retangulo.h"
#include "../../formas/linha/linha.h"
#include "../../formas/texto/texto.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef INFINITY
#define INFINITY (1.0/0.0)
#endif

/* ============================================================================
 * Implementação das Funções de Orientação
 * ============================================================================ */

double produto_vetorial(Ponto p1, Ponto p2, Ponto p3)
{
    if (p1 == NULL || p2 == NULL || p3 == NULL) return 0.0;
    
    double x1 = get_ponto_x(p1);
    double y1 = get_ponto_y(p1);
    double x2 = get_ponto_x(p2);
    double y2 = get_ponto_y(p2);
    double x3 = get_ponto_x(p3);
    double y3 = get_ponto_y(p3);
    
    /* (P2 - P1) x (P3 - P1) */
    return (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
}

Orientacao calcular_orientacao(Ponto p1, Ponto p2, Ponto p3)
{
    double cross = produto_vetorial(p1, p2, p3);
    
    if (fabs(cross) < GEO_EPSILON)
    {
        return ORIENTACAO_COLINEAR;
    }
    else if (cross > 0)
    {
        return ORIENTACAO_ANTIHORARIO;
    }
    else
    {
        return ORIENTACAO_HORARIO;
    }
}

Orientacao calcular_orientacao_coords(double x1, double y1, 
                                       double x2, double y2, 
                                       double x3, double y3)
{
    double cross = (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
    
    if (fabs(cross) < GEO_EPSILON)
    {
        return ORIENTACAO_COLINEAR;
    }
    else if (cross > 0)
    {
        return ORIENTACAO_ANTIHORARIO;
    }
    else
    {
        return ORIENTACAO_HORARIO;
    }
}

/* ============================================================================
 * Implementação das Funções de Interseção
 * ============================================================================ */

int intersecao_raio_segmento(Ponto origem, Ponto direcao, Segmento seg, Ponto *resultado)
{
    if (origem == NULL || direcao == NULL || seg == NULL || resultado == NULL)
    {
        return 0;
    }
    
    double ox = get_ponto_x(origem);
    double oy = get_ponto_y(origem);
    double dx = get_ponto_x(direcao) - ox;
    double dy = get_ponto_y(direcao) - oy;
    
    double sx1 = get_segmento_x1(seg);
    double sy1 = get_segmento_y1(seg);
    double sx2 = get_segmento_x2(seg);
    double sy2 = get_segmento_y2(seg);
    
    /* Vetor do segmento */
    double segx = sx2 - sx1;
    double segy = sy2 - sy1;
    
    /* Denominador do sistema de equações paramétricas */
    double denom = dx * segy - dy * segx;
    
    /* Raio paralelo ao segmento */
    if (fabs(denom) < GEO_EPSILON)
    {
        return 0;
    }
    
    /* Parâmetro t para o raio (origem + t * direção) */
    double t = ((sx1 - ox) * segy - (sy1 - oy) * segx) / denom;
    
    /* Parâmetro u para o segmento (s1 + u * (s2 - s1)) */
    double u = ((sx1 - ox) * dy - (sy1 - oy) * dx) / denom;
    
    /* Interseção válida: t >= 0 (na direção do raio) e 0 <= u <= 1 (dentro do segmento) */
    if (t >= -GEO_EPSILON && u >= -GEO_EPSILON && u <= 1.0 + GEO_EPSILON)
    {
        double ix = ox + t * dx;
        double iy = oy + t * dy;
        *resultado = criar_ponto(ix, iy);
        return 1;
    }
    
    return 0;
}

double distancia_raio_segmento(Ponto origem, double angulo, Segmento seg)
{
    if (origem == NULL || seg == NULL) return INFINITY;
    
    double ox = get_ponto_x(origem);
    double oy = get_ponto_y(origem);
    
    /* Criar ponto de direção */
    double dx = cos(angulo);
    double dy = sin(angulo);
    
    double sx1 = get_segmento_x1(seg);
    double sy1 = get_segmento_y1(seg);
    double sx2 = get_segmento_x2(seg);
    double sy2 = get_segmento_y2(seg);
    
    double segx = sx2 - sx1;
    double segy = sy2 - sy1;
    
    double denom = dx * segy - dy * segx;
    
    if (fabs(denom) < GEO_EPSILON)
    {
        return INFINITY;
    }
    
    double t = ((sx1 - ox) * segy - (sy1 - oy) * segx) / denom;
    double u = ((sx1 - ox) * dy - (sy1 - oy) * dx) / denom;
    
    if (t >= -GEO_EPSILON && u >= -GEO_EPSILON && u <= 1.0 + GEO_EPSILON)
    {
        return t;
    }
    
    return INFINITY;
}

int ponto_na_frente(Ponto origem, Ponto ponto, Segmento seg)
{
    if (origem == NULL || ponto == NULL || seg == NULL) return 0;
    
    /* Calcular distância do ponto até a origem */
    double dist_ponto = ponto_distancia(origem, ponto);
    
    /* Calcular ângulo do ponto */
    double angulo = ponto_angulo_polar(origem, ponto);
    
    /* Calcular distância até o segmento nesse ângulo */
    double dist_seg = distancia_raio_segmento(origem, angulo, seg);
    
    /* Ponto está na frente se está mais perto que o segmento */
    return (dist_ponto < dist_seg - GEO_EPSILON);
}

int comparar_segmentos_raio(Ponto origem, double angulo, Segmento seg1, Segmento seg2)
{
    double dist1 = distancia_raio_segmento(origem, angulo, seg1);
    double dist2 = distancia_raio_segmento(origem, angulo, seg2);
    
    if (fabs(dist1 - dist2) < GEO_EPSILON)
    {
        return 0;
    }
    
    return (dist1 < dist2) ? -1 : 1;
}

/* ============================================================================
 * Implementação das Funções de Ponto no Polígono
 * ============================================================================ */

int ponto_no_poligono(double px, double py, double *vertices, int num_vertices)
{
    if (vertices == NULL || num_vertices < 3)
    {
        return 0;
    }
    
    /* Algoritmo Ray Casting (par/ímpar) */
    int dentro = 0;
    
    for (int i = 0, j = num_vertices - 1; i < num_vertices; j = i++)
    {
        double xi = vertices[i * 2];
        double yi = vertices[i * 2 + 1];
        double xj = vertices[j * 2];
        double yj = vertices[j * 2 + 1];
        
        /* Verifica se o raio horizontal cruza a aresta (i, j) */
        if (((yi > py) != (yj > py)) &&
            (px < (xj - xi) * (py - yi) / (yj - yi) + xi))
        {
            dentro = !dentro;
        }
    }
    
    return dentro;
}

/* ============================================================================
 * Funções Auxiliares Estáticas para Colisão
 * ============================================================================ */

/* Verifica se q está no segmento pr */
static int no_segmento(double px, double py, double rx, double ry, double qx, double qy)
{
    return qx <= fmax(px, rx) && qx >= fmin(px, rx) &&
           qy <= fmax(py, ry) && qy >= fmin(py, ry);
}

static int seg_intersepta(double x1, double y1, double x2, double y2,
                          double x3, double y3, double x4, double y4)
{
    Orientacao o1 = calcular_orientacao_coords(x1, y1, x2, y2, x3, y3);
    Orientacao o2 = calcular_orientacao_coords(x1, y1, x2, y2, x4, y4);
    Orientacao o3 = calcular_orientacao_coords(x3, y3, x4, y4, x1, y1);
    Orientacao o4 = calcular_orientacao_coords(x3, y3, x4, y4, x2, y2);
    
    /* Caso geral */
    if (o1 != o2 && o3 != o4) return 1;
    
    /* Casos especiais de colinearidade */
    if (o1 == ORIENTACAO_COLINEAR && no_segmento(x1, y1, x2, y2, x3, y3)) return 1;
    if (o2 == ORIENTACAO_COLINEAR && no_segmento(x1, y1, x2, y2, x4, y4)) return 1;
    if (o3 == ORIENTACAO_COLINEAR && no_segmento(x3, y3, x4, y4, x1, y1)) return 1;
    if (o4 == ORIENTACAO_COLINEAR && no_segmento(x3, y3, x4, y4, x2, y2)) return 1;
    
    return 0;
}

/* Distância quadrada mínima de um ponto P a um segmento AB */
static double dist_sq_ponto_segmento(double px, double py, double ax, double ay, double bx, double by)
{
    double l2 = (bx - ax)*(bx - ax) + (by - ay)*(by - ay);
    if (l2 == 0) return (px - ax)*(px - ax) + (py - ay)*(py - ay);
    
    double t = ((px - ax)*(bx - ax) + (py - ay)*(by - ay)) / l2;
    t = fmax(0, fmin(1, t));
    
    double projx = ax + t * (bx - ax);
    double projy = ay + t * (by - ay);
    
    return (px - projx)*(px - projx) + (py - projy)*(py - projy);
}


/* ============================================================================
 * Função Principal de Verificação
 * ============================================================================ */

// Note: shape accessors must match the names in src (or srcAndre port)
// IMPORTANT: src shapes use different getters! I must use the src getters here.
// src getters: circulo_get_x, retangulo_get_x, line_get_x1, text_get_x
// srcAndre getters: getCirculoX, getRetanguloX, getLinhaX1, getTextoX

int forma_no_poligono(void *forma_ptr, double *vertices, int num_vertices)
{
    if (forma_ptr == NULL || vertices == NULL || num_vertices < 3)
    {
        return 0;
    }
    
    // We cannot cast to Forma here because src doesn't have a generic Forma struct visible here maybe?
    // But qry.c and geo.c handle specific types.
    // However, this function in srcAndre gets a void* and casts it based on type.
    // Wait, the signature in header is `void* forma` but inside it considers it wraps a `Forma` object which has type.
    // In src, we don't have a generic `Forma` wrapper struct that exposes type easily unless we pass type.
    // In srcAndre `Forma` is an object that wraps the specific shape.
    // In `src/lib/geo/geo.c`, we store `ElementoGeo` which has `TipoForma` and `void* forma`.
    
    // PROBLEM: `forma_no_poligono` needs to know the type.
    // In srcAndre, `forma` is a `Forma` object.
    // In src, we have raw pointers to Circle, Rectangle, etc.
    // I should change the signature of `forma_no_poligono` to accept `TipoForma` and `void*` in `src`.
    // But I am writing `calculos.c` now.
    // I will modify `forma_no_poligono` to NOT implement the dispatch here, but let the caller handle it?
    // Or I can add `TipoForma` parameter.
    
    // Actually, `qry.c` calls `forma_foi_atingida` which calls `visibilidade_ponto_atingido`.
    // `visibilidade_ponto_atingido` calls `ponto_no_poligono`.
    // The `forma_no_poligono` function in srcAndre seems to be used for "advanced" checking (segment intersection etc).
    // `qry.c` in `src` ALREADY implements manual checks for each shape type calling `visibilidade_ponto_atingido`.
    // So I might NOT need `forma_no_poligono` in `calculos.c` if `qry.c` does the job.
    // Let's comment out `forma_no_poligono` for now or provide an empty impl or just skip it
    // because `qry.c` in `src` implements the logic of checking vertices against the polygon.
    
    // Wait, `visibilidade_segmento_atingido` is used in `qry.c`.
    // I should expose `ponto_no_poligono` and helper utils.
    
     return 0; // Placeholder, logic is in qry.c in src
}

/* Implementação de intersecao_segmentos usando orientacao */
int intersecao_segmentos(Ponto p1, Ponto p2, Ponto p3, Ponto p4) {
    Orientacao o1 = calcular_orientacao(p1, p2, p3);
    Orientacao o2 = calcular_orientacao(p1, p2, p4);
    Orientacao o3 = calcular_orientacao(p3, p4, p1);
    Orientacao o4 = calcular_orientacao(p3, p4, p2);

    // Caso geral
    if (o1 != o2 && o3 != o4) return 1;

    // Casos especiais (colineares e nas pontas)
    
    // Função auxiliar para verificar se Q está no segmento PR
    #define NO_SEGMENTO(P, Q, R) (\
        get_ponto_x(Q) <= fmax(get_ponto_x(P), get_ponto_x(R)) + GEO_EPSILON && \
        get_ponto_x(Q) >= fmin(get_ponto_x(P), get_ponto_x(R)) - GEO_EPSILON && \
        get_ponto_y(Q) <= fmax(get_ponto_y(P), get_ponto_y(R)) + GEO_EPSILON && \
        get_ponto_y(Q) >= fmin(get_ponto_y(P), get_ponto_y(R)) - GEO_EPSILON)

    if (o1 == 0 && NO_SEGMENTO(p1, p3, p2)) return 1;
    if (o2 == 0 && NO_SEGMENTO(p1, p4, p2)) return 1;
    if (o3 == 0 && NO_SEGMENTO(p3, p1, p4)) return 1;
    if (o4 == 0 && NO_SEGMENTO(p3, p2, p4)) return 1;

    return 0;
}
