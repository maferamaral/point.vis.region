/* segmento.h
 *
 * TAD Segmento - Representação de segmento de reta
 * Usado no algoritmo de visibilidade como "anteparo"
 */

#ifndef SEGMENTO_H
#define SEGMENTO_H

#include "../ponto/ponto.h"

/* Tipo opaco para Segmento */
typedef void* Segmento;

/* Tipo de vértice (para eventos do sweep) */
typedef enum {
    VERTICE_INICIO,  /* Início do segmento */
    VERTICE_FIM      /* Fim do segmento */
} TipoVertice;

/* ============================================================================
 * Funções de Criação e Destruição
 * ============================================================================ */

/**
 * Cria um segmento a partir de coordenadas.
 * @param id Identificador único do segmento
 * @param id_original Identificador da forma original
 * @param x1 Coordenada X do ponto inicial
 * @param y1 Coordenada Y do ponto inicial
 * @param x2 Coordenada X do ponto final
 * @param y2 Coordenada Y do ponto final
 * @param cor Cor do segmento (hex ou nome)
 * @return Novo segmento, ou NULL em caso de erro
 */
Segmento criar_segmento(int id, int id_original, double x1, double y1, double x2, double y2, const char *cor);

/**
 * Cria um segmento a partir de dois pontos.
 * @param id Identificador único do segmento
 * @param id_original Identificador da forma original
 * @param p1 Ponto inicial (será clonado)
 * @param p2 Ponto final (será clonado)
 * @param cor Cor do segmento
 * @return Novo segmento, ou NULL em caso de erro
 */
Segmento criar_segmento_pontos(int id, int id_original, Ponto p1, Ponto p2, const char *cor);

/**
 * Clona um segmento.
 * @param seg Segmento a ser clonado
 * @return Nova cópia do segmento, ou NULL em caso de erro
 */
Segmento clonar_segmento(Segmento seg);

/**
 * Destroi um segmento.
 * @param seg Segmento a ser destruído
 */
void destruir_segmento(Segmento seg);

/* ============================================================================
 * Funções de Acesso
 * ============================================================================ */

/**
 * Obtém o ID do segmento (identificador único).
 */
int get_segmento_id(Segmento seg);

/**
 * Obtém o ID original (identificador da forma).
 */
int get_segmento_id_original(Segmento seg);

/**
 * Obtém a cor do segmento.
 */
const char* get_segmento_cor(Segmento seg);

/**
 * Obtém o ponto inicial do segmento.
 * @note NÃO modifique nem destrua o ponto retornado!
 */
Ponto get_segmento_p1(Segmento seg);

/**
 * Obtém o ponto final do segmento.
 * @note NÃO modifique nem destrua o ponto retornado!
 */
Ponto get_segmento_p2(Segmento seg);

/**
 * Obtém coordenada X1 do ponto inicial.
 */
double get_segmento_x1(Segmento seg);

/**
 * Obtém coordenada Y1 do ponto inicial.
 */
double get_segmento_y1(Segmento seg);

/**
 * Obtém coordenada X2 do ponto final.
 */
double get_segmento_x2(Segmento seg);

/**
 * Obtém coordenada Y2 do ponto final.
 */
double get_segmento_y2(Segmento seg);

/* ============================================================================
 * Funções Geométricas
 * ============================================================================ */

/**
 * Calcula o comprimento do segmento.
 */
double segmento_comprimento(Segmento seg);

/**
 * Divide um segmento em um ponto de interseção.
 * @param seg Segmento a ser dividido
 * @param ponto Ponto de divisão
 * @param seg1 Saída: primeiro segmento (p1 até ponto)
 * @param seg2 Saída: segundo segmento (ponto até p2)
 * @return 1 se dividiu com sucesso, 0 caso contrário
 */
int segmento_dividir(Segmento seg, Ponto ponto, Segmento *seg1, Segmento *seg2);

#endif /* SEGMENTO_H */
