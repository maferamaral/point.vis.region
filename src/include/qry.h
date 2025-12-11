#ifndef QRY_HANDLER_H
#define QRY_HANDLER_H

#include "../lib/geo/geo.h"

/**
 * Processa um arquivo de consultas (.qry) e executa operações sobre as formas geométricas.
 * Gera arquivos de saída (texto e SVG) com os resultados.
 *
 * @param cidade A estrutura Geo com todas as formas cadastradas
 * @param qryPath Caminho do arquivo de consultas (.qry)
 * @param outPath Caminho base para arquivos de saída (sem extensão)
 * @param geoName Nome do arquivo .geo (para referência em logs)
 */
void qry_processar(Geo cidade, const char *qryPath, const char *outPath, const char *geoName);

#endif // QRY_HANDLER_H
