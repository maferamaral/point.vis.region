/* ponto.h
 *
 * TAD Ponto - Representação de ponto 2D
 * Usado no algoritmo de visibilidade
 */

#ifndef PONTO_H
#define PONTO_H

/* Tipo opaco para Ponto */
typedef void* Ponto;

/* ============================================================================
 * Funções de Criação e Destruição
 * ============================================================================ */

/**
 * Cria um novo ponto.
 * @param x Coordenada X
 * @param y Coordenada Y
 * @return Novo ponto, ou NULL em caso de erro
 */
Ponto criar_ponto(double x, double y);

/**
 * Clona um ponto.
 * @param p Ponto a ser clonado
 * @return Nova cópia do ponto, ou NULL em caso de erro
 */
Ponto clonar_ponto(Ponto p);

/**
 * Destroi um ponto.
 * @param p Ponto a ser destruído
 */
void destruir_ponto(Ponto p);

/* ============================================================================
 * Funções de Acesso
 * ============================================================================ */

/**
 * Obtém a coordenada X do ponto.
 */
double get_ponto_x(Ponto p);

/**
 * Obtém a coordenada Y do ponto.
 */
double get_ponto_y(Ponto p);

/**
 * Define a coordenada X do ponto.
 */
void set_ponto_x(Ponto p, double x);

/**
 * Define a coordenada Y do ponto.
 */
void set_ponto_y(Ponto p, double y);

/**
 * Define ambas coordenadas do ponto.
 */
void set_ponto(Ponto p, double x, double y);

/* ============================================================================
 * Funções Geométricas
 * ============================================================================ */

/**
 * Calcula a distância entre dois pontos.
 * @param p1 Primeiro ponto
 * @param p2 Segundo ponto
 * @return Distância euclidiana
 */
double ponto_distancia(Ponto p1, Ponto p2);

/**
 * Calcula o ângulo polar de p2 relativo a p1 (origem).
 * @param origem Ponto de origem
 * @param p Ponto para calcular ângulo
 * @return Ângulo em radianos [0, 2*PI)
 */
double ponto_angulo_polar(Ponto origem, Ponto p);

/**
 * Verifica se dois pontos são iguais (com tolerância epsilon).
 * @param p1 Primeiro ponto
 * @param p2 Segundo ponto
 * @return 1 se iguais, 0 caso contrário
 */
int ponto_igual(Ponto p1, Ponto p2);

#endif /* PONTO_H */
