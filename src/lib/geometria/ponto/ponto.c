/* ponto.c
 *
 * Implementação do TAD Ponto
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ponto.h"

/* Tolerância para comparações de ponto flutuante */
#define EPSILON 1e-9

/* Constante PI */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* ============================================================================
 * Estrutura Interna (Ponteiro Opaco)
 * ============================================================================ */

typedef struct ponto_internal
{
    double x;
    double y;
} PontoInternal;

/* ============================================================================
 * Implementação das Funções de Criação e Destruição
 * ============================================================================ */

Ponto criar_ponto(double x, double y)
{
    PontoInternal *p = (PontoInternal*)malloc(sizeof(PontoInternal));
    if (p == NULL)
    {
        fprintf(stderr, "Erro: falha ao alocar ponto.\n");
        return NULL;
    }
    
    p->x = x;
    p->y = y;
    
    return (Ponto)p;
}

Ponto clonar_ponto(Ponto ponto)
{
    PontoInternal *p = (PontoInternal*)ponto;
    if (p == NULL) return NULL;
    
    return criar_ponto(p->x, p->y);
}

void destruir_ponto(Ponto ponto)
{
    if (ponto != NULL)
    {
        free(ponto);
    }
}

/* ============================================================================
 * Implementação das Funções de Acesso
 * ============================================================================ */

double get_ponto_x(Ponto ponto)
{
    PontoInternal *p = (PontoInternal*)ponto;
    return p ? p->x : 0.0;
}

double get_ponto_y(Ponto ponto)
{
    PontoInternal *p = (PontoInternal*)ponto;
    return p ? p->y : 0.0;
}

void set_ponto_x(Ponto ponto, double x)
{
    PontoInternal *p = (PontoInternal*)ponto;
    if (p != NULL)
    {
        p->x = x;
    }
}

void set_ponto_y(Ponto ponto, double y)
{
    PontoInternal *p = (PontoInternal*)ponto;
    if (p != NULL)
    {
        p->y = y;
    }
}

void set_ponto(Ponto ponto, double x, double y)
{
    PontoInternal *p = (PontoInternal*)ponto;
    if (p != NULL)
    {
        p->x = x;
        p->y = y;
    }
}

/* ============================================================================
 * Implementação das Funções Geométricas
 * ============================================================================ */

double ponto_distancia(Ponto ponto1, Ponto ponto2)
{
    PontoInternal *p1 = (PontoInternal*)ponto1;
    PontoInternal *p2 = (PontoInternal*)ponto2;
    
    if (p1 == NULL || p2 == NULL) return 0.0;
    
    double dx = p2->x - p1->x;
    double dy = p2->y - p1->y;
    
    return sqrt(dx * dx + dy * dy);
}

double ponto_angulo_polar(Ponto origem, Ponto ponto)
{
    PontoInternal *o = (PontoInternal*)origem;
    PontoInternal *p = (PontoInternal*)ponto;
    
    if (o == NULL || p == NULL) return 0.0;
    
    double dx = p->x - o->x;
    double dy = p->y - o->y;
    
    double angulo = atan2(dy, dx);
    
    /* Normaliza para [0, 2*PI) */
    if (angulo < 0)
    {
        angulo += 2.0 * M_PI;
    }
    
    return angulo;
}

int ponto_igual(Ponto ponto1, Ponto ponto2)
{
    PontoInternal *p1 = (PontoInternal*)ponto1;
    PontoInternal *p2 = (PontoInternal*)ponto2;
    
    if (p1 == NULL || p2 == NULL) return 0;
    
    return (fabs(p1->x - p2->x) < EPSILON) && 
           (fabs(p1->y - p2->y) < EPSILON);
}
