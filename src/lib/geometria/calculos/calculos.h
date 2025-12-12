/* calculos.h
 *
 * Funções para cálculos geométricos
 * Orientação, interseção raio-segmento, etc.
 */

#ifndef CALCULOS_H
#define CALCULOS_H

#include "../ponto/ponto.h"
#include "../segmento/segmento.h"

/* Tolerância para comparações de ponto flutuante */
#define GEO_EPSILON 1e-9

/* Resultados de orientação */
typedef enum {
    ORIENTACAO_COLINEAR = 0,
    ORIENTACAO_HORARIO = -1,      /* Right turn */
    ORIENTACAO_ANTIHORARIO = 1    /* Left turn */
} Orientacao;

/* ============================================================================
 * Funções de Orientação
 * ============================================================================ */

/**
 * Calcula a orientação de três pontos P1 -> P2 -> P3.
 * 
 * @param p1 Primeiro ponto
 * @param p2 Segundo ponto
 * @param p3 Terceiro ponto
 * @return ORIENTACAO_ANTIHORARIO (esquerda), ORIENTACAO_HORARIO (direita), ou COLINEAR
 * 
 * @note Usa produto vetorial: (p2-p1) x (p3-p1)
 */
Orientacao calcular_orientacao(Ponto p1, Ponto p2, Ponto p3);

/**
 * Calcula a orientação usando coordenadas diretamente.
 */
Orientacao calcular_orientacao_coords(double x1, double y1, 
                                       double x2, double y2, 
                                       double x3, double y3);

/**
 * Calcula o produto vetorial 2D (P2-P1) x (P3-P1).
 * Também conhecido como "área com sinal" * 2.
 * 
 * @return Positivo se antihorário, negativo se horário, zero se colinear
 */
double produto_vetorial(Ponto p1, Ponto p2, Ponto p3);

/* ============================================================================
 * Funções de Interseção
 * ============================================================================ */

/**
 * Calcula o ponto de interseção entre um raio e um segmento.
 * O raio parte de 'origem' passando por 'direcao'.
 * 
 * @param origem Origem do raio
 * @param direcao Ponto que define a direção do raio
 * @param seg Segmento a testar
 * @param resultado Saída: ponto de interseção (se houver)
 * @return 1 se há interseção, 0 caso contrário
 * 
 * @note O resultado deve ser destruído pelo chamador!
 */
int intersecao_raio_segmento(Ponto origem, Ponto direcao, Segmento seg, Ponto *resultado);

/**
 * Verifica se dois segmentos (p1,p2) e (p3,p4) se intersectam.
 * 
 * @param p1 Inicio do seg1
 * @param p2 Fim do seg1
 * @param p3 Inicio do seg2
 * @param p4 Fim do seg2
 * @return 1 se intersectam, 0 caso contrário
 */
int intersecao_segmentos(Ponto p1, Ponto p2, Ponto p3, Ponto p4);

/**
 * Verifica se um ponto está "à frente" de um segmento do ponto de vista da origem.
 * Usado para determinar se um segmento bloqueia a visão.
 * 
 * @param origem Ponto de vista
 * @param ponto Ponto a testar
 * @param seg Segmento bloqueador
 * @return 1 se o ponto está à frente (mais perto), 0 se está atrás (bloqueado)
 */
int ponto_na_frente(Ponto origem, Ponto ponto, Segmento seg);

/**
 * Calcula a distância de um ponto até um segmento ao longo de uma direção.
 * Usado para comparar segmentos na árvore de sweep.
 * 
 * @param origem Origem do raio
 * @param angulo Ângulo do raio (radianos)
 * @param seg Segmento
 * @return Distância até o segmento, ou INFINITY se não intersecta
 */
double distancia_raio_segmento(Ponto origem, double angulo, Segmento seg);

/* ============================================================================
 * Funções de Comparação para Ordenação
 * ============================================================================ */

/**
 * Compara dois segmentos pela distância ao longo do raio atual.
 * Usado na árvore de segmentos ativos.
 * 
 * @param origem Ponto de vista (origem do raio)
 * @param angulo Ângulo atual da varredura
 * @param seg1 Primeiro segmento
 * @param seg2 Segundo segmento
 * @return < 0 se seg1 mais perto, > 0 se seg2 mais perto, 0 se equidistantes
 */
int comparar_segmentos_raio(Ponto origem, double angulo, Segmento seg1, Segmento seg2);

/* ============================================================================
 * Funções de Ponto no Polígono
 * ============================================================================ */

/**
 * Verifica se um ponto está dentro de um polígono.
 * Usa o algoritmo de Ray Casting (par/ímpar).
 * 
 * @param px Coordenada X do ponto
 * @param py Coordenada Y do ponto
 * @param vertices Array de coordenadas do polígono [x0, y0, x1, y1, ...]
 * @param num_vertices Número de vértices do polígono
 * @return 1 se o ponto está dentro, 0 se está fora
 */
int ponto_no_poligono(double px, double py, double *vertices, int num_vertices);

/**
 * Verifica se uma forma está pelo menos parcialmente dentro do polígono.
 * Para círculos: verifica se o centro está dentro.
 * Para retângulos: verifica se algum canto está dentro.
 * Para linhas: verifica se algum extremo está dentro.
 * Para textos: verifica se a âncora está dentro.
 * 
 * @param forma A forma a testar
 * @param vertices Array de coordenadas do polígono
 * @param num_vertices Número de vértices
 * @return 1 se a forma está (parcialmente) dentro, 0 caso contrário
 */
int forma_no_poligono(void *forma, double *vertices, int num_vertices);

#endif /* CALCULOS_H */
