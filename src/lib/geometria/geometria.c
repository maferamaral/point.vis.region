#include "geometria.h"
#include <math.h>
#include <stdlib.h> // Para NULL se necessário
#include <stdio.h>  // Para debug

#define EPSILON 1e-9 // Tolerância para erros de ponto flutuante

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
    // atan2 retorna o ângulo em radianos entre -PI e PI
    return atan2(p.y - centro.y, p.x - centro.x);
}

// Funções auxiliares para intersecção
static double min(double a, double b) { return a < b ? a : b; }
static double max(double a, double b) { return a > b ? a : b; }

static bool on_segment(Ponto p, Ponto a, Ponto b)
{
    return p.x >= min(a.x, b.x) && p.x <= max(a.x, b.x) &&
           p.y >= min(a.y, b.y) && p.y <= max(a.y, b.y);
}

bool segmentos_intersectam(Segmento s1, Segmento s2)
{
    Ponto p1 = s1.p1, q1 = s1.p2;
    Ponto p2 = s2.p1, q2 = s2.p2;

    double d1 = produto_vetorial(p2, q2, p1);
    double d2 = produto_vetorial(p2, q2, q1);
    double d3 = produto_vetorial(p1, q1, p2);
    double d4 = produto_vetorial(p1, q1, q2);

    // Caso geral
    if (((d1 > EPSILON && d2 < -EPSILON) || (d1 < -EPSILON && d2 > EPSILON)) &&
        ((d3 > EPSILON && d4 < -EPSILON) || (d3 < -EPSILON && d4 > EPSILON)))
    {
        return true;
    }

    // Casos colineares (bordas se tocam)
    if (fabs(d1) < EPSILON && on_segment(p1, p2, q2))
        return true;
    if (fabs(d2) < EPSILON && on_segment(q1, p2, q2))
        return true;
    if (fabs(d3) < EPSILON && on_segment(p2, p1, q1))
        return true;
    if (fabs(d4) < EPSILON && on_segment(q2, p1, q1))
        return true;

    return false;
}

Ponto ponto_interseccao(Segmento s1, Segmento s2)
{
    // Utilizando a fórmula de determinantes para intersecção de linhas
    double x1 = s1.p1.x, y1 = s1.p1.y;
    double x2 = s1.p2.x, y2 = s1.p2.y;
    double x3 = s2.p1.x, y3 = s2.p1.y;
    double x4 = s2.p2.x, y4 = s2.p2.y;

    double den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

    if (fabs(den) < EPSILON)
    {
        // Retas paralelas ou coincidentes, retorna NAN
        return ponto_criar(NAN, NAN);
    }

    double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / den;

    // Ponto de intersecção
    Ponto inter;
    inter.x = x1 + t * (x2 - x1);
    inter.y = y1 + t * (y2 - y1);

    return inter;
}

Ponto interseccao_raio_segmento(Ponto origem, double angulo, Segmento seg)
{
    // Cria um segmento "fictício" muito longo representando o raio
    // Comprimento grande suficiente para cobrir a cidade
    double comprimento_raio = 100000.0;
    Ponto fim_raio;
    fim_raio.x = origem.x + cos(angulo) * comprimento_raio;
    fim_raio.y = origem.y + sin(angulo) * comprimento_raio;

    Segmento raio = segmento_criar(origem, fim_raio);

    // Verifica se intersecta
    if (segmentos_intersectam(raio, seg))
    {
        return ponto_interseccao(raio, seg);
    }

    // Se não intersecta, retorna NAN
    return ponto_criar(NAN, NAN);
}