#include "geo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h> // Para DBL_MAX
#include <math.h>

// Includes das formas
#include "../formas/circulo/circulo.h"
#include "../formas/retangulo/retangulo.h"
#include "../formas/linha/linha.h"
#include "../formas/texto/texto.h"
#include "../formas/formas.h" // Enum TipoForma
#include "../utils/lista/lista.h"
#include "../geometria/geometria.h"

// Estrutura interna para encapsular a forma e saber seu tipo
typedef struct {
    TipoForma tipo;
    void *forma;
} ElementoGeo;

struct Geo_st {
    LinkedList formas; // Lista de ElementoGeo*
};

Geo geo_criar() {
    struct Geo_st *g = malloc(sizeof(struct Geo_st));
    if (g) {
        g->formas = list_create();
    }
    return g;
}

// Função auxiliar para inserir na lista
static void inserir_forma(Geo geo, TipoForma tipo, void *objeto) {
    struct Geo_st *g = (struct Geo_st *)geo;
    ElementoGeo *el = malloc(sizeof(ElementoGeo));
    el->tipo = tipo;
    el->forma = objeto;
    list_insert_back(g->formas, el);
}

void geo_ler(Geo geo, const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        printf("Erro: Nao foi possivel abrir o arquivo .geo: %s\n", path);
        return;
    }

    char buf[1024];
    while (fgets(buf, sizeof(buf), f)) {
        char cmd[10];
        if (sscanf(buf, "%s", cmd) != 1) continue;

        if (strcmp(cmd, "c") == 0) { // Círculo
            int id; double x, y, r; char cb[50], cp[50];
            sscanf(buf, "c %d %lf %lf %lf %s %s", &id, &x, &y, &r, cb, cp);
            void *c = circulo_criar(id, x, y, r, cb, cp);
            inserir_forma(geo, CIRCLE, c);
        }
        else if (strcmp(cmd, "r") == 0) { // Retângulo
            int id; double x, y, w, h; char cb[50], cp[50];
            sscanf(buf, "r %d %lf %lf %lf %lf %s %s", &id, &x, &y, &w, &h, cb, cp);
            void *r = retangulo_criar(id, x, y, w, h, cb, cp);
            inserir_forma(geo, RECTANGLE, r);
        }
        else if (strcmp(cmd, "l") == 0) { // Linha
            int id; double x1, y1, x2, y2; char color[50];
            sscanf(buf, "l %d %lf %lf %lf %lf %s", &id, &x1, &y1, &x2, &y2, color);
            void *l = line_create(id, x1, y1, x2, y2, color);
            inserir_forma(geo, LINE, l);
        }
        else if (strcmp(cmd, "t") == 0) { // Texto
            int id; double x, y; char cb[50], cp[50], a;
            char txt[256];
            // Leitura segura do texto
            int offset = 0;
            char temp_cmd[10];
            sscanf(buf, "%s %d %lf %lf %s %s %c%n", temp_cmd, &id, &x, &y, cb, cp, &a, &offset);
            
            if (offset > 0 && offset < (int)strlen(buf)) {
                char *ptr = buf + offset;
                while (*ptr == ' ' || *ptr == '\t') ptr++; 
                strcpy(txt, ptr);
                size_t len = strlen(txt);
                if (len > 0 && txt[len-1] == '\n') txt[len-1] = '\0';
            } else {
                strcpy(txt, "");
            }

            void *t = text_create(id, x, y, cb, cp, a, txt);
            inserir_forma(geo, TEXT, t);
        }
    }
    fclose(f);
}

void geo_escrever_svg(Geo geo, FILE *svg) {
    if (!geo || !svg) return;
    struct Geo_st *g = (struct Geo_st *)geo;

    int n = list_size(g->formas);
    for (int i = 0; i < n; i++) {
        ElementoGeo *el = (ElementoGeo *)list_get_at(g->formas, i);
        
        if (el->tipo == CIRCLE) {
            void *c = el->forma;
            fprintf(svg, "<circle cx=\"%.2f\" cy=\"%.2f\" r=\"%.2f\" stroke=\"%s\" fill=\"%s\" stroke-width=\"1\" />\n",
                circulo_get_x(c), circulo_get_y(c), circulo_get_raio(c), 
                circulo_get_cor_borda(c), circulo_get_cor_preenchimento(c));
        }
        else if (el->tipo == RECTANGLE) {
            void *r = el->forma;
            fprintf(svg, "<rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" stroke=\"%s\" fill=\"%s\" stroke-width=\"1\" />\n",
                retangulo_get_x(r), retangulo_get_y(r), 
                retangulo_get_largura(r), retangulo_get_altura(r),
                retangulo_get_cor_borda(r), retangulo_get_cor_preenchimento(r));
        }
        else if (el->tipo == LINE) {
            void *l = el->forma;
            fprintf(svg, "<line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" y2=\"%.2f\" stroke=\"%s\" stroke-width=\"1\" />\n",
                line_get_x1(l), line_get_y1(l), line_get_x2(l), line_get_y2(l), line_get_color(l));
        }
        else if (el->tipo == TEXT) {
            void *t = el->forma;
            char anchor = text_get_anchor(t);
            const char *svg_anchor = "start";
            if (anchor == 'm') svg_anchor = "middle";
            if (anchor == 'e' || anchor == 'f') svg_anchor = "end"; // 'f' ou 'e' dependendo do padrão

            fprintf(svg, "<text x=\"%.2f\" y=\"%.2f\" stroke=\"%s\" fill=\"%s\" text-anchor=\"%s\">%s</text>\n",
                text_get_x(t), text_get_y(t), text_get_border_color(t), 
                text_get_fill_color(t), svg_anchor, text_get_text(t));
        }
    }
}

LinkedList geo_get_formas(Geo geo) {
    return ((struct Geo_st *)geo)->formas;
}

LinkedList geo_obter_todas_barreiras(Geo geo) {
    struct Geo_st *g = (struct Geo_st *)geo;
    LinkedList segmentos = list_create(); 

    int n = list_size(g->formas);
    for (int i = 0; i < n; i++) {
        ElementoGeo *el = (ElementoGeo *)list_get_at(g->formas, i);
        
        // Apenas Linhas e Retângulos bloqueiam a visão
        if (el->tipo == LINE) {
            double x1 = line_get_x1(el->forma);
            double y1 = line_get_y1(el->forma);
            double x2 = line_get_x2(el->forma);
            double y2 = line_get_y2(el->forma);
            
            Segmento *s = malloc(sizeof(Segmento));
            *s = segmento_criar(ponto_criar(x1, y1), ponto_criar(x2, y2));
            list_insert_back(segmentos, s);
        }
        else if (el->tipo == RECTANGLE) {
            double x = retangulo_get_x(el->forma);
            double y = retangulo_get_y(el->forma);
            double w = retangulo_get_largura(el->forma);
            double h = retangulo_get_altura(el->forma);

            Ponto p1 = ponto_criar(x, y);
            Ponto p2 = ponto_criar(x + w, y);
            Ponto p3 = ponto_criar(x + w, y + h);
            Ponto p4 = ponto_criar(x, y + h);

            Segmento *s1 = malloc(sizeof(Segmento)); *s1 = segmento_criar(p1, p2);
            Segmento *s2 = malloc(sizeof(Segmento)); *s2 = segmento_criar(p2, p3);
            Segmento *s3 = malloc(sizeof(Segmento)); *s3 = segmento_criar(p3, p4);
            Segmento *s4 = malloc(sizeof(Segmento)); *s4 = segmento_criar(p4, p1);

            list_insert_back(segmentos, s1);
            list_insert_back(segmentos, s2);
            list_insert_back(segmentos, s3);
            list_insert_back(segmentos, s4);
        }
    }
    return segmentos;
}

void geo_get_bounding_box(Geo geo, double *min_x, double *min_y, double *max_x, double *max_y) {
    struct Geo_st *g = (struct Geo_st *)geo;
    double mx = DBL_MAX, my = DBL_MAX, Mx = -DBL_MAX, My = -DBL_MAX;

    int n = list_size(g->formas);
    if (n == 0) {
        mx = 0; my = 0; Mx = 1000; My = 1000;
    }

    for (int i = 0; i < n; i++) {
        ElementoGeo *el = (ElementoGeo *)list_get_at(g->formas, i);
        double x1 = 0, y1 = 0, x2 = 0, y2 = 0;

        if (el->tipo == CIRCLE) {
            double x = circulo_get_x(el->forma);
            double y = circulo_get_y(el->forma);
            double r = circulo_get_raio(el->forma);
            x1 = x - r; y1 = y - r;
            x2 = x + r; y2 = y + r;
        }
        else if (el->tipo == RECTANGLE) {
            x1 = retangulo_get_x(el->forma);
            y1 = retangulo_get_y(el->forma);
            x2 = x1 + retangulo_get_largura(el->forma);
            y2 = y1 + retangulo_get_altura(el->forma);
        }
        else if (el->tipo == LINE) {
            double lx1 = line_get_x1(el->forma);
            double ly1 = line_get_y1(el->forma);
            double lx2 = line_get_x2(el->forma);
            double ly2 = line_get_y2(el->forma);
            x1 = (lx1 < lx2) ? lx1 : lx2;
            x2 = (lx1 > lx2) ? lx1 : lx2;
            y1 = (ly1 < ly2) ? ly1 : ly2;
            y2 = (ly1 > ly2) ? ly1 : ly2;
        }
        else if (el->tipo == TEXT) {
            x1 = x2 = text_get_x(el->forma);
            y1 = y2 = text_get_y(el->forma);
        }
        else {
            continue;
        }

        if (x1 < mx) mx = x1;
        if (y1 < my) my = y1;
        if (x2 > Mx) Mx = x2;
        if (y2 > My) My = y2;
    }
    
    if (min_x) *min_x = mx;
    if (min_y) *min_y = my;
    if (max_x) *max_x = Mx;
    if (max_y) *max_y = My;
}

LinkedList geo_gerar_biombo(Geo geo, double margem) {
    struct Geo_st *g = (struct Geo_st *)geo;
    LinkedList biombo = list_create();

    double min_x, min_y, max_x, max_y;
    geo_get_bounding_box(geo, &min_x, &min_y, &max_x, &max_y);

    // Aplica margem
    min_x -= margem;
    min_y -= margem;
    max_x += margem;
    max_y += margem;

    Ponto p1 = ponto_criar(min_x, min_y);
    Ponto p2 = ponto_criar(max_x, min_y);
    Ponto p3 = ponto_criar(max_x, max_y);
    Ponto p4 = ponto_criar(min_x, max_y);

    Segmento *s1 = malloc(sizeof(Segmento)); *s1 = segmento_criar(p1, p2);
    Segmento *s2 = malloc(sizeof(Segmento)); *s2 = segmento_criar(p2, p3);
    Segmento *s3 = malloc(sizeof(Segmento)); *s3 = segmento_criar(p3, p4);
    Segmento *s4 = malloc(sizeof(Segmento)); *s4 = segmento_criar(p4, p1);

    list_insert_back(biombo, s1);
    list_insert_back(biombo, s2);
    list_insert_back(biombo, s3);
    list_insert_back(biombo, s4);

    return biombo;
}

void geo_destruir(Geo geo) {
    if (!geo) return;
    struct Geo_st *g = (struct Geo_st *)geo;
    
    int n = list_size(g->formas);
    for (int i = 0; i < n; i++) {
        ElementoGeo *el = (ElementoGeo *)list_get_at(g->formas, i);
        
        if (el->tipo == CIRCLE) circulo_destruir(el->forma);
        else if (el->tipo == RECTANGLE) retangulo_destruir(el->forma);
        else if (el->tipo == LINE) line_destroy(el->forma);
        else if (el->tipo == TEXT) text_destroy(el->forma);
        // Se houver TEXT_STYLE na lista, adicionar destruição aqui

        free(el);
    }
    
    list_destroy(g->formas);
    free(g);
}