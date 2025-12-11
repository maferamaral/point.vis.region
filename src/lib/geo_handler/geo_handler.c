#include "geo_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Includes das suas formas e utilitários
#include "../formas/circulo/circulo.h"
#include "../formas/retangulo/retangulo.h"
#include "../formas/linha/linha.h"
#include "../formas/texto/texto.h"
#include "../formas/text_style/text_style.h"
#include "../formas/formas.h" // Enum TipoForma
#include "../utils/lista/lista.h"

// Estrutura interna para encapsular a forma e seu tipo
typedef struct
{
    TipoForma tipo;
    void *forma;
} ElementoGeo;

struct Geo_st
{
    LinkedList formas; // Lista de ElementoGeo*
};

Geo geo_criar()
{
    struct Geo_st *g = malloc(sizeof(struct Geo_st));
    g->formas = list_create();
    return g;
}

// Função auxiliar para criar e guardar o elemento
static void inserir_forma(Geo geo, TipoForma tipo, void *objeto)
{
    struct Geo_st *g = (struct Geo_st *)geo;
    ElementoGeo *el = malloc(sizeof(ElementoGeo));
    el->tipo = tipo;
    el->forma = objeto;
    list_insert_back(g->formas, el);
}

void geo_ler(Geo geo, const char *path)
{
    FILE *f = fopen(path, "r");
    if (!f)
    {
        printf("Erro ao abrir .geo: %s\n", path);
        return;
    }

    char buf[1024];
    while (fgets(buf, sizeof(buf), f))
    {
        char cmd[10];
        if (sscanf(buf, "%s", cmd) != 1)
            continue;

        if (strcmp(cmd, "c") == 0)
        {
            int id;
            double x, y, r;
            char cb[20], cp[20];
            sscanf(buf, "c %d %lf %lf %lf %s %s", &id, &x, &y, &r, cb, cp);
            void *c = circulo_criar(id, x, y, r, cb, cp);
            inserir_forma(geo, CIRCLE, c);
        }
        else if (strcmp(cmd, "r") == 0)
        {
            int id;
            double x, y, w, h;
            char cb[20], cp[20];
            sscanf(buf, "r %d %lf %lf %lf %lf %s %s", &id, &x, &y, &w, &h, cb, cp);
            void *r = retangulo_criar(id, x, y, w, h, cb, cp);
            inserir_forma(geo, RECTANGLE, r);
        }
        else if (strcmp(cmd, "l") == 0)
        {
            int id;
            double x1, y1, x2, y2;
            char color[20];
            sscanf(buf, "l %d %lf %lf %lf %lf %s", &id, &x1, &y1, &x2, &y2, color);
            void *l = line_create(id, x1, y1, x2, y2, color);
            inserir_forma(geo, LINE, l);
        }
        else if (strcmp(cmd, "t") == 0)
        {
            // Simplificado para leitura rápida
            int id;
            double x, y;
            char cb[20], cp[20], a;
            char txt[200];
            // Nota: ler texto com espaços exige parse mais cuidadoso, aqui é um exemplo simples
            sscanf(buf, "t %d %lf %lf %s %s %c %s", &id, &x, &y, cb, cp, &a, txt);
            void *t = text_create(id, x, y, cb, cp, a, txt);
            inserir_forma(geo, TEXT, t);
        }
    }
    fclose(f);
}

LinkedList geo_get_formas(Geo geo)
{
    struct Geo_st *g = (struct Geo_st *)geo;
    return g->formas;
}

// === AQUI A MÁGICA ACONTECE ===
LinkedList geo_obter_todas_barreiras(Geo geo)
{
    struct Geo_st *g = (struct Geo_st *)geo;
    LinkedList segmentos = list_create(); // Lista que vai para o algoritmo de visibilidade

    int n = list_size(g->formas);
    for (int i = 0; i < n; i++)
    {
        ElementoGeo *el = (ElementoGeo *)list_get_at(g->formas, i);

        if (el->tipo == LINE)
        {
            // Uma linha vira 1 segmento
            double x1 = line_get_x1(el->forma);
            double y1 = line_get_y1(el->forma);
            double x2 = line_get_x2(el->forma);
            double y2 = line_get_y2(el->forma);

            Segmento *s = malloc(sizeof(Segmento));
            *s = segmento_criar(ponto_criar(x1, y1), ponto_criar(x2, y2));
            list_insert_back(segmentos, s);
        }
        else if (el->tipo == RECTANGLE)
        {
            // Um retângulo vira 4 segmentos (paredes)
            double x = retangulo_get_x(el->forma);
            double y = retangulo_get_y(el->forma);
            double w = retangulo_get_largura(el->forma);
            double h = retangulo_get_altura(el->forma);

            Ponto p1 = ponto_criar(x, y);
            Ponto p2 = ponto_criar(x + w, y);
            Ponto p3 = ponto_criar(x + w, y + h);
            Ponto p4 = ponto_criar(x, y + h);

            Segmento *s1 = malloc(sizeof(Segmento));
            *s1 = segmento_criar(p1, p2);
            Segmento *s2 = malloc(sizeof(Segmento));
            *s2 = segmento_criar(p2, p3);
            Segmento *s3 = malloc(sizeof(Segmento));
            *s3 = segmento_criar(p3, p4);
            Segmento *s4 = malloc(sizeof(Segmento));
            *s4 = segmento_criar(p4, p1);

            list_insert_back(segmentos, s1);
            list_insert_back(segmentos, s2);
            list_insert_back(segmentos, s3);
            list_insert_back(segmentos, s4);
        }
        // Círculos e Textos geralmente não bloqueiam.
    }
    return segmentos;
}

void geo_destruir(Geo geo)
{
    struct Geo_st *g = (struct Geo_st *)geo;
    if (g == NULL)
        return;

    int n = list_size(g->formas);
    for (int i = 0; i < n; i++)
    {
        ElementoGeo *el = (ElementoGeo *)list_get_at(g->formas, i);
        if (el)
        {
            // Tentar liberar forma; se as formas tiverem destrutores próprios,
            // substitua por essas chamadas (ex: circulo_destruir)
            // Chamar destrutor específico por tipo quando disponível
            switch (el->tipo)
            {
            case CIRCLE:
                circulo_destruir((Circulo)el->forma);
                break;
            case RECTANGLE:
                // Retângulos têm duas APIs; chamamos a em português
                retangulo_destruir((Retangulo)el->forma);
                break;
            case LINE:
                line_destroy((Line)el->forma);
                break;
            case TEXT:
                text_destroy((Text)el->forma);
                break;
            case TEXT_STYLE:
                text_style_destroy((TextStyle)el->forma);
                break;
            default:
                // Fallback genérico
                free(el->forma);
                break;
            }
            free(el);
        }
    }
    list_destroy(g->formas);
    free(g);
}