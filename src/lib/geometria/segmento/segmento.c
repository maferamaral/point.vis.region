/* segmento.c
 *
 * Implementação do TAD Segmento
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "segmento.h"
#include <string.h>
#include "../ponto/ponto.h"

/* ============================================================================
 * Estrutura Interna (Ponteiro Opaco)
 * ============================================================================ */

typedef struct segmento_internal
{
    int id;           /* ID único do segmento */
    int id_original;  /* ID da forma original */
    Ponto p1;         /* Ponto inicial */
    Ponto p2;         /* Ponto final */
    char cor[32];     /* Cor do segmento */
} SegmentoInternal;

/* ============================================================================
 * Implementação das Funções de Criação e Destruição
 * ============================================================================ */

Segmento criar_segmento(int id, int id_original, double x1, double y1, double x2, double y2, const char *cor)
{
    SegmentoInternal *seg = (SegmentoInternal*)malloc(sizeof(SegmentoInternal));
    if (seg == NULL)
    {
        fprintf(stderr, "Erro: falha ao alocar segmento.\n");
        return NULL;
    }
    
    seg->id = id;
    seg->id_original = id_original;
    seg->p1 = criar_ponto(x1, y1);
    seg->p2 = criar_ponto(x2, y2);
    
    if (cor != NULL)
    {
        strncpy(seg->cor, cor, 31);
        seg->cor[31] = '\0';
    }
    else
    {
        strcpy(seg->cor, "black");
    }
    
    if (seg->p1 == NULL || seg->p2 == NULL)
    {
        destruir_ponto(seg->p1);
        destruir_ponto(seg->p2);
        free(seg);
        return NULL;
    }
    
    return (Segmento)seg;
}

Segmento criar_segmento_pontos(int id, int id_original, Ponto p1, Ponto p2, const char *cor)
{
    if (p1 == NULL || p2 == NULL) return NULL;
    
    return criar_segmento(id, id_original,
                          get_ponto_x(p1), get_ponto_y(p1),
                          get_ponto_x(p2), get_ponto_y(p2),
                          cor);
}

Segmento clonar_segmento(Segmento segmento)
{
    SegmentoInternal *seg = (SegmentoInternal*)segmento;
    if (seg == NULL) return NULL;
    
    return criar_segmento(seg->id, seg->id_original,
                          get_ponto_x(seg->p1), get_ponto_y(seg->p1),
                          get_ponto_x(seg->p2), get_ponto_y(seg->p2),
                          seg->cor);
}

void destruir_segmento(Segmento segmento)
{
    SegmentoInternal *seg = (SegmentoInternal*)segmento;
    if (seg == NULL) return;
    
    destruir_ponto(seg->p1);
    destruir_ponto(seg->p2);
    free(seg);
}

/* ============================================================================
 * Implementação das Funções de Acesso
 * ============================================================================ */

int get_segmento_id(Segmento segmento)
{
    SegmentoInternal *seg = (SegmentoInternal*)segmento;
    return seg ? seg->id : -1;
}

int get_segmento_id_original(Segmento segmento)
{
    SegmentoInternal *seg = (SegmentoInternal*)segmento;
    return seg ? seg->id_original : -1;
}

const char* get_segmento_cor(Segmento segmento)
{
    SegmentoInternal *seg = (SegmentoInternal*)segmento;
    return seg ? seg->cor : "none";
}

Ponto get_segmento_p1(Segmento segmento)
{
    SegmentoInternal *seg = (SegmentoInternal*)segmento;
    return seg ? seg->p1 : NULL;
}

Ponto get_segmento_p2(Segmento segmento)
{
    SegmentoInternal *seg = (SegmentoInternal*)segmento;
    return seg ? seg->p2 : NULL;
}

double get_segmento_x1(Segmento segmento)
{
    SegmentoInternal *seg = (SegmentoInternal*)segmento;
    return seg ? get_ponto_x(seg->p1) : 0.0;
}

double get_segmento_y1(Segmento segmento)
{
    SegmentoInternal *seg = (SegmentoInternal*)segmento;
    return seg ? get_ponto_y(seg->p1) : 0.0;
}

double get_segmento_x2(Segmento segmento)
{
    SegmentoInternal *seg = (SegmentoInternal*)segmento;
    return seg ? get_ponto_x(seg->p2) : 0.0;
}

double get_segmento_y2(Segmento segmento)
{
    SegmentoInternal *seg = (SegmentoInternal*)segmento;
    return seg ? get_ponto_y(seg->p2) : 0.0;
}

/* ============================================================================
 * Implementação das Funções Geométricas
 * ============================================================================ */

double segmento_comprimento(Segmento segmento)
{
    SegmentoInternal *seg = (SegmentoInternal*)segmento;
    if (seg == NULL) return 0.0;
    
    return ponto_distancia(seg->p1, seg->p2);
}

int segmento_dividir(Segmento segmento, Ponto ponto, Segmento *seg1, Segmento *seg2)
{
    SegmentoInternal *seg = (SegmentoInternal*)segmento;
    if (seg == NULL || ponto == NULL || seg1 == NULL || seg2 == NULL)
    {
        return 0;
    }
    
    /* Primeiro segmento: p1 até ponto de divisão */
    *seg1 = criar_segmento(seg->id, seg->id_original,
                           get_ponto_x(seg->p1), get_ponto_y(seg->p1),
                           get_ponto_x(ponto), get_ponto_y(ponto),
                           seg->cor);
    
    /* Segundo segmento: ponto de divisão até p2 */
    *seg2 = criar_segmento(seg->id, seg->id_original,
                           get_ponto_x(ponto), get_ponto_y(ponto),
                           get_ponto_x(seg->p2), get_ponto_y(seg->p2),
                           seg->cor);
    
    return (*seg1 != NULL && *seg2 != NULL);
}
