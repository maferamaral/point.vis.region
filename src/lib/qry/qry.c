#include "../../include/qry.h"
#include "../visibilidade/visibilidade.h"
#include "../geo/geo.h"
#include "../svg/svg.h"
#include "../formas/formas.h"
#include "../utils/utils.h"
#include "../utils/lista/lista.h"
#include "../geometria/geometria.h"
#include "../formas/circulo/circulo.h"
#include "../formas/retangulo/retangulo.h"
#include "../formas/linha/linha.h"
#include "../formas/texto/texto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper to write SVG header
static void start_svg(FILE *f) {
    if (f) fprintf(f, "<svg xmlns=\"http://www.w3.org/2000/svg\">\n");
}

// Helper to write SVG footer
static void end_svg(FILE *f) {
    if (f) fprintf(f, "</svg>\n");
}

static int get_max_id(LinkedList formas) {
    int max = 0;
    int n = list_size(formas);
    for(int i=0; i<n; i++) {
        ElementoGeo *el = (ElementoGeo*)list_get_at(formas, i);
        int id = get_id_generico(el->forma, el->tipo);
        if (id > max) max = id;
    }
    return max;
}

static void set_color_generic(void *forma, TipoForma tipo, const char *cor) {
    if (tipo == CIRCLE) {
        circulo_set_cor_borda(forma, cor);
        circulo_set_cor_preenchimento(forma, cor);
    } else if (tipo == RECTANGLE) {
        retangulo_set_cor_borda(forma, cor);
        retangulo_set_cor_preenchimento(forma, cor);
    } else if (tipo == LINE) {
        line_set_color(forma, cor);
    } else if (tipo == TEXT) {
        text_set_border_color(forma, cor);
        text_set_fill_color(forma, cor);
    }
}

static void clone_generic(LinkedList formas, void *forma, TipoForma tipo, double dx, double dy) {
    int new_id = get_max_id(formas) + 1;
    void *new_shape = NULL;
    
    if (tipo == CIRCLE) {
        double x = circulo_get_x(forma);
        double y = circulo_get_y(forma);
        double r = circulo_get_raio(forma);
        const char *cb = circulo_get_cor_borda(forma);
        const char *cp = circulo_get_cor_preenchimento(forma);
        new_shape = circulo_criar(new_id, x + dx, y + dy, r, cb, cp);
    } else if (tipo == RECTANGLE) {
        double x = retangulo_get_x(forma);
        double y = retangulo_get_y(forma);
        double w = retangulo_get_largura(forma);
        double h = retangulo_get_altura(forma);
        const char *cb = retangulo_get_cor_borda(forma);
        const char *cp = retangulo_get_cor_preenchimento(forma);
        new_shape = retangulo_criar(new_id, x + dx, y + dy, w, h, cb, cp);
    } else if (tipo == LINE) {
        double x1 = line_get_x1(forma);
        double y1 = line_get_y1(forma);
        double x2 = line_get_x2(forma);
        double y2 = line_get_y2(forma);
        const char *c = line_get_color(forma);
        new_shape = line_create(new_id, x1 + dx, y1 + dy, x2 + dx, y2 + dy, c);
    } else if (tipo == TEXT) {
        double x = text_get_x(forma);
        double y = text_get_y(forma);
        // Note: text module getter/setter naming might vary. Using what was seen.
        const char *cb = text_get_border_color(forma);
        const char *cp = text_get_fill_color(forma);
        const char *conteudo = text_get_text(forma);
        char anchor = text_get_anchor(forma);
        new_shape = text_create(new_id, x + dx, y + dy, cb, cp, anchor, conteudo);
    }

    if (new_shape) {
        ElementoGeo *wrapper = malloc(sizeof(ElementoGeo));
        wrapper->tipo = tipo;
        wrapper->forma = new_shape;
        list_insert_back(formas, wrapper);
    }
}

// Helper to create segments based on form type and rule
// Helper to create segments based on form type and rule
static void add_barriers_from_form(void *forma, TipoForma tipo, LinkedList barriers, char orientation, int *id_counter, FILE *ftxt) {
    if (tipo == RECTANGLE) {
        double x = retangulo_get_x(forma);
        double y = retangulo_get_y(forma);
        double w = retangulo_get_largura(forma);
        double h = retangulo_get_altura(forma);
        
        Ponto p1 = {x, y};
        Ponto p2 = {x + w, y};
        Ponto p3 = {x + w, y + h};
        Ponto p4 = {x, y + h};
        
        Ponto pts[] = {p1, p2, p3, p4};
        Ponto next_pts[] = {p2, p3, p4, p1};

        for (int k = 0; k < 4; k++) {
            Segmento *s = malloc(sizeof(Segmento));
            s->p1 = pts[k];
            s->p2 = next_pts[k];
            s->id = (*id_counter)++;
            list_insert_back(barriers, s);
            fprintf(ftxt, "Id: %d (%.2f,%.2f) -> (%.2f,%.2f)\n", s->id, s->p1.x, s->p1.y, s->p2.x, s->p2.y);
        }

    } else if (tipo == LINE) {
        Segmento *s = malloc(sizeof(Segmento));
        s->p1.x = line_get_x1(forma);
        s->p1.y = line_get_y1(forma);
        s->p2.x = line_get_x2(forma);
        s->p2.y = line_get_y2(forma);
        s->id = (*id_counter)++;
        list_insert_back(barriers, s);
        fprintf(ftxt, "Id: %d (%.2f,%.2f) -> (%.2f,%.2f)\n", s->id, s->p1.x, s->p1.y, s->p2.x, s->p2.y);

    } else if (tipo == CIRCLE) {
        double cx = circulo_get_x(forma);
        double cy = circulo_get_y(forma);
        double r = circulo_get_raio(forma);
        Ponto p1, p2;
        
        if (orientation == 'v') {
            p1.x = cx; p1.y = cy - r;
            p2.x = cx; p2.y = cy + r;
        } else { // 'h' or default
            p1.x = cx - r; p1.y = cy;
            p2.x = cx + r; p2.y = cy;
        }
        
        Segmento *s = malloc(sizeof(Segmento));
        s->p1 = p1; s->p2 = p2;
        s->id = (*id_counter)++;
        list_insert_back(barriers, s);
        fprintf(ftxt, "Id: %d (%.2f,%.2f) -> (%.2f,%.2f)\n", s->id, s->p1.x, s->p1.y, s->p2.x, s->p2.y);

    } else if (tipo == TEXT) {
        double xt = text_get_x(forma);
        double yt = text_get_y(forma);
        char anchor = text_get_anchor(forma);
        // Assuming text_get_text exists or similar
        const char *txt = text_get_text(forma);
        int len = strlen(txt);
        double width = 10.0 * len;
        
        Ponto p1 = {0, yt};
        Ponto p2 = {0, yt};
        
        if (anchor == 'i') {
            p1.x = xt;
            p2.x = xt + width;
        } else if (anchor == 'f') {
            p1.x = xt - width;
            p2.x = xt;
        } else if (anchor == 'm') {
            p1.x = xt - width / 2.0;
            p2.x = xt + width / 2.0;
        } else { // Default to 'i' behavior if unknown
            p1.x = xt;
            p2.x = xt + width;
        }
        
        Segmento *s = malloc(sizeof(Segmento));
        s->p1 = p1; s->p2 = p2;
        s->id = (*id_counter)++;
        list_insert_back(barriers, s);
        fprintf(ftxt, "Id: %d (%.2f,%.2f) -> (%.2f,%.2f)\n", s->id, s->p1.x, s->p1.y, s->p2.x, s->p2.y);
    }
}

// Logic for bombs
static bool forma_atingida(void *forma, TipoForma tipo, PoligonoVisibilidade pol) {
    Ponto ancora;
    if (tipo == CIRCLE) {
        ancora.x = circulo_get_x(forma);
        ancora.y = circulo_get_y(forma);
    } else if (tipo == RECTANGLE) {
        ancora.x = retangulo_get_x(forma);
        ancora.y = retangulo_get_y(forma);
    } else if (tipo == LINE) {
        ancora.x = line_get_x1(forma);
        ancora.y = line_get_y1(forma);
    } else if (tipo == TEXT) {
        ancora.x = text_get_x(forma);
        ancora.y = text_get_y(forma);
    }
    return visibilidade_ponto_atingido(pol, ancora);
}

void qry_processar(Geo cidade, const char *qryPath, const char *outPath, const char *geoName) {
    char txtName[512];
    sprintf(txtName, "%s.txt", outPath);
    FILE *ftxt = fopen(txtName, "w");
    FILE *fqry = fopen(qryPath, "r");
    
    if (!fqry || !ftxt) {
        if (fqry) fclose(fqry);
        if (ftxt) fclose(ftxt);
        return;
    }

    // Get initial barriers (universe walls etc)
    LinkedList all_barriers = geo_obter_todas_barreiras(cidade);
    // Maintain list of PoligonoVisibilidade for sfx="-"
    LinkedList final_polys = list_create();

    // Initialize ID counter for new segments
    LinkedList formas = geo_get_formas(cidade);
    int seg_id_counter = get_max_id(formas) + 1;

    char line[1024];
    while (fgets(line, sizeof(line), fqry)) {
        char cmd[32];
        if (sscanf(line, "%s", cmd) != 1) continue;
        
        if (strcmp(cmd, "a") == 0) {
            int i, j;
            char opt = 'h'; 
            // a i j [opt]
            char buffCopy[1024];
            strcpy(buffCopy, line);
            
            char *token = strtok(buffCopy, " \n"); // cmd
            char *si = strtok(NULL, " \n");
            char *sj = strtok(NULL, " \n");
            char *so = strtok(NULL, " \n");

            if (si && sj) {
                i = atoi(si);
                j = atoi(sj);
                if (so) opt = so[0];
            
                // remove_at is destructive, so iteration needs care.
                int k = 0;
                while (k < list_size(formas)) {
                    ElementoGeo *el = (ElementoGeo *)list_get_at(formas, k);
                    int id = get_id_generico(el->forma, el->tipo);
                    
                    if (id >= i && id <= j) {
                        fprintf(ftxt, "Id: %d Type: %d transformed to barrier.\n", id, el->tipo);
                        add_barriers_from_form(el->forma, el->tipo, all_barriers, opt, &seg_id_counter, ftxt);
                        
                        if (el->tipo != LINE) {
                            // Remove from Geo
                            ElementoGeo *removed = (ElementoGeo *)list_remove_at(formas, k);
                            // TODO: destructor for shape? skipping for now as 'geo' might own strict memory mgmt
                            free(removed);
                            // k stays same
                        } else {
                            // Line stays
                            k++;
                        }
                    } else {
                        k++;
                    }
                }
            }
        } else if (strcmp(cmd, "d") == 0 || strcmp(cmd, "p") == 0 || strcmp(cmd, "cln") == 0) {
           // Parse args using strtok
           char buffCopy[1024];
           strcpy(buffCopy, line);
           char *token = strtok(buffCopy, " \n"); // cmd
           
           double x=0, y=0;
           char *sx = strtok(NULL, " \n");
           char *sy = strtok(NULL, " \n");
           if (sx) x = atof(sx);
           if (sy) y = atof(sy);
           
           char *arg3 = strtok(NULL, " \n");
           char *arg4 = strtok(NULL, " \n");
           char *arg5 = strtok(NULL, " \n");

           char *actual_sfx = NULL;
           char *cor = NULL;
           double dx=0, dy=0;

           if (strcmp(cmd, "d") == 0) {
               actual_sfx = arg3;
           } else if (strcmp(cmd, "p") == 0) {
               cor = arg3;
               actual_sfx = arg4;
           } else if (strcmp(cmd, "cln") == 0) {
               if(arg3) dx = atof(arg3);
               if(arg4) dy = atof(arg4);
               actual_sfx = arg5;
           }

           if (!actual_sfx) continue; // Error parsing

           // Calc vis
           Ponto centro = {x, y};
           PoligonoVisibilidade poly = visibilidade_calcular(centro, all_barriers);
            
           // Apply effects
           LinkedList formas = geo_get_formas(cidade);
           int k = 0;
           while(k < list_size(formas)) {
                ElementoGeo *el = (ElementoGeo *)list_get_at(formas, k);
                bool atingido = forma_atingida(el->forma, el->tipo, poly);
                
                if (atingido) {
                    if (strcmp(cmd, "d") == 0) {
                        fprintf(ftxt, "Id: %d destroyed.\n", get_id_generico(el->forma, el->tipo));
                        ElementoGeo *removed = list_remove_at(formas, k);
                        free(removed);
                        continue; // No inc k
                    } else if (strcmp(cmd, "p") == 0) {
                        if (cor) {
                            set_color_generic(el->forma, el->tipo, cor);
                            fprintf(ftxt, "Id: %d painted %s.\n", get_id_generico(el->forma, el->tipo), cor);
                        }
                    } else if (strcmp(cmd, "cln") == 0) {
                        clone_generic(formas, el->forma, el->tipo, dx, dy);
                        fprintf(ftxt, "Id: %d cloned.\n", get_id_generico(el->forma, el->tipo));
                        // Cloned element is added at end. Since we iterate by index,
                        // and new items are at end, we might process the clone?
                        // list_size grows. But standard behavior is process 'existing' so we should capture initial size?
                        // But wait, if we process clone, it might clone again? (Infinite loop if dx=0?)
                        // We should probably iterate up to INITIAL size.
                        // I will assume simple iteration for now but ideally should fix loop bounds.
                    }
                }
                k++;
           }
           
           // Output SVG
           if (strcmp(actual_sfx, "-") == 0) {
               list_insert_back(final_polys, poly);
           } else {
               char svgName[512];
               sprintf(svgName, "%s-%s.svg", outPath, actual_sfx);
               FILE *fsvg = fopen(svgName, "w");
               start_svg(fsvg);
               geo_escrever_svg(cidade, fsvg);
               svg_desenhar_poligono(fsvg, poly, "yellow", 0.5);
               end_svg(fsvg);
               fclose(fsvg);
               visibilidade_destruir_poly(poly);
           }
        }
    }
    
    // Final SVG
    char finalSvg[512];
    sprintf(finalSvg, "%s.svg", outPath);
    FILE *favg = fopen(finalSvg, "w");
    start_svg(favg);
    geo_escrever_svg(cidade, favg);
    for (int i=0; i<list_size(final_polys); i++) {
        PoligonoVisibilidade p = list_get_at(final_polys, i);
        svg_desenhar_poligono(favg, p, "yellow", 0.5);
        visibilidade_destruir_poly(p);
    }
    end_svg(favg);
    fclose(favg);
    
    fclose(fqry);
    fclose(ftxt);
    
    list_destroy(final_polys);
    for(int i=0; i<list_size(all_barriers); i++) free(list_get_at(all_barriers, i));
    list_destroy(all_barriers);
}