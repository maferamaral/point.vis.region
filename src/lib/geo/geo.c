#include "geo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Includes das formas (ajuste os caminhos se necessário)
#include "../formas/circulo/circulo.h"
#include "../formas/retangulo/retangulo.h"
#include "../formas/linha/linha.h"
#include "../formas/texto/texto.h"
#include "../formas/formas.h" // Enum TipoForma
#include "../utils/lista/lista.h"
#include "../geometria/geometria.h"

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

// Função auxiliar para inserir na lista
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
        printf("Erro: Nao foi possivel abrir o arquivo .geo: %s\n", path);
        return;
    }

    char buf[1024];
    while (fgets(buf, sizeof(buf), f))
    {
        char cmd[10];
        // Lê a primeira palavra da linha para identificar o comando
        if (sscanf(buf, "%s", cmd) != 1)
            continue;

        if (strcmp(cmd, "c") == 0)
        { // Círculo
            int id;
            double x, y, r;
            char cb[50], cp[50];
            sscanf(buf, "c %d %lf %lf %lf %s %s", &id, &x, &y, &r, cb, cp);
            void *c = circulo_criar(id, x, y, r, cb, cp);
            inserir_forma(geo, CIRCLE, c);
        }
        else if (strcmp(cmd, "r") == 0)
        { // Retângulo
            int id;
            double x, y, w, h;
            char cb[50], cp[50];
            sscanf(buf, "r %d %lf %lf %lf %lf %s %s", &id, &x, &y, &w, &h, cb, cp);
            void *r = retangulo_criar(id, x, y, w, h, cb, cp);
            inserir_forma(geo, RECTANGLE, r);
        }
        else if (strcmp(cmd, "l") == 0)
        { // Linha
            int id;
            double x1, y1, x2, y2;
            char color[50];
            sscanf(buf, "l %d %lf %lf %lf %lf %s", &id, &x1, &y1, &x2, &y2, color);
            void *l = line_create(id, x1, y1, x2, y2, color);
            inserir_forma(geo, LINE, l);
        }
        else if (strcmp(cmd, "t") == 0)
        { // Texto
            int id;
            double x, y;
            char cb[50], cp[50], a;
            char txt[256];
            // Leitura segura do texto que pode conter espaços (assumindo que é o último campo)
            // Formato: t id x y cb cp a texto...

            // Lemos os metadados primeiro
            int offset = 0;
            char temp_cmd[10];
            sscanf(buf, "%s %d %lf %lf %s %s %c%n", temp_cmd, &id, &x, &y, cb, cp, &a, &offset);

            // O resto da linha é o texto (pulando o espaço após o 'anchor')
            if (offset > 0 && offset < (int)strlen(buf))
            {
                char *ptr = buf + offset;
                while (*ptr == ' ' || *ptr == '\t')
                    ptr++; // Pula espaços
                strcpy(txt, ptr);
                // Remove newline no final se existir
                size_t len = strlen(txt);
                if (len > 0 && txt[len - 1] == '\n')
                    txt[len - 1] = '\0';
            }
            else
            {
                strcpy(txt, "");
            }

            void *t = text_create(id, x, y, cb, cp, a, txt);
            inserir_forma(geo, TEXT, t);
        }
    }
    fclose(f);
}

