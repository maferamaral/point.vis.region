#ifndef GEOMETRIA_H
#define GEOMETRIA_H

#include <stdbool.h>

// Definição de Ponto (usado também como Vetor)
typedef struct
{
    double x;
    double y;
} Ponto;

// Definição de Segmento de Reta
typedef struct
{
    Ponto p1; // Início
    Ponto p2; // Fim
} Segmento;

// --- Construtores e Utilitários Básicos ---
Ponto ponto_criar(double x, double y);
Segmento segmento_criar(Ponto p1, Ponto p2);

// --- Operações Vetoriais ---
// Retorna o quadrado da distância (evita raiz quadrada, mais rápido para comparações)
double distancia_sq(Ponto a, Ponto b);
// Retorna a distância euclidiana real
double distancia(Ponto a, Ponto b);

// Produto vetorial (Cross Product) entre vetores (b-a) e (c-a).
// Retorno > 0: c está à esquerda de ab
// Retorno < 0: c está à direita de ab
// Retorno = 0: a, b, c são colineares
double produto_vetorial(Ponto a, Ponto b, Ponto c);

// Retorna o ângulo polar de 'p' em relação ao 'centro' (em radianos, -PI a PI ou 0 a 2PI)
double angulo_polar(Ponto centro, Ponto p);

// --- Intersecções e Visibilidade ---

// Verifica se dois segmentos se intersectam (retorna true/false)
bool segmentos_intersectam(Segmento s1, Segmento s2);

// Calcula o Ponto de intersecção entre duas retas definidas pelos segmentos.
// Retorna um ponto com coordenadas NAN ou INFINITY se forem paralelas.
Ponto ponto_interseccao(Segmento s1, Segmento s2);

// Calcula o ponto de intersecção entre um "raio" (linha da bomba até um ângulo) e um segmento.
// Útil para determinar onde a "luz" bate na parede.
// 'origem': centro da explosão
// 'angulo': direção do raio
// 'seg': barreira
// Retorna o ponto de impacto.
Ponto interseccao_raio_segmento(Ponto origem, double angulo, Segmento seg);

#endif