#include "geometria.h"
#include <math.h>
#include <stdio.h>

Ponto ponto_criar(double x, double y) {
    Ponto p = {x, y};
    return p;
}

Segmento segmento_criar(Ponto p1, Ponto p2) {
    Segmento s;
    s.p1 = p1; s.p2 = p2; s.id = 0;
    return s;
}

double distancia_sq(Ponto a, Ponto b) {
    return (a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y);
}

double distancia(Ponto a, Ponto b) {
    return sqrt(distancia_sq(a, b));
}

double produto_vetorial(Ponto a, Ponto b, Ponto c) {
    return (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x);
}

double angulo_polar(Ponto centro, Ponto p) {
    return atan2(p.y - centro.y, p.x - centro.x);
}

double max_d(double a, double b) { return (a > b) ? a : b; }
double min_d(double a, double b) { return (a < b) ? a : b; }

static bool on_segment(Ponto p, Segmento s) {
    return p.x >= min_d(s.p1.x, s.p2.x) - EPSILON && 
           p.x <= max_d(s.p1.x, s.p2.x) + EPSILON &&
           p.y >= min_d(s.p1.y, s.p2.y) - EPSILON && 
           p.y <= max_d(s.p1.y, s.p2.y) + EPSILON;
}

bool segmentos_intersectam(Segmento s1, Segmento s2) {
    double d1 = produto_vetorial(s2.p1, s2.p2, s1.p1);
    double d2 = produto_vetorial(s2.p1, s2.p2, s1.p2);
    double d3 = produto_vetorial(s1.p1, s1.p2, s2.p1);
    double d4 = produto_vetorial(s1.p1, s1.p2, s2.p2);

    if (((d1 > EPSILON && d2 < -EPSILON) || (d1 < -EPSILON && d2 > EPSILON)) &&
        ((d3 > EPSILON && d4 < -EPSILON) || (d3 < -EPSILON && d4 > EPSILON))) return true;

    if (fabs(d1) < EPSILON && on_segment(s1.p1, s2)) return true;
    if (fabs(d2) < EPSILON && on_segment(s1.p2, s2)) return true;
    if (fabs(d3) < EPSILON && on_segment(s2.p1, s1)) return true;
    if (fabs(d4) < EPSILON && on_segment(s2.p2, s1)) return true;

    return false;
}

Ponto ponto_interseccao(Segmento s1, Segmento s2) {
    double x1 = s1.p1.x, y1 = s1.p1.y, x2 = s1.p2.x, y2 = s1.p2.y;
    double x3 = s2.p1.x, y3 = s2.p1.y, x4 = s2.p2.x, y4 = s2.p2.y;

    double den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    if (fabs(den) < EPSILON) return ponto_criar(NAN, NAN);

    double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / den;
    return ponto_criar(x1 + t * (x2 - x1), y1 + t * (y2 - y1));
}

Ponto interseccao_raio_segmento(Ponto origem, double angulo, Segmento seg) {
    double len = 10000000.0; // Raio muito grande
    Ponto fim = {origem.x + cos(angulo) * len, origem.y + sin(angulo) * len};
    Segmento raio = {origem, fim};

    if (segmentos_intersectam(raio, seg)) {
        return ponto_interseccao(raio, seg);
    }
    return ponto_criar(NAN, NAN);
}

double distancia_raio_segmento(Ponto origem, double angulo, Segmento seg) {
    Ponto inter = interseccao_raio_segmento(origem, angulo, seg);
    if (isnan(inter.x)) return INFINITY;
    return distancia(origem, inter);
}