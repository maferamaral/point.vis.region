#ifndef GEOMETRIA_H
#define GEOMETRIA_H

#include <stdbool.h>

// --- Estruturas Básicas ---

typedef struct
{
    double x;
    double y;
} Ponto;

typedef struct
{
    double x;
    double y;
} Vetor;

typedef struct
{
    Ponto p1;
    Ponto p2;
} Segmento;

// --- Construtores ---

Ponto ponto_criar(double x, double y);
Segmento segmento_criar(Ponto p1, Ponto p2);

// --- Cálculos Básicos ---

// Retorna a distância euclidiana entre dois pontos
double distancia(Ponto a, Ponto b);

// Retorna o quadrado da distância (mais rápido para comparações)
double distancia_sq(Ponto a, Ponto b);

// Produto Vetorial (Cross Product) de (b-a) e (c-a).
// Retorna > 0 se c está à esquerda de ab
// Retorna < 0 se c está à direita de ab
// Retorna 0 se são colineares
double produto_vetorial(Ponto a, Ponto b, Ponto c);

// Retorna o ângulo polar do ponto p em relação ao centro (em radianos, de -PI a PI)
double angulo_polar(Ponto centro, Ponto p);

// --- Intersecções ---

// Verifica se dois segmentos se intersectam (retorna true/false)
bool segmentos_intersectam(Segmento s1, Segmento s2);

// Calcula o ponto exato de intersecção entre duas retas definidas pelos segmentos.
// Retorna um ponto com coordenadas NAN se forem paralelas.
Ponto ponto_interseccao(Segmento s1, Segmento s2);

// Calcula o ponto onde um "raio" (saindo da origem com um certo ângulo) atinge um segmento.
// Essencial para a árvore de segmentos ativos na varredura.
// Se não intersectar, retorna um ponto com coordenadas NAN.
Ponto interseccao_raio_segmento(Ponto origem, double angulo, Segmento seg);

// Verifica se o ponto P está sobre o segmento S (assumindo colinearidade prévia)
bool ponto_no_segmento(Ponto p, Segmento s);

// Retorna o valor máximo entre dois doubles
double max_d(double a, double b);
// Retorna o valor mínimo entre dois doubles
double min_d(double a, double b);

#endif