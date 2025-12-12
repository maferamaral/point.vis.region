/* poligono.h */

#ifndef POLIGONO_H
#define POLIGONO_H

#include "../geometria/ponto/ponto.h"
#include "../utils/lista/lista.h"

/* Tipo de dados opaco para o Polígono */
typedef void* Poligono;

/**
 * Cria um novo polígono vazio.
 * @return Instância de Poligono ou NULL em caso de erro.
 */
Poligono poligono_criar();

/**
 * Destrói o polígono e libera todos os recursos.
 * @param p Polígono a ser destruído.
 */
void poligono_destruir(Poligono p);

/**
 * Insere um vértice no final da sequência do polígono.
 * @param p Polígono.
 * @param x Coordenada X.
 * @param y Coordenada Y.
 */
void poligono_inserir_vertice(Poligono p, double x, double y);

/**
 * Retorna a quantidade de vértices no polígono.
 * @param p Polígono.
 * @return Número de vértices.
 */
int poligono_qtd_vertices(Poligono p);

/**
 * Obtém o i-ésimo vértice (Ponto) do polígono para apenas leitura.
 * @param p Polígono.
 * @param indice Índice do vértice.
 * @return Ponto (cópia ou ponteiro temporário, cuidado).
 * @note Com a alteração para array, isso retorna uma cópia de Ponto efêmera ou null.
 */
Ponto poligono_get_vertice(Poligono p, int indice);

/**
 * Exporta os vértices para um vetor de doubles [x0, y0, x1, y1, ...].
 * @param p Polígono.
 * @param vertices_out Endereço de um ponteiro de double que receberá o vetor alocado (MALLOC).
 * @return Número de vértices exportados (tamanho do vetor será 2 * retorno).
 * @note O chamador é responsável por dar free(*vertices_out).
 * @deprecated Use poligono_get_vertices_ref para performance.
 */
int poligono_obter_vertices_array(Poligono p, double **vertices_out);

/**
 * Retorna ponteiro direto para o array interno de vértices [x0, y0, ...].
 * @param p Polígono.
 * @param num_vertices Ponteiro para receber a quantidade de vértices.
 * @return Ponteiro para array interno (NAO DAR FREE).
 */
double* poligono_get_vertices_ref(Poligono p, int *num_vertices);

/**
 * Retorna a lista interna de pontos.
 * @param p Polígono.
 * @return Lista de pontos (gerada sob demanda!).
 * @note Uso legado. Lento.
 */
LinkedList poligono_obter_lista(Poligono p);

#endif
