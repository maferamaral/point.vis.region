/*
 * arvore_seg.c
 * 
 * Implementação da Árvore de Segmentos Ativos
 * Usa uma árvore binária de busca com estado interno (origem e ângulo).
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "arvore_seg.h"

#define EPSILON 1e-9

/* ============================================================================
 * Estruturas Internas
 * ============================================================================ */

/* Nó da árvore */
typedef struct no_arvore
{
    Segmento *segmento;
    struct no_arvore *esquerda;
    struct no_arvore *direita;
    struct no_arvore *pai;
} NoArvore;

/* Estrutura principal da árvore */
typedef struct arvore_internal
{
    NoArvore *raiz;
    Ponto origem;       /* Ponto de vista */
    double angulo;      /* Ângulo atual da varredura */
    int tamanho;
} ArvoreInternal;

/* ============================================================================
 * Funções Auxiliares
 * ============================================================================ */

static NoArvore* criar_no(Segmento *seg)
{
    NoArvore *no = (NoArvore*)malloc(sizeof(NoArvore));
    if (no == NULL) return NULL;
    
    no->segmento = seg;
    no->esquerda = NULL;
    no->direita = NULL;
    no->pai = NULL;
    
    return no;
}

/**
 * Compara dois segmentos pela distância no ângulo atual.
 * @return < 0 se seg1 mais perto, > 0 se seg2 mais perto
 */
static int comparar_segmentos(ArvoreInternal *arv, Segmento *seg1, Segmento *seg2)
{
    if (seg1 == seg2) return 0;
    
    double d1 = distancia_raio_segmento(arv->origem, arv->angulo, *seg1);
    double d2 = distancia_raio_segmento(arv->origem, arv->angulo, *seg2);
    
    // Fallback para distâncias inválidas
    if (isinf(d1) || isnan(d1)) d1 = 1e18;
    if (isinf(d2) || isnan(d2)) d2 = 1e18;
    
    if (fabs(d1 - d2) > EPSILON)
    {
        return (d1 < d2) ? -1 : 1;
    }
    
    // Desempate por endereço de memória
    return (seg1 < seg2) ? -1 : 1;
}

static NoArvore* encontrar_minimo(NoArvore *no)
{
    if (no == NULL) return NULL;
    
    while (no->esquerda != NULL)
    {
        no = no->esquerda;
    }
    return no;
}

static void transplantar(ArvoreInternal *arv, NoArvore *u, NoArvore *v)
{
    if (u->pai == NULL)
    {
        arv->raiz = v;
    }
    else if (u == u->pai->esquerda)
    {
        u->pai->esquerda = v;
    }
    else
    {
        u->pai->direita = v;
    }
    
    if (v != NULL)
    {
        v->pai = u->pai;
    }
}

static void destruir_nos(NoArvore *no)
{
    if (no == NULL) return;
    
    destruir_nos(no->esquerda);
    destruir_nos(no->direita);
    free(no);
}

/* ============================================================================
 * Implementação das Funções Públicas
 * ============================================================================ */

ArvoreSegmentos arvore_seg_criar(Ponto origem)
{
    ArvoreInternal *arv = (ArvoreInternal*)malloc(sizeof(ArvoreInternal));
    if (arv == NULL)
    {
        return NULL;
    }
    
    arv->raiz = NULL;
    arv->origem = origem;
    arv->angulo = 0.0;
    arv->tamanho = 0;
    
    return (ArvoreSegmentos)arv;
}

void arvore_seg_destruir(ArvoreSegmentos arvore)
{
    ArvoreInternal *arv = (ArvoreInternal*)arvore;
    if (arv == NULL) return;
    
    destruir_nos(arv->raiz);
    free(arv);
}

void arvore_seg_definir_angulo(ArvoreSegmentos arvore, double angulo)
{
    ArvoreInternal *arv = (ArvoreInternal*)arvore;
    if (arv != NULL)
    {
        arv->angulo = angulo;
    }
}

int arvore_seg_inserir(ArvoreSegmentos arvore, Segmento *seg)
{
    ArvoreInternal *arv = (ArvoreInternal*)arvore;
    if (arv == NULL || seg == NULL) return 0;
    
    NoArvore *novo = criar_no(seg);
    if (novo == NULL) return 0;
    
    /* Inserção padrão da BST */
    NoArvore *pai = NULL;
    NoArvore *atual = arv->raiz;
    
    while (atual != NULL)
    {
        pai = atual;
        int cmp = comparar_segmentos(arv, seg, atual->segmento);
        
        if (cmp < 0)
        {
            atual = atual->esquerda;
        }
        else
        {
            atual = atual->direita;
        }
    }
    
    novo->pai = pai;
    
    if (pai == NULL)
    {
        arv->raiz = novo;
    }
    else if (comparar_segmentos(arv, seg, pai->segmento) < 0)
    {
        pai->esquerda = novo;
    }
    else
    {
        pai->direita = novo;
    }
    
    arv->tamanho++;
    return 1;
}

int arvore_seg_remover(ArvoreSegmentos arvore, Segmento *seg)
{
    ArvoreInternal *arv = (ArvoreInternal*)arvore;
    if (arv == NULL || seg == NULL) return 0;
    
    /* Busca linear para encontrar o nó (por ponteiro do segmento) */
    NoArvore *no = NULL;
    NoArvore *stack[1000];
    int stack_top = 0;
    
    if (arv->raiz != NULL)
    {
        stack[stack_top++] = arv->raiz;
    }
    
    while (stack_top > 0)
    {
        NoArvore *atual = stack[--stack_top];
        
        if (atual->segmento == seg)
        {
            no = atual;
            break;
        }
        
        if (atual->direita != NULL) stack[stack_top++] = atual->direita;
        if (atual->esquerda != NULL) stack[stack_top++] = atual->esquerda;
    }
    
    if (no == NULL) return 0;
    
    /* Remoção padrão da BST */
    if (no->esquerda == NULL)
    {
        transplantar(arv, no, no->direita);
    }
    else if (no->direita == NULL)
    {
        transplantar(arv, no, no->esquerda);
    }
    else
    {
        NoArvore *sucessor = encontrar_minimo(no->direita);
        
        if (sucessor->pai != no)
        {
            transplantar(arv, sucessor, sucessor->direita);
            sucessor->direita = no->direita;
            sucessor->direita->pai = sucessor;
        }
        
        transplantar(arv, no, sucessor);
        sucessor->esquerda = no->esquerda;
        sucessor->esquerda->pai = sucessor;
    }
    
    free(no);
    arv->tamanho--;
    return 1;
}

Segmento* arvore_seg_obter_primeiro(ArvoreSegmentos arvore)
{
    ArvoreInternal *arv = (ArvoreInternal*)arvore;
    if (arv == NULL || arv->raiz == NULL) return NULL;
    
    NoArvore *minimo = encontrar_minimo(arv->raiz);
    return minimo ? minimo->segmento : NULL;
}

int arvore_seg_vazia(ArvoreSegmentos arvore)
{
    ArvoreInternal *arv = (ArvoreInternal*)arvore;
    return (arv == NULL || arv->raiz == NULL);
}

int arvore_seg_tamanho(ArvoreSegmentos arvore)
{
    ArvoreInternal *arv = (ArvoreInternal*)arvore;
    return arv ? arv->tamanho : 0;
}
