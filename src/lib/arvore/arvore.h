/* arvore.h
 *
 * TAD Árvore de Segmentos Ativos
 * Árvore binária de busca para o algoritmo de varredura angular.
 * Ordena segmentos pela distância ao ponto de vista.
 */

#ifndef ARVORE_H
#define ARVORE_H

#include "../geometria/segmento/segmento.h"
#include "../geometria/ponto/ponto.h"

/* Tipo opaco para Árvore de Segmentos */
typedef void* ArvoreSegmentos;

/* ============================================================================
 * Funções de Criação e Destruição
 * ============================================================================ */

/**
 * Cria uma nova árvore de segmentos.
 * @param origem Ponto de vista (origem dos raios)
 * @return Nova árvore, ou NULL em caso de erro
 * 
 * @note A árvore mantém referência à origem para comparações.
 */
ArvoreSegmentos arvore_criar(Ponto origem);

/**
 * Destroi a árvore de segmentos.
 * @param arvore Árvore a ser destruída
 * 
 * @note NÃO destrói os segmentos armazenados!
 *       O chamador é responsável pela memória dos segmentos.
 */
void arvore_destruir(ArvoreSegmentos arvore);

/* ============================================================================
 * Funções de Modificação
 * ============================================================================ */

/**
 * Atualiza o ângulo atual da varredura.
 * Isso afeta a ordenação dos segmentos na árvore.
 * @param arvore Árvore de segmentos
 * @param angulo Novo ângulo (radianos)
 */
void arvore_definir_angulo(ArvoreSegmentos arvore, double angulo);

/**
 * Insere um segmento na árvore.
 * @param arvore Árvore de segmentos
 * @param seg Segmento a inserir
 * @return 1 se inseriu com sucesso, 0 caso contrário
 */
int arvore_inserir(ArvoreSegmentos arvore, Segmento seg);

/**
 * Remove um segmento da árvore.
 * @param arvore Árvore de segmentos
 * @param seg Segmento a remover
 * @return 1 se removeu com sucesso, 0 se não encontrou
 */
int arvore_remover(ArvoreSegmentos arvore, Segmento seg);

/* ============================================================================
 * Funções de Consulta
 * ============================================================================ */

/**
 * Obtém o segmento mais próximo da origem (o "biombo").
 * @param arvore Árvore de segmentos
 * @return Segmento mais próximo, ou NULL se árvore vazia
 */
Segmento arvore_obter_primeiro(ArvoreSegmentos arvore);

/**
 * Obtém o próximo segmento após um dado segmento (logo atrás dele).
 * @param arvore Árvore de segmentos
 * @param seg Segmento de referência
 * @return Próximo segmento, ou NULL se não existe
 */
Segmento arvore_obter_proximo(ArvoreSegmentos arvore, Segmento seg);

/**
 * Verifica se a árvore está vazia.
 * @param arvore Árvore de segmentos
 * @return 1 se vazia, 0 caso contrário
 */
int arvore_vazia(ArvoreSegmentos arvore);

/**
 * Obtém o número de segmentos na árvore.
 * @param arvore Árvore de segmentos
 * @return Quantidade de segmentos
 */
int arvore_tamanho(ArvoreSegmentos arvore);

#endif /* ARVORE_H */