void geo_escrever_svg(Geo geo, FILE *svg)
{
    if (!geo || !svg)
        return;
    struct Geo_st *g = (struct Geo_st *)geo;

    int n = list_size(g->formas);
    for (int i = 0; i < n; i++)
    {
        ElementoGeo *el = (ElementoGeo *)list_get_at(g->formas, i);

        if (el->tipo == CIRCLE)
        {
            void *c = el->forma;
            fprintf(svg, "<circle cx=\"%.2f\" cy=\"%.2f\" r=\"%.2f\" stroke=\"%s\" fill=\"%s\" stroke-width=\"1\" opacity=\"0.6\" />\n",
                    circulo_get_x(c), circulo_get_y(c), circulo_get_raio(c),
                    circulo_get_cor_borda(c), circulo_get_cor_preenchimento(c));
        }
        else if (el->tipo == RECTANGLE)
        {
            void *r = el->forma;
            fprintf(svg, "<rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" stroke=\"%s\" fill=\"%s\" stroke-width=\"1\" opacity=\"0.6\" />\n",
                    retangulo_get_x(r), retangulo_get_y(r),
                    retangulo_get_largura(r), retangulo_get_altura(r),
                    retangulo_get_cor_borda(r), retangulo_get_cor_preenchimento(r));
        }
        else if (el->tipo == LINE)
        {
            void *l = el->forma;
            fprintf(svg, "<line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" y2=\"%.2f\" stroke=\"%s\" stroke-width=\"1\" opacity=\"0.6\" />\n",
                    line_get_x1(l), line_get_y1(l), line_get_x2(l), line_get_y2(l), line_get_color(l));
        }
        else if (el->tipo == TEXT)
        {
            void *t = el->forma;
            char anchor = text_get_anchor(t);
            const char *svg_anchor = "start";
            if (anchor == 'm')
                svg_anchor = "middle";
            if (anchor == 'e')
                svg_anchor = "end";

            fprintf(svg, "<text x=\"%.2f\" y=\"%.2f\" stroke=\"%s\" fill=\"%s\" text-anchor=\"%s\" opacity=\"0.6\">%s</text>\n",
                    text_get_x(t), text_get_y(t), text_get_border_color(t),
                    text_get_fill_color(t), svg_anchor, text_get_text(t));
        }
    }
}

LinkedList geo_get_formas(Geo geo)
{
    return ((struct Geo_st *)geo)->formas;
}

LinkedList geo_obter_todas_barreiras(Geo geo)
{
    struct Geo_st *g = (struct Geo_st *)geo;
    LinkedList segmentos = list_create();

    int n = list_size(g->formas);
    for (int i = 0; i < n; i++)
    {
        ElementoGeo *el = (ElementoGeo *)list_get_at(g->formas, i);

        // Regra de negócio: O que bloqueia a visão?
        // Assumindo Linhas e Retângulos como barreiras opacas.

        if (el->tipo == LINE)
        {
            // Linha vira 1 segmento
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
            // Retângulo vira 4 segmentos (as paredes)
            double x = retangulo_get_x(el->forma);
            double y = retangulo_get_y(el->forma);
            double w = retangulo_get_largura(el->forma);
            double h = retangulo_get_altura(el->forma);

            Ponto p1 = ponto_criar(x, y);
            Ponto p2 = ponto_criar(x + w, y);
            Ponto p3 = ponto_criar(x + w, y + h);
            Ponto p4 = ponto_criar(x, y + h);

            // Cria e insere os 4 lados
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

        // Círculos e Textos geralmente são transparentes ou alvos,
        // a não ser que o enunciado peça especificamente para bloquearem.
    }
    return segmentos;
}

void geo_destruir(Geo geo)
{
    struct Geo_st *g = (struct Geo_st *)geo;

    // Iterar e liberar ElementoGeo* e as formas internas
    int n = list_size(g->formas);
    for (int i = 0; i < n; i++)
    {
        ElementoGeo *el = (ElementoGeo *)list_get_at(g->formas, i);

        // Aqui você deve chamar os destrutores específicos
        if (el->tipo == CIRCLE)
            circulo_destruir(el->forma);
        else if (el->tipo == RECTANGLE)
            retangulo_destruir(el->forma);
        else if (el->tipo == LINE)
            line_destroy(el->forma);
        else if (el->tipo == TEXT)
            text_destroy(el->forma);
        // TEXT_STYLE geralmente não é armazenado na lista principal, mas se for, liberar aqui.

        free(el);
    }

    list_destroy(g->formas); // Destrói a estrutura da lista
    free(g);
}

// Implementação de helper genérico para obter ID de uma forma
int get_id_generico(void *forma, TipoForma tipo)
{
    if (!forma)
        return -1;

    switch (tipo)
    {
    case CIRCLE:
        return circulo_get_id(forma);
    case RECTANGLE:
        return retangulo_get_id(forma);
    case LINE:
        return line_get_id(forma);
    case TEXT:
        return text_get_id(forma);
    default:
        return -1;
    }
}