#ifndef GEO_HANDLER_H
#define GEO_HANDLER_H

#include <stdio.h>
#include "../utils/lista/lista.h"
#include "../geometria/geometria.h"

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
 * O arquivo já deve estar aberto e com o cabeçalho SVG escrito.
 * @param geo A cidade
 * @param svg Ponteiro para o arquivo .svg aberto
 */
void geo_escrever_svg(Geo geo, FILE *svg);

/**
 * Retorna a lista genérica de formas (para manipulações gerais).
 */
LinkedList geo_get_formas(Geo geo);

/**
 * [IMPORTANTE PARA VISIBILIDADE]
 * Converte as barreiras da cidade (paredes de Retângulos e Linhas)
 * em uma lista de Segmentos matemáticos.
 * * @param geo A cidade
 * @return Uma LinkedList nova contendo elementos do tipo 'Segmento' (struct da lib geometria)
 * Nota: Quem chamar esta função deve destruir a lista retornada depois.
 */
LinkedList geo_obter_todas_barreiras(Geo geo);

/**
 * Libera toda a memória alocada pelo módulo Geo.
 */
void geo_destruir(Geo geo);

#endif