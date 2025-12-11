#ifndef GEO_HANDLER_H
#define GEO_HANDLER_H

#include <stdio.h>
#include "../utils/lista/lista.h"   // Sua lista genérica
#include "../geometria/geometria.h" // O TAD de matemática criado anteriormente

typedef void *Geo;

/**
 * Inicializa a "Cidade" / Banco de Dados Geográfico
 */
Geo geo_criar();

/**
 * Lê o arquivo .geo e popula a cidade com as formas.
 * @param geo A estrutura da cidade
 * @param path Caminho completo do arquivo .geo
 */
void geo_ler(Geo geo, const char *path);

/**
 * Retorna a lista de todas as formas armazenadas (para desenhar o SVG final, por exemplo).
 */
LinkedList geo_get_formas(Geo geo);

/**
 * [NOVO] Converte as barreiras da cidade (Retângulos, Linhas) em uma lista de Segmentos.
 * Essencial para o algoritmo de visibilidade.
 * * @param geo A cidade
 * @return Uma Lista contendo elementos do tipo 'Segmento' (struct da lib geometria)
 */
LinkedList geo_obter_todas_barreiras(Geo geo);

/**
 * Limpa a memória
 */
void geo_destruir(Geo geo);

#endif