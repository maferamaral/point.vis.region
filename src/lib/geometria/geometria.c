#include "geometria.h"
#include <math.h>
#include <stdio.h>

#define EPSILON 1e-9 // Tolerância para erros de ponto flutuante

// --- Construtores ---

Ponto ponto_criar(double x, double y)
{
    Ponto p;
    p.x = x;
    p.y = y;
    return p;
}

Segmento segmento_criar(Ponto p1, Ponto p2)
{
    Segmento s;
    s.p1 = p1;
    s.p2 = p2;
    return s;
}

// --- Funções Auxiliares ---

double max_d(double a, double b) { return (a > b) ? a : b; }
double min_d(double a, double b) { return (a < b) ? a : b; }

// --- Cálculos Geométricos ---

double distancia_sq(Ponto a, Ponto b)
{
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return dx * dx + dy * dy;
}

double distancia(Ponto a, Ponto b)
{
    return sqrt(distancia_sq(a, b));
}

double produto_vetorial(Ponto a, Ponto b, Ponto c)
{
    // Vetor AB = (b.x - a.x, b.y - a.y)
    // Vetor AC = (c.x - a.x, c.y - a.y)
    // Cross = (AB.x * AC.y) - (AB.y * AC.x)
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

double angulo_polar(Ponto centro, Ponto p)
{
    return atan2(p.y - centro.y, p.x - centro.x);
}

bool ponto_no_segmento(Ponto p, Segmento s)
{
    return p.x >= min_d(s.p1.x, s.p2.x) - EPSILON &&
           p.x <= max_d(s.p1.x, s.p2.x) + EPSILON &&
           p.y >= min_d(s.p1.y, s.p2.y) - EPSILON &&
           p.y <= max_d(s.p1.y, s.p2.y) + EPSILON;
}

// --- Intersecções ---

bool segmentos_intersectam(Segmento s1, Segmento s2)
{
    Ponto p1 = s1.p1, q1 = s1.p2;
    Ponto p2 = s2.p1, q2 = s2.p2;

    // Orientações
    double d1 = produto_vetorial(p2, q2, p1);
    double d2 = produto_vetorial(p2, q2, q1);
    double d3 = produto_vetorial(p1, q1, p2);
    double d4 = produto_vetorial(p1, q1, q2);

    // Caso Geral: intersecção propriamente dita
    if (((d1 > EPSILON && d2 < -EPSILON) || (d1 < -EPSILON && d2 > EPSILON)) &&
        ((d3 > EPSILON && d4 < -EPSILON) || (d3 < -EPSILON && d4 > EPSILON)))
    {
        return true;
    }

    // Casos Especiais: Colineares e sobrepostos
    if (fabs(d1) < EPSILON && ponto_no_segmento(p1, s2))
        return true;
    if (fabs(d2) < EPSILON && ponto_no_segmento(q1, s2))
        return true;
    if (fabs(d3) < EPSILON && ponto_no_segmento(p2, s1))
        return true;
    if (fabs(d4) < EPSILON && ponto_no_segmento(q2, s1))
        return true;

    return false;
}

Ponto ponto_interseccao(Segmento s1, Segmento s2)
{
    // Fórmula baseada em determinantes (Regra de Cramer para intersecção de retas)
    double x1 = s1.p1.x, y1 = s1.p1.y;
    double x2 = s1.p2.x, y2 = s1.p2.y;
    double x3 = s2.p1.x, y3 = s2.p1.y;
    double x4 = s2.p2.x, y4 = s2.p2.y;

    double den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

    // Se o denominador for zero, as retas são paralelas
    if (fabs(den) < EPSILON)
    {
        return ponto_criar(NAN, NAN);
    }

    double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / den;

    // Ponto de intersecção: P = P1 + t * (P2 - P1)
    Ponto inter;
    inter.x = x1 + t * (x2 - x1);
    inter.y = y1 + t * (y2 - y1);

    return inter;
}

Ponto interseccao_raio_segmento(Ponto origem, double angulo, Segmento seg)
{
    // Criamos um segmento fictício muito longo para representar o raio
    // O comprimento deve ser maior que a diagonal da cidade
    double comprimento_grande = 1000000.0;

    Ponto fim_raio;
    fim_raio.x = origem.x + cos(angulo) * comprimento_grande;
    fim_raio.y = origem.y + sin(angulo) * comprimento_grande;

    Segmento raio = segmento_criar(origem, fim_raio);

    if (segmentos_intersectam(raio, seg))
    {
        return ponto_interseccao(raio, seg);
    }

    return ponto_criar(NAN, NAN);
}