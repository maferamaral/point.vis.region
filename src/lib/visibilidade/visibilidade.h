/* visibilidade.h
 *
 * Algoritmo de Região de Visibilidade
 * Implementa varredura angular para calcular polígono visível.
 */

#ifndef VISIBILIDADE_H
#define VISIBILIDADE_H

#include <stdbool.h>
#include "../utils/lista/lista.h"
#include "../geometria/ponto/ponto.h"
#include "../geometria/segmento/segmento.h"
#include "../poligono/poligono.h" 

// Note: src/lib/poligono/poligono.h will be overwritten next.
// We assume Poligono type is 'Poligono' or opaque void*
// In srcAndre, typedef Poligono PoligonoVisibilidade;
// In src, we use PoligonoVisibilidade.

/* Alias para compatibilidade ou uso semântico */
typedef void* PoligonoVisibilidade;

/* ============================================================================
 * Funções Principais
 * ============================================================================ */

/**
 * Calcula o polígono de visibilidade a partir de um ponto.
 * 
 * @param origem Ponto de vista (de onde a "luz" emana)
 * @param segmentos Lista de Segmento (obstáculos)
 * @param min_x Limite mínimo X do cenário (bounding box)
 * @param min_y Limite mínimo Y do cenário
 * @param max_x Limite máximo X do cenário
 * @param max_y Limite máximo Y do cenário
 * @param tipo_ordenacao "qsort" ou "mergesort"
 * @param limiar_insertion Limiar para InsertionSort
 * @param segmentos_visiveis Lista opcional para preencher com segmentos visíveis (pode ser NULL)
 * @return Polígono de visibilidade (lista de pontos), ou NULL em caso de erro
 * 
 * @note O polígono retornado deve ser destruído com destruir_poligono_visibilidade()
 */
PoligonoVisibilidade calcular_visibilidade(Ponto origem, LinkedList segmentos,
                                            double min_x, double min_y,
                                            double max_x, double max_y,
                                            const char *tipo_ordenacao,
                                            int limiar_insertion);

// Note: I am merging `calcular_visibilidade` and `calcular_visibilidade_com_segmentos` logic 
// or providing same interface as srcAndre.
// srcAndre has both.

PoligonoVisibilidade calcular_visibilidade_com_segmentos(
    Ponto origem, LinkedList segmentos,
    double min_x, double min_y,
    double max_x, double max_y,
    const char *tipo_ordenacao,
    int limiar_insertion,
    LinkedList segmentos_visiveis);

/**
 * Destroi um polígono de visibilidade.
 * @param poligono Polígono a ser destruído
 */
void destruir_poligono_visibilidade(PoligonoVisibilidade poligono);

/* ============================================================================
 * Funções de Acesso ao Polígono
 * ============================================================================ */

/**
 * Obtém o número de vértices do polígono.
 */
int poligono_num_vertices(PoligonoVisibilidade poligono);

/**
 * Obtém o i-ésimo vértice do polígono.
 * @param poligono O polígono
 * @param indice Índice do vértice (0 a num_vertices-1)
 * @return Ponto do vértice, ou NULL se índice inválido
 * 
 * @note NÃO destrua o ponto retornado!
 */
Ponto poligono_obter_vertice(PoligonoVisibilidade poligono, int indice);

/**
 * Obtém a lista de pontos do polígono.
 * @return Lista de Ponto (não destrua!)
 */
LinkedList poligono_obter_vertices(PoligonoVisibilidade poligono);

/* ============================================================================
 * Funções de Conversão
 * ============================================================================ */

/**
 * Converte formas geométricas em segmentos para o algoritmo.
 * 
 * @param lista_formas Lista de Forma (círculos, retângulos, linhas, textos)
 * @param lista_segmentos Lista onde os segmentos serão inseridos
 * @param orientacao Para círculos: 'h' (horizontal) ou 'v' (vertical)
 * @return Número de segmentos criados
 * 
 * @note Os segmentos criados devem ser destruídos pelo chamador.
 */
int converter_formas_para_segmentos(LinkedList lista_formas, LinkedList lista_segmentos, char orientacao);

// Compatibility with src main.c calls
void visibilidade_set_sort_method(char method);

// Mapping OLD src function names to NEW srcAndre function names (adapters in .c)
PoligonoVisibilidade visibilidade_calcular(Ponto centro, LinkedList barreiras);
void visibilidade_destruir(PoligonoVisibilidade pol);
LinkedList visibilidade_obter_vertices(PoligonoVisibilidade pol);
bool visibilidade_ponto_atingido(PoligonoVisibilidade pol, Ponto p);
bool visibilidade_segmento_atingido(PoligonoVisibilidade pol, Ponto p1, Ponto p2);

#endif /* VISIBILIDADE_H */