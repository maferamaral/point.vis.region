/**
 * arvore_seg.h
 * 
 * TAD Árvore de Segmentos Ativos (AVL)
 * Árvore balanceada para o algoritmo de varredura angular.
 * Ordena segmentos por TOPOLOGIA RELATIVA (área orientada), NÃO por distância.
 */

#ifndef ARVORE_SEG_H
#define ARVORE_SEG_H

#include "../geometria/geometria.h"

/* Tipo opaco para Árvore de Segmentos */
typedef void* ArvoreSegmentos;

/* ============================================================================
 * Criação e Destruição
 * ============================================================================ */

/**
 * Cria uma nova árvore de segmentos.
 * @param origem Ponto de observação (fonte de luz)
 * @return Nova árvore, ou NULL em caso de erro
 */
ArvoreSegmentos arvore_seg_criar(Ponto origem);

/**
 * Destrói a árvore de segmentos.
 * NÃO destrói os segmentos armazenados (apenas os nós).
 * @param arvore Árvore a ser destruída
 */
void arvore_seg_destruir(ArvoreSegmentos arvore);

/* ============================================================================
 * Modificação
 * ============================================================================ */

/**
 * Atualiza o ângulo atual da varredura.
 * DEVE ser chamada antes de inserções/buscas.
 * @param arvore Árvore de segmentos
 * @param angulo Novo ângulo em radianos
 */
void arvore_seg_definir_angulo(ArvoreSegmentos arvore, double angulo);

/**
 * Insere um segmento na árvore.
 * @param arvore Árvore de segmentos
 * @param seg Segmento a inserir
 * @return 1 se inseriu com sucesso, 0 caso contrário
 */
int arvore_seg_inserir(ArvoreSegmentos arvore, Segmento *seg);

/**
 * Remove um segmento da árvore.
 * @param arvore Árvore de segmentos
 * @param seg Segmento a remover (por ponteiro)
 * @return 1 se removeu com sucesso, 0 se não encontrou
 */
int arvore_seg_remover(ArvoreSegmentos arvore, Segmento *seg);

/* ============================================================================
 * Consulta
 * ============================================================================ */

/**
 * Obtém o segmento mais próximo da origem (o "biombo").
 * @param arvore Árvore de segmentos
 * @return Segmento mais próximo, ou NULL se vazia
 */
Segmento* arvore_seg_obter_primeiro(ArvoreSegmentos arvore);

/**
 * Verifica se a árvore está vazia.
 * @return 1 se vazia, 0 caso contrário
 */
int arvore_seg_vazia(ArvoreSegmentos arvore);

/**
 * Obtém o número de segmentos na árvore.
 * @return Quantidade de segmentos
 */
int arvore_seg_tamanho(ArvoreSegmentos arvore);

#endif /* ARVORE_SEG_H */
