#ifndef GEO_H
#define GEO_H

#include <stdio.h>
#include "../geometria/geometria.h"

// Forward declaration - using existing LinkedList API
typedef void* LinkedList;

typedef void *Geo;

/**
 * Cria a estrutura de dados para armazenar a cidade.
 */
Geo geo_criar();

/**
 * Lê o arquivo .geo e popula a cidade com as formas.
 * @param geo A estrutura da cidade
 * @param path Caminho completo do arquivo .geo
 */
void geo_ler(Geo geo, const char *path);

/**
 * Escreve todas as formas da cidade em formato SVG no arquivo fornecido.
 * O arquivo já deve estar aberto.
 * @param geo A cidade
 * @param svg Ponteiro para o arquivo .svg aberto
 */
void geo_escrever_svg(Geo geo, FILE *svg);

/**
 * Retorna a lista genérica de formas (para manipulações gerais no qry).
 * @return Lista contendo ElementoGeo* (interno) ou void* das formas.
 */
LinkedList geo_get_formas(Geo geo);

/**
 * [IMPORTANTE PARA VISIBILIDADE]
 * Converte as barreiras da cidade (paredes de Retângulos e Linhas) 
 * em uma lista de Segmentos matemáticos.
 * @param geo A cidade
 * @return Uma Lista nova contendo elementos do tipo 'Segmento' (struct da lib geometria)
 * Nota: Quem chamar esta função deve destruir a lista retornada depois.
 */
LinkedList geo_obter_todas_barreiras(Geo geo);

/**
 * [IMPORTANTE PARA VISIBILIDADE]
 * Gera uma lista de 4 segmentos (retângulo) que envolve todas as formas da cidade.
 * Essencial para criar o "Universo" ou "Biombo" do algoritmo de visibilidade.
 * @param geo A estrutura da cidade contendo as formas.
 * @param margem Valor de folga (padding) para expandir a caixa envolvente (ex: 500.0).
 * @return Uma Lista nova contendo 4 'Segmento' que formam o bounding box.
 */
LinkedList geo_gerar_biombo(Geo geo, double margem);

/**
 * Retorna o bounding box (retângulo envolvente) de todas as formas da cidade.
 * @param geo A cidade
 * @param min_x Ponteiro para retornar x minimo
 * @param min_y Ponteiro para retornar y minimo
 * @param max_x Ponteiro para retornar x maximo
 * @param max_y Ponteiro para retornar y maximo
 */
void geo_get_bounding_box(Geo geo, double *min_x, double *min_y, double *max_x, double *max_y);

/**
 * Libera toda a memória alocada pelo módulo Geo.
 */
void geo_destruir(Geo geo);

#endif