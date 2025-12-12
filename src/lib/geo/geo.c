#include "geo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include "../formas/circulo/circulo.h"
#include "../formas/retangulo/retangulo.h"
#include "../formas/linha/linha.h"
#include "../formas/texto/texto.h"
#include "../formas/formas.h"
#include "../utils/lista/lista.h"
#include "../geometria/ponto/ponto.h"
#include "../geometria/segmento/segmento.h"

typedef struct {
    TipoForma tipo;
    void *forma;
} ElementoGeo;

struct Geo_st {
    LinkedList formas;
};

Geo geo_criar() {
    struct Geo_st *g = malloc(sizeof(struct Geo_st));
    if (g) {
        g->formas = list_create();
    }
    return g;
}

static void inserir_forma(Geo geo, TipoForma tipo, void *objeto) {
    struct Geo_st *g = (struct Geo_st *)geo;
    ElementoGeo *el = malloc(sizeof(ElementoGeo));
    el->tipo = tipo;
    el->forma = objeto;
    list_insert_back(g->formas, el);
}

void geo_ler(Geo geo, const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;

    char buf[1024];
    while (fgets(buf, sizeof(buf), f)) {
        char cmd[10];
        if (sscanf(buf, "%s", cmd) != 1) continue;

        if (strcmp(cmd, "c") == 0) {
            int id; double x, y, r; char cb[50], cp[50];
            sscanf(buf, "c %d %lf %lf %lf %s %s", &id, &x, &y, &r, cb, cp);
            void *c = circulo_criar(id, x, y, r, cb, cp);
            inserir_forma(geo, CIRCLE, c);
        }
        else if (strcmp(cmd, "r") == 0) {
            int id; double x, y, w, h; char cb[50], cp[50];
            sscanf(buf, "r %d %lf %lf %lf %lf %s %s", &id, &x, &y, &w, &h, cb, cp);
            void *r = retangulo_criar(id, x, y, w, h, cb, cp);
            inserir_forma(geo, RECTANGLE, r);
        }
        else if (strcmp(cmd, "l") == 0) {
            int id; double x1, y1, x2, y2; char color[50];
            sscanf(buf, "l %d %lf %lf %lf %lf %s", &id, &x1, &y1, &x2, &y2, color);
            void *l = line_create(id, x1, y1, x2, y2, color);
            inserir_forma(geo, LINE, l);
        }
        else if (strcmp(cmd, "t") == 0) {
            int id; double x, y; char cb[50], cp[50], a;
            char txt[256];
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
            fprintf(svg, "<circle cx=\"%.2f\" cy=\"%.2f\" r=\"%.2f\" stroke=\"%s\" fill=\"%s\" stroke-width=\"1\" fill-opacity=\"0.6\" stroke-opacity=\"0.6\" />\n",
                circulo_get_x(c), circulo_get_y(c), circulo_get_raio(c), 
                circulo_get_cor_borda(c), circulo_get_cor_preenchimento(c));
        }
        else if (el->tipo == RECTANGLE) {
            void *r = el->forma;
            fprintf(svg, "<rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" stroke=\"%s\" fill=\"%s\" stroke-width=\"1\" fill-opacity=\"0.6\" stroke-opacity=\"0.6\" />\n",
                retangulo_get_x(r), retangulo_get_y(r), 
                retangulo_get_largura(r), retangulo_get_altura(r),
                retangulo_get_cor_borda(r), retangulo_get_cor_preenchimento(r));
        }
        else if (el->tipo == LINE) {
            void *l = el->forma;
            fprintf(svg, "<line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" y2=\"%.2f\" stroke=\"%s\" stroke-width=\"1\" stroke-opacity=\"0.6\" />\n",
                line_get_x1(l), line_get_y1(l), line_get_x2(l), line_get_y2(l), line_get_color(l));
        }
        else if (el->tipo == TEXT) {
            void *t = el->forma;
            char anchor = text_get_anchor(t);
            const char *svg_anchor = "start";
            if (anchor == 'm') svg_anchor = "middle";
            if (anchor == 'e' || anchor == 'f') svg_anchor = "end";

            fprintf(svg, "<text x=\"%.2f\" y=\"%.2f\" stroke=\"%s\" fill=\"%s\" text-anchor=\"%s\" fill-opacity=\"0.6\" stroke-opacity=\"0.6\">%s</text>\n",
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
        
        // Only include lines that are anteparos (those created by command 'a')
        // Anteparos have IDs >= 5000 by convention (set in qry.c)
        if (el->tipo == LINE) {
            int line_id = line_get_id(el->forma);
            if (line_id >= 5000) {  // Only anteparos
                double x1 = line_get_x1(el->forma);
                double y1 = line_get_y1(el->forma);
                double x2 = line_get_x2(el->forma);
                double y2 = line_get_y2(el->forma);
                
                // Correct usage with new API:
                Segmento seg = criar_segmento(line_id, line_id, x1, y1, x2, y2, "black");
                list_insert_back(segmentos, seg);
            }
        }
        // Rectangles are NOT barriers - they become barriers only through command 'a'
        // which converts them to 4 LINE segments with ID >= 5000
    }
    return segmentos;
}

LinkedList geo_gerar_biombo(Geo geo, Ponto centro_bomba) {
    struct Geo_st *g = (struct Geo_st *)geo;
    LinkedList biombo = list_create();

    double min_x = DBL_MAX, min_y = DBL_MAX;
    double max_x = -DBL_MAX, max_y = -DBL_MAX;

    double cx = get_ponto_x(centro_bomba);
    double cy = get_ponto_y(centro_bomba);

    if (list_is_empty(g->formas)) {
        min_x = cx; max_x = cx;
        min_y = cy; max_y = cy;
    } else {
        geo_get_bounding_box(geo, &min_x, &min_y, &max_x, &max_y);
    }

    // Expande para incluir a bomba se ela estiver fora
    if (cx < min_x) min_x = cx;
    if (cx > max_x) max_x = cx;
    if (cy < min_y) min_y = cy;
    if (cy > max_y) max_y = cy;

    double largura = max_x - min_x;
    double altura = max_y - min_y;
    // Margem de segurança de 10%
    double dx = (largura > 0) ? largura * 0.10 : 50.0;
    double dy = (altura > 0) ? altura * 0.10 : 50.0;

    min_x -= dx; min_y -= dy;
    max_x += dx; max_y += dy;

    Segmento s1 = criar_segmento(-1, -1, min_x, min_y, max_x, min_y, "none");
    Segmento s2 = criar_segmento(-1, -1, max_x, min_y, max_x, max_y, "none");
    Segmento s3 = criar_segmento(-1, -1, max_x, max_y, min_x, max_y, "none");
    Segmento s4 = criar_segmento(-1, -1, min_x, max_y, min_x, min_y, "none");

    list_insert_back(biombo, s1);
    list_insert_back(biombo, s2);
    list_insert_back(biombo, s3);
    list_insert_back(biombo, s4);

    return biombo;
}

void geo_get_bounding_box(Geo geo, double *min_x, double *min_y, double *max_x, double *max_y) {
    struct Geo_st *g = (struct Geo_st *)geo;
    double mx = DBL_MAX, my = DBL_MAX, Mx = -DBL_MAX, My = -DBL_MAX;

    int n = list_size(g->formas);
    if (n == 0) {
        if (min_x) *min_x = 0; if (min_y) *min_y = 0;
        if (max_x) *max_x = 1000; if (max_y) *max_y = 1000;
        return;
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

void geo_remover_forma(Geo geo, int id) {
    struct Geo_st *g = (struct Geo_st *)geo;
    int n = list_size(g->formas);
    for (int i = 0; i < n; i++) {
        ElementoGeo *el = (ElementoGeo *)list_get_at(g->formas, i);
        int el_id = -1;
        
        if (el->tipo == CIRCLE) el_id = circulo_get_id(el->forma);
        else if (el->tipo == RECTANGLE) el_id = retangulo_get_id(el->forma);
        else if (el->tipo == LINE) el_id = line_get_id(el->forma);
        else if (el->tipo == TEXT) el_id = text_get_id(el->forma);
        
        if (el_id == id) {
            list_remove_at(g->formas, i);
            
            if (el->tipo == CIRCLE) circulo_destruir(el->forma);
            else if (el->tipo == RECTANGLE) retangulo_destruir(el->forma);
            else if (el->tipo == LINE) line_destroy(el->forma);
            else if (el->tipo == TEXT) text_destroy(el->forma);
            
            free(el);
            return;
        }
    }
}

void geo_alterar_cor(Geo geo, int id, const char *cor) {
    struct Geo_st *g = (struct Geo_st *)geo;
    int n = list_size(g->formas);
    for (int i = 0; i < n; i++) {
        ElementoGeo *el = (ElementoGeo *)list_get_at(g->formas, i);
        int el_id = -1;
        if (el->tipo == CIRCLE) el_id = circulo_get_id(el->forma);
        else if (el->tipo == RECTANGLE) el_id = retangulo_get_id(el->forma);
        else if (el->tipo == LINE) el_id = line_get_id(el->forma);
        else if (el->tipo == TEXT) el_id = text_get_id(el->forma);

        if (el_id == id) {
            if (el->tipo == CIRCLE) {
                circulo_set_cor_borda(el->forma, cor);
                circulo_set_cor_preenchimento(el->forma, cor);
            } else if (el->tipo == RECTANGLE) {
                retangulo_set_cor_borda(el->forma, cor);
                retangulo_set_cor_preenchimento(el->forma, cor);
            } else if (el->tipo == LINE) {
                line_set_color(el->forma, cor);
            } else if (el->tipo == TEXT) {
                text_set_border_color(el->forma, cor);
                text_set_fill_color(el->forma, cor);
            }
            return;
        }
    }
}

void geo_clonar_forma(Geo geo, int id, double dx, double dy) {
    struct Geo_st *g = (struct Geo_st *)geo;
    int n = list_size(g->formas);
    for (int i = 0; i < n; i++) {
        ElementoGeo *el = (ElementoGeo *)list_get_at(g->formas, i);
        int el_id = -1;
        if (el->tipo == CIRCLE) el_id = circulo_get_id(el->forma);
        else if (el->tipo == RECTANGLE) el_id = retangulo_get_id(el->forma);
        else if (el->tipo == LINE) el_id = line_get_id(el->forma);
        else if (el->tipo == TEXT) el_id = text_get_id(el->forma);
        
        if (el_id == id) {
            // O qry.c define o ID novo como id + 10000.
            int new_id = id + 10000;

            if (el->tipo == CIRCLE) {
                void* c = el->forma;
                void* novo = circulo_criar(new_id, circulo_get_x(c)+dx, circulo_get_y(c)+dy, 
                                           circulo_get_raio(c), circulo_get_cor_borda(c), circulo_get_cor_preenchimento(c));
                inserir_forma(geo, CIRCLE, novo);
            } 
            else if (el->tipo == RECTANGLE) {
                void* r = el->forma;
                void* novo = retangulo_criar(new_id, retangulo_get_x(r)+dx, retangulo_get_y(r)+dy, 
                                             retangulo_get_largura(r), retangulo_get_altura(r),
                                             retangulo_get_cor_borda(r), retangulo_get_cor_preenchimento(r));
                inserir_forma(geo, RECTANGLE, novo);
            }
            else if (el->tipo == LINE) {
                void* l = el->forma;
                void* novo = line_create(new_id, line_get_x1(l)+dx, line_get_y1(l)+dy, 
                                         line_get_x2(l)+dx, line_get_y2(l)+dy, 
                                         line_get_color(l));
                inserir_forma(geo, LINE, novo);
            }
            else if (el->tipo == TEXT) {
                void* t = el->forma;
                void* novo = text_create(new_id, text_get_x(t)+dx, text_get_y(t)+dy, 
                                         text_get_border_color(t), text_get_fill_color(t),
                                         text_get_anchor(t), text_get_text(t));
                inserir_forma(geo, TEXT, novo);
            }
            return;
        }
    }
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

        free(el);
    }
    
    list_destroy(g->formas);
    free(g);
}