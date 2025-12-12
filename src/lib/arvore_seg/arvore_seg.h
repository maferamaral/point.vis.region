/*
 * arvore_seg.h
 * 
 * TAD Árvore de Segmentos Ativos
 * Árvore binária de busca especializada para o algoritmo de varredura angular.
 * Ordena segmentos pela distância ao ponto de vista no ângulo atual.
 */

#ifndef ARVORE_SEG_H
#define ARVORE_SEG_H

#include "../geometria/geometria.h"

/* Tipo opaco para Árvore de Segmentos */
typedef void* ArvoreSegmentos;

/* ============================================================================
 * Funções de Criação e Destruição
 * ============================================================================ */

/**
 * Cria uma nova árvore de segmentos.
 * @param origem Ponto de vista (origem dos raios)
 * @return Nova árvore, ou NULL em caso de erro
 */
ArvoreSegmentos arvore_seg_criar(Ponto origem);

/**
 * Destroi a árvore de segmentos.
 * @param arvore Árvore a ser destruída
 * @note NÃO destrói os segmentos armazenados!
 */
void arvore_seg_destruir(ArvoreSegmentos arvore);

/* ============================================================================
 * Funções de Modificação
 * ============================================================================ */

/**
 * Atualiza o ângulo atual da varredura.
 * DEVE ser chamada antes de inserções/buscas para manter ordenação correta.
 * @param arvore Árvore de segmentos
 * @param angulo Novo ângulo (radianos)
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
 * @param seg Segmento a remover
 * @return 1 se removeu com sucesso, 0 se não encontrou
 */
int arvore_seg_remover(ArvoreSegmentos arvore, Segmento *seg);

/* ============================================================================
 * Funções de Consulta
 * ============================================================================ */

/**
 * Obtém o segmento mais próximo da origem (o "biombo").
 * @param arvore Árvore de segmentos
 * @return Segmento mais próximo, ou NULL se árvore vazia
 */
Segmento* arvore_seg_obter_primeiro(ArvoreSegmentos arvore);

/**
 * Verifica se a árvore está vazia.
 * @param arvore Árvore de segmentos
 * @return 1 se vazia, 0 caso contrário
 */
int arvore_seg_vazia(ArvoreSegmentos arvore);

/**
 * Obtém o número de segmentos na árvore.
 * @param arvore Árvore de segmentos
 * @return Quantidade de segmentos
 */
int arvore_seg_tamanho(ArvoreSegmentos arvore);

#endif /* ARVORE_SEG_H */
