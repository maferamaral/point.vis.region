#ifndef GEOMETRIA_H
#define GEOMETRIA_H

#include <stdbool.h>
#include <math.h>

// Definição de tolerância para comparações de float
#define EPSILON 1e-9

typedef
{
    double x;
    double y;
}
Ponto;

typedef struct
{
    Ponto p1;
    Ponto p2;
    int id;       // Opcional, útil para debug
    char cor[32]; // Added for visibility/painting
} Segmento;

// --- Construtores ---
Ponto ponto_criar(double x, double y);
Segmento segmento_criar(Ponto p1, Ponto p2);

// --- Operações Básicas ---
double distancia_sq(Ponto a, Ponto b);
double distancia(Ponto a, Ponto b);
double produto_vetorial(Ponto a, Ponto b, Ponto c);
double angulo_polar(Ponto centro, Ponto p);

// --- Intersecções ---
bool segmentos_intersectam(Segmento s1, Segmento s2);
Ponto ponto_interseccao(Segmento s1, Segmento s2);

// Calcula o ponto onde um raio (partindo da origem com certo ângulo) atinge um segmento.
// Se não atingir, retorna NAN.
Ponto interseccao_raio_segmento(Ponto origem, double angulo, Segmento seg);

// Verifica se um ponto está dentro de um triângulo (útil para verificar point-in-polygon depois)
bool ponto_no_triangulo(Ponto p, Ponto a, Ponto b, Ponto c);

// Retorna a distância da origem até a intersecção do raio com o segmento
// Usado pela árvore para ordenar segmentos
double distancia_raio_segmento(Ponto origem, double angulo, Segmento seg);

#endif