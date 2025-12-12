/**
 * arvore_seg.c
 * 
 * Implementação da Árvore AVL de Segmentos Ativos.
 * Usa predicados topológicos (área orientada) para comparação estável.
 * 
 * REGRA DE ORDENAÇÃO (do professor):
 * S_r fica na sub-árvore direita de S_t se S_r estiver "à direita" 
 * (ou "atrás") de S_t em relação ao ponto de observação.
 * Isso é calculado usando Área Orientada/Determinante.
 */

#include "arvore_seg.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define EPSILON 1e-9

/* ============================================================================
 * Estruturas Internas
 * ============================================================================ */

typedef struct no_arvore {
    Segmento *segmento;
    struct no_arvore *esquerda;
    struct no_arvore *direita;
    int altura;
} NoArvore;

typedef struct {
    NoArvore *raiz;
    Ponto origem;
    double angulo;
    int tamanho;
} ArvoreInternal;

/* ============================================================================
 * Funções AVL Básicas
 * ============================================================================ */

static int max_int(int a, int b)
{
    return (a > b) ? a : b;
}

static int obter_altura(NoArvore *no)
{
    return (no == NULL) ? 0 : no->altura;
}

static int fator_balanceamento(NoArvore *no)
{
    if (no == NULL) return 0;
    return obter_altura(no->esquerda) - obter_altura(no->direita);
}

static void atualizar_altura(NoArvore *no)
{
    if (no != NULL)
    {
        no->altura = 1 + max_int(obter_altura(no->esquerda), obter_altura(no->direita));
    }
}

static NoArvore* criar_no(Segmento *seg)
{
    NoArvore *no = malloc(sizeof(NoArvore));
    if (no == NULL) return NULL;
    
    no->segmento = seg;
    no->esquerda = NULL;
    no->direita = NULL;
    no->altura = 1;
    
    return no;
}

/* ============================================================================
 * Rotações AVL
 * ============================================================================ */

static NoArvore* rotacao_direita(NoArvore *y)
{
    NoArvore *x = y->esquerda;
    NoArvore *T2 = x->direita;
    
    x->direita = y;
    y->esquerda = T2;
    
    atualizar_altura(y);
    atualizar_altura(x);
    
    return x;
}

static NoArvore* rotacao_esquerda(NoArvore *x)
{
    NoArvore *y = x->direita;
    NoArvore *T2 = y->esquerda;
    
    y->esquerda = x;
    x->direita = T2;
    
    atualizar_altura(x);
    atualizar_altura(y);
    
    return y;
}

/* ============================================================================
 * Predicado Topológico - O "Pulo do Gato"
 * ============================================================================ */

/**
 * Calcula a área orientada (determinante) para três pontos.
 * > 0: anti-horário (C está à esquerda de A->B)
 * < 0: horário (C está à direita de A->B)
 * = 0: colineares
 */
static double area_orientada(Ponto a, Ponto b, Ponto c)
{
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

/**
 * Compara dois segmentos para determinar qual está "na frente" (mais perto da origem).
 * 
 * A comparação é feita calculando a distância de cada segmento ao longo do raio atual.
 * O segmento mais próximo no raio atual está "na frente" e deve vir primeiro na árvore.
 * 
 * Para segmentos que não cruzam o raio atual, usamos predicados topológicos.
 */
static int comparar_segmentos(ArvoreInternal *arv, Segmento *seg1, Segmento *seg2)
{
    if (seg1 == seg2) return 0;
    
    Ponto origem = arv->origem;
    double angulo = arv->angulo;
    
    // Calcula distância de cada segmento no raio atual
    double d1 = distancia_raio_segmento(origem, angulo, *seg1);
    double d2 = distancia_raio_segmento(origem, angulo, *seg2);
    
    // Trata valores inválidos (segmento não cruza o raio)
    bool d1_valido = !isinf(d1) && !isnan(d1) && d1 > 0;
    bool d2_valido = !isinf(d2) && !isnan(d2) && d2 > 0;
    
    // Se ambos cruzam o raio, o mais próximo está na frente
    if (d1_valido && d2_valido)
    {
        if (fabs(d1 - d2) > EPSILON)
        {
            return (d1 < d2) ? -1 : 1;
        }
    }
    
    // Se apenas um cruza o raio, esse está na frente
    if (d1_valido && !d2_valido) return -1;
    if (d2_valido && !d1_valido) return 1;
    
    // Nenhum cruza o raio: usa predicado topológico
    // Verifica se seg1 está entre origem e seg2
    double area_origem1 = area_orientada(seg1->p1, seg1->p2, origem);
    double area_seg2_p1 = area_orientada(seg1->p1, seg1->p2, seg2->p1);
    double area_seg2_p2 = area_orientada(seg1->p1, seg1->p2, seg2->p2);
    
    // Se origem e seg2 estão em lados opostos de seg1, seg1 está na frente
    if (fabs(area_origem1) > EPSILON)
    {
        bool seg2_outro_lado = 
            (area_origem1 > 0 && area_seg2_p1 < -EPSILON && area_seg2_p2 < -EPSILON) ||
            (area_origem1 < 0 && area_seg2_p1 > EPSILON && area_seg2_p2 > EPSILON);
        
        if (seg2_outro_lado) return -1;
        
        bool seg2_mesmo_lado = 
            (area_origem1 > 0 && area_seg2_p1 > EPSILON && area_seg2_p2 > EPSILON) ||
            (area_origem1 < 0 && area_seg2_p1 < -EPSILON && area_seg2_p2 < -EPSILON);
        
        if (seg2_mesmo_lado) return 1;
    }
    
    // Teste inverso
    double area_origem2 = area_orientada(seg2->p1, seg2->p2, origem);
    double area_seg1_p1 = area_orientada(seg2->p1, seg2->p2, seg1->p1);
    double area_seg1_p2 = area_orientada(seg2->p1, seg2->p2, seg1->p2);
    
    if (fabs(area_origem2) > EPSILON)
    {
        bool seg1_outro_lado = 
            (area_origem2 > 0 && area_seg1_p1 < -EPSILON && area_seg1_p2 < -EPSILON) ||
            (area_origem2 < 0 && area_seg1_p1 > EPSILON && area_seg1_p2 > EPSILON);
        
        if (seg1_outro_lado) return 1;
        
        bool seg1_mesmo_lado = 
            (area_origem2 > 0 && area_seg1_p1 > EPSILON && area_seg1_p2 > EPSILON) ||
            (area_origem2 < 0 && area_seg1_p1 < -EPSILON && area_seg1_p2 < -EPSILON);
        
        if (seg1_mesmo_lado) return -1;
    }
    
    // Fallback: usa ponteiro para consistência
    return (seg1 < seg2) ? -1 : 1;
}


/* ============================================================================
 * Funções Auxiliares
 * ============================================================================ */

static NoArvore* encontrar_minimo(NoArvore *no)
{
    if (no == NULL) return NULL;
    while (no->esquerda != NULL) no = no->esquerda;
    return no;
}

static void destruir_nos(NoArvore *no)
{
    if (no == NULL) return;
    destruir_nos(no->esquerda);
    destruir_nos(no->direita);
    free(no);
}

/* ============================================================================
 * Inserção AVL
 * ============================================================================ */

static NoArvore* inserir_avl(ArvoreInternal *arv, NoArvore *no, Segmento *seg)
{
    if (no == NULL) return criar_no(seg);
    
    int cmp = comparar_segmentos(arv, seg, no->segmento);
    
    if (cmp < 0)
        no->esquerda = inserir_avl(arv, no->esquerda, seg);
    else
        no->direita = inserir_avl(arv, no->direita, seg);
    
    atualizar_altura(no);
    
    int fb = fator_balanceamento(no);
    
    // Esquerda-Esquerda
    if (fb > 1 && comparar_segmentos(arv, seg, no->esquerda->segmento) < 0)
        return rotacao_direita(no);
    
    // Direita-Direita
    if (fb < -1 && comparar_segmentos(arv, seg, no->direita->segmento) > 0)
        return rotacao_esquerda(no);
    
    // Esquerda-Direita
    if (fb > 1 && comparar_segmentos(arv, seg, no->esquerda->segmento) > 0)
    {
        no->esquerda = rotacao_esquerda(no->esquerda);
        return rotacao_direita(no);
    }
    
    // Direita-Esquerda
    if (fb < -1 && comparar_segmentos(arv, seg, no->direita->segmento) < 0)
    {
        no->direita = rotacao_direita(no->direita);
        return rotacao_esquerda(no);
    }
    
    return no;
}

/* ============================================================================
 * Remoção AVL
 * ============================================================================ */

static NoArvore* remover_avl(ArvoreInternal *arv, NoArvore *no, Segmento *seg)
{
    if (no == NULL) return NULL;
    
    // Busca por ponteiro (não por comparação)
    if (no->segmento == seg)
    {
        if (no->esquerda == NULL || no->direita == NULL)
        {
            NoArvore *temp = no->esquerda ? no->esquerda : no->direita;
            free(no);
            return temp;
        }
        else
        {
            NoArvore *sucessor = encontrar_minimo(no->direita);
            no->segmento = sucessor->segmento;
            no->direita = remover_avl(arv, no->direita, sucessor->segmento);
        }
    }
    else
    {
        // Busca em ambas subárvores (por ponteiro)
        no->esquerda = remover_avl(arv, no->esquerda, seg);
        no->direita = remover_avl(arv, no->direita, seg);
    }
    
    if (no == NULL) return NULL;
    
    atualizar_altura(no);
    
    int fb = fator_balanceamento(no);
    
    if (fb > 1 && fator_balanceamento(no->esquerda) >= 0)
        return rotacao_direita(no);
    
    if (fb > 1 && fator_balanceamento(no->esquerda) < 0)
    {
        no->esquerda = rotacao_esquerda(no->esquerda);
        return rotacao_direita(no);
    }
    
    if (fb < -1 && fator_balanceamento(no->direita) <= 0)
        return rotacao_esquerda(no);
    
    if (fb < -1 && fator_balanceamento(no->direita) > 0)
    {
        no->direita = rotacao_direita(no->direita);
        return rotacao_esquerda(no);
    }
    
    return no;
}

/* ============================================================================
 * Interface Pública
 * ============================================================================ */

ArvoreSegmentos arvore_seg_criar(Ponto origem)
{
    ArvoreInternal *arv = malloc(sizeof(ArvoreInternal));
    if (arv == NULL) return NULL;
    
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
    if (arv != NULL) arv->angulo = angulo;
}

int arvore_seg_inserir(ArvoreSegmentos arvore, Segmento *seg)
{
    ArvoreInternal *arv = (ArvoreInternal*)arvore;
    if (arv == NULL || seg == NULL) return 0;
    
    arv->raiz = inserir_avl(arv, arv->raiz, seg);
    arv->tamanho++;
    
    return 1;
}

int arvore_seg_remover(ArvoreSegmentos arvore, Segmento *seg)
{
    ArvoreInternal *arv = (ArvoreInternal*)arvore;
    if (arv == NULL || seg == NULL) return 0;
    
    int tamanho_antes = arv->tamanho;
    arv->raiz = remover_avl(arv, arv->raiz, seg);
    arv->tamanho--;
    
    return (arv->tamanho < tamanho_antes) ? 1 : 0;
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
