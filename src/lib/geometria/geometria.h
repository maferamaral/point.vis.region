#ifndef GEOMETRIA_H
#define GEOMETRIA_H

#include <stdbool.h>
#include <math.h>

#define EPSILON 1e-9

typedef struct {
    double x;
    double y;
} Ponto;

typedef struct {
    Ponto p1;
    Ponto p2;
    int id; // Útil para identificar barreiras no debug
} Segmento;

// Construtores
Ponto ponto_criar(double x, double y);
Segmento segmento_criar(Ponto p1, Ponto p2);

// Operações
double distancia(Ponto a, Ponto b);
double distancia_sq(Ponto a, Ponto b);
double produto_vetorial(Ponto a, Ponto b, Ponto c);
double angulo_polar(Ponto centro, Ponto p);

// Intersecções
bool segmentos_intersectam(Segmento s1, Segmento s2);
Ponto ponto_interseccao(Segmento s1, Segmento s2);
Ponto interseccao_raio_segmento(Ponto origem, double angulo, Segmento seg);
double distancia_raio_segmento(Ponto origem, double angulo, Segmento seg);

// Auxiliares
double max_d(double a, double b);
double min_d(double a, double b);

#endif