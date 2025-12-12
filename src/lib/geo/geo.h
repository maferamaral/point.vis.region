#ifndef GEO_H
#define GEO_H

#include <stdio.h>
#include "../geometria/ponto/ponto.h"
#include "../geometria/segmento/segmento.h"

typedef void* LinkedList;
typedef void *Geo;

Geo geo_criar();
void geo_ler(Geo geo, const char *path);
void geo_escrever_svg(Geo geo, FILE *svg);
LinkedList geo_get_formas(Geo geo);
LinkedList geo_obter_todas_barreiras(Geo geo);
LinkedList geo_gerar_biombo(Geo geo, Ponto centro_bomba);
void geo_get_bounding_box(Geo geo, double *min_x, double *min_y, double *max_x, double *max_y);
void geo_destruir(Geo geo);
void geo_remover_forma(Geo geo, int id);
void geo_alterar_cor(Geo geo, int id, const char *cor);
void geo_clonar_forma(Geo geo, int id, double dx, double dy);

#endif