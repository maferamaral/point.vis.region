/* arvore.c
 *
 * Implementação da Árvore de Segmentos Ativos
 * Usa uma árvore binária de busca simples.
 * 
 * A ordenação é dinâmica: segmentos são comparados pela distância
 * ao ponto de vista no ângulo atual da varredura.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "arvore.h"
#include "../geometria/calculos/calculos.h"

/* ============================================================================
 * Estruturas Internas
 * ============================================================================ */

/* Nó da árvore */
typedef struct no_arvore
{
    Segmento segmento;
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

/**
 * Cria um novo nó.
 */
static NoArvore* criar_no(Segmento seg)
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
static int comparar_segmentos(ArvoreInternal *arv, Segmento seg1, Segmento seg2)
{
    if (seg1 == seg2) return 0;
    
    return comparar_segmentos_raio(arv->origem, arv->angulo, seg1, seg2);
}

/**
 * Encontra o nó com menor valor (mais à esquerda).
 */
static NoArvore* encontrar_minimo(NoArvore *no)
{
    if (no == NULL) return NULL;
    
    while (no->esquerda != NULL)
    {
        no = no->esquerda;
    }
    return no;
}

/**
 * Encontra o nó sucessor (próximo na ordem).
 */
static NoArvore* encontrar_sucessor(NoArvore *no)
{
    if (no == NULL) return NULL;
    
    /* Se tem filho direito, é o mínimo do filho direito */
    if (no->direita != NULL)
    {
        return encontrar_minimo(no->direita);
    }
    
    /* Senão, sobe até encontrar onde viemos da esquerda */
    NoArvore *pai = no->pai;
    while (pai != NULL && no == pai->direita)
    {
        no = pai;
        pai = pai->pai;
    }
    return pai;
}



/**
 * Transplanta uma subárvore (usada na remoção).
 */
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

/**
 * Destroi recursivamente os nós da árvore.
 */
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

ArvoreSegmentos arvore_criar(Ponto origem)
{
    if (origem == NULL) return NULL;
    
    ArvoreInternal *arv = (ArvoreInternal*)malloc(sizeof(ArvoreInternal));
    if (arv == NULL)
    {
        fprintf(stderr, "Erro: falha ao alocar árvore de segmentos.\n");
        return NULL;
    }
    
    arv->raiz = NULL;
    arv->origem = origem;
    arv->angulo = 0.0;
    arv->tamanho = 0;
    
    return (ArvoreSegmentos)arv;
}

void arvore_destruir(ArvoreSegmentos arvore)
{
    ArvoreInternal *arv = (ArvoreInternal*)arvore;
    if (arv == NULL) return;
    
    destruir_nos(arv->raiz);
    free(arv);
}

void arvore_definir_angulo(ArvoreSegmentos arvore, double angulo)
{
    ArvoreInternal *arv = (ArvoreInternal*)arvore;
    if (arv != NULL)
    {
        arv->angulo = angulo;
    }
}

int arvore_inserir(ArvoreSegmentos arvore, Segmento seg)
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

int arvore_remover(ArvoreSegmentos arvore, Segmento seg)
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

Segmento arvore_obter_primeiro(ArvoreSegmentos arvore)
{
    ArvoreInternal *arv = (ArvoreInternal*)arvore;
    if (arv == NULL || arv->raiz == NULL) return NULL;
    
    /* IMPORTANTE: A BST não é reorganizada quando o ângulo muda.
     * Precisamos fazer busca linear por todos os segmentos para encontrar
     * o que está mais próximo da origem no ângulo ATUAL. */
    
    Segmento mais_proximo = NULL;
    double menor_dist = 1e18;
    
    /* Busca usando pilha iterativa */
    NoArvore *stack[1000];
    int stack_top = 0;
    stack[stack_top++] = arv->raiz;
    
    while (stack_top > 0)
    {
        NoArvore *atual = stack[--stack_top];
        
        double dist = distancia_raio_segmento(arv->origem, arv->angulo, atual->segmento);
        if (dist < menor_dist)
        {
            menor_dist = dist;
            mais_proximo = atual->segmento;
        }
        
        if (atual->direita != NULL) stack[stack_top++] = atual->direita;
        if (atual->esquerda != NULL) stack[stack_top++] = atual->esquerda;
    }
    
    return mais_proximo;
}

Segmento arvore_obter_proximo(ArvoreSegmentos arvore, Segmento seg)
{
    ArvoreInternal *arv = (ArvoreInternal*)arvore;
    if (arv == NULL || seg == NULL) return NULL;
    
    /* Busca linear para encontrar o nó */
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
    
    if (no == NULL) return NULL;
    
    NoArvore *sucessor = encontrar_sucessor(no);
    return sucessor ? sucessor->segmento : NULL;
}

int arvore_vazia(ArvoreSegmentos arvore)
{
    ArvoreInternal *arv = (ArvoreInternal*)arvore;
    return (arv == NULL || arv->raiz == NULL);
}

int arvore_tamanho(ArvoreSegmentos arvore)
{
    ArvoreInternal *arv = (ArvoreInternal*)arvore;
    return arv ? arv->tamanho : 0;
}
