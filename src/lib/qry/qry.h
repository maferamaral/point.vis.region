#ifndef QRY_H
#define QRY_H

#include "../geo/geo.h"

/**
 * Processa um arquivo .qry aplicando comandos sobre a cidade.
 * Gera arquivos .svg e .txt com os resultados.
 * 
 * @param cidade Estrutura Geo contendo as formas da cidade
 * @param qryPath Caminho para o arquivo .qry
 * @param outPath Caminho base para os arquivos de saída (sem extensão)
 * @param geoName Nome do arquivo .geo (para referência nos outputs)
 */
void qry_processar(Geo cidade, const char *qryPath, const char *outPath, const char *geoName);

#endif
