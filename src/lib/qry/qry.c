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

// Helper for TXT report types
static const char* get_type_str(TipoForma t) {
    switch(t) {
        case CIRCLE: return "c";
        case RECTANGLE: return "r";
        case LINE: return "l";
        case TEXT: return "t";
        default: return "?";
    }
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
static void add_barriers_from_form(void *forma, TipoForma tipo, LinkedList barriers, char orientation, int *id_counter, FILE *ftxt, int id_original) {
    // Determine color
    const char *cor = "black";
    if (tipo == RECTANGLE) cor = retangulo_get_cor_borda(forma);
    else if (tipo == CIRCLE) cor = circulo_get_cor_borda(forma);
    else if (tipo == LINE) cor = line_get_color(forma);
    else if (tipo == TEXT) cor = text_get_border_color(forma);

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

        int ids[4];
        for(int k=0; k<4; k++) ids[k] = (*id_counter)++;

        for (int k = 0; k < 4; k++) {
            Segmento *s = malloc(sizeof(Segmento));
            s->p1 = pts[k];
            s->p2 = next_pts[k];
            s->id = ids[k];
            strncpy(s->cor, cor, 31);
            list_insert_back(barriers, s);
            // OUTPUT FORMAT: \tID (TYPE) -> ID (anteparo)
            fprintf(ftxt, "\t%d (%s) -> %d (anteparo)\n", id_original, get_type_str(tipo), s->id);
        }

    } else if (tipo == LINE) {
        Segmento *s = malloc(sizeof(Segmento));
        s->p1.x = line_get_x1(forma);
        s->p1.y = line_get_y1(forma);
        s->p2.x = line_get_x2(forma);
        s->p2.y = line_get_y2(forma);
        s->id = (*id_counter)++;
        strncpy(s->cor, cor, 31);
        list_insert_back(barriers, s);
        fprintf(ftxt, "\t%d (%s) -> %d (anteparo)\n", id_original, get_type_str(tipo), s->id);

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
        strncpy(s->cor, cor, 31);
        list_insert_back(barriers, s);
        fprintf(ftxt, "\t%d (%s) -> %d (anteparo)\n", id_original, get_type_str(tipo), s->id);

    } else if (tipo == TEXT) {
        double xt = text_get_x(forma);
        double yt = text_get_y(forma);
        char anchor = text_get_anchor(forma);
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
        strncpy(s->cor, cor, 31);
        list_insert_back(barriers, s);
        fprintf(ftxt, "\t%d (%s) -> %d (anteparo)\n", id_original, get_type_str(tipo), s->id);
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
    
    // Initialize ID counter and fix initial barrier IDs
    LinkedList formas = geo_get_formas(cidade);
    int seg_id_counter = get_max_id(formas) + 1;

    // Accumulate ALL generated polygons for the final SVG
    LinkedList accumulated_polys = list_create();

    for(int i=0; i<list_size(all_barriers); i++) {
        Segmento *s = list_get_at(all_barriers, i);
        if (s->id == 0) s->id = seg_id_counter++; // Assign unique ID if 0
        strncpy(s->cor, "black", 31);
    }

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
            
                fprintf(ftxt, "[*] a\n");
                
                int k = 0;
                while (k < list_size(formas)) {
                    ElementoGeo *el = (ElementoGeo *)list_get_at(formas, k);
                    int id = get_id_generico(el->forma, el->tipo);
                    
                    if (id >= i && id <= j) {
                        add_barriers_from_form(el->forma, el->tipo, all_barriers, opt, &seg_id_counter, ftxt, id);
                        
                        // Remove from Geo (including LINEs as per srcAndre behavior)
                        ElementoGeo *removed = (ElementoGeo *)list_remove_at(formas, k);
                        // cleanup
                        free(removed);
                    } else {
                        k++;
                    }
                }
            }
        } else if (strcmp(cmd, "d") == 0 || strcmp(cmd, "p") == 0 || strcmp(cmd, "cln") == 0) {
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
           
           // Print Command Header
           if (strcmp(cmd, "d") == 0) {
               fprintf(ftxt, "[*] d x=%.2f y=%.2f\n", x, y);
           } else if (strcmp(cmd, "p") == 0) {
               fprintf(ftxt, "[*] p x=%.2f y=%.2f %s\n", x, y, cor);
           } else if (strcmp(cmd, "cln") == 0) {
               fprintf(ftxt, "[*] cln x=%.2f y=%.2f dx=%.2f dy=%.2f\n", x, y, dx, dy);
           }

           // Calc vis
           Ponto centro = {x, y};
           PoligonoVisibilidade poly = visibilidade_calcular(centro, all_barriers);
           
           // Store for final SVG
           list_insert_back(accumulated_polys, poly);
            
           // Apply effects to SHAPES
           int k = 0;
           while(k < list_size(formas)) {
                ElementoGeo *el = (ElementoGeo *)list_get_at(formas, k);
                bool atingido = forma_atingida(el->forma, el->tipo, poly);
                
                if (atingido) {
                    int id_shape = get_id_generico(el->forma, el->tipo);
                    if (strcmp(cmd, "d") == 0) {
                        fprintf(ftxt, "\t%d %s\n", id_shape, get_type_str(el->tipo));
                        ElementoGeo *removed = list_remove_at(formas, k);
                        free(removed);
                        continue; 
                    } else if (strcmp(cmd, "p") == 0) {
                        if (cor) {
                            set_color_generic(el->forma, el->tipo, cor);
                            fprintf(ftxt, "\t%d %s\n", id_shape, get_type_str(el->tipo));
                        }
                    } else if (strcmp(cmd, "cln") == 0) {
                        clone_generic(formas, el->forma, el->tipo, dx, dy);
                        // clone_generic adds to 'formas', need to get ID of added? 
                        // It adds to back.
                        ElementoGeo *new_el = list_get_at(formas, list_size(formas)-1);
                        int new_id = get_id_generico(new_el->forma, new_el->tipo);
                        fprintf(ftxt, "\t%d %s (clone do %d %s)\n", new_id, get_type_str(new_el->tipo), id_shape, get_type_str(el->tipo));
                    }
                }
                k++;
           }

           // EFFECT FOR BARRIERS (Anteparos)
           k = 0;
           int initial_barrier_count = list_size(all_barriers); // Fix infinite cloning
           while (k < initial_barrier_count) {
               Segmento *s = (Segmento*)list_get_at(all_barriers, k);
               bool atingido = visibilidade_ponto_atingido(poly, s->p1); // Use p1 as anchor
               
               if (atingido && s->id >= 0) { // Don't allow destroying bbox (id -1)
                   if (strcmp(cmd, "d") == 0) {
                       fprintf(ftxt, "\t%d l\n", s->id); // Using 'l' (line) for barrier
                       Segmento *rem = list_remove_at(all_barriers, k);
                       free(rem);
                       k--;
                       initial_barrier_count--; 
                   } else if (strcmp(cmd, "p") == 0) {
                       if (cor) {
                           strncpy(s->cor, cor, 31);
                           fprintf(ftxt, "\t%d l\n", s->id);
                       }
                   } else if (strcmp(cmd, "cln") == 0) {
                       // Clone barrier
                       Segmento *new_s = malloc(sizeof(Segmento));
                       *new_s = *s;
                       new_s->p1.x += dx; new_s->p1.y += dy;
                       new_s->p2.x += dx; new_s->p2.y += dy;
                       new_s->id = seg_id_counter++; // New ID
                       fprintf(ftxt, "\t%d l (clone do %d l)\n", new_s->id, s->id);
                       list_insert_back(all_barriers, new_s);
                       // Not counting new barrier for this bomb pass
                   }
               }
               k++;
           }

        } // End command block
    }
    
    // Final Combined SVG
    char finalSvg[512];
    sprintf(finalSvg, "%s.svg", outPath);
    FILE *favg = fopen(finalSvg, "w");
    start_svg(favg);
    
    // 1. Draw Final Shapes
    geo_escrever_svg(cidade, favg);
    
    // 2. Draw Anteparo Segments (Lines) with color
    for(int i=0; i<list_size(all_barriers); i++) {
        Segmento *s = list_get_at(all_barriers, i);
        if (s->id == 0) continue; 
        
        fprintf(favg, "<line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" y2=\"%.2f\" stroke=\"%s\" stroke-width=\"1\" />\n",
            s->p1.x, s->p1.y, s->p2.x, s->p2.y, s->cor);
    }

    // 3. Draw All Polygons
    for (int i=0; i<list_size(accumulated_polys); i++) {
        PoligonoVisibilidade p = list_get_at(accumulated_polys, i);
        svg_desenhar_poligono(favg, p, "yellow", 0.5);
    }
    
    end_svg(favg);
    fclose(favg);
    
    fclose(fqry);
    fclose(ftxt);
    
    for(int i=0; i<list_size(accumulated_polys); i++) visibilidade_destruir_poly(list_get_at(accumulated_polys, i));
    list_destroy(accumulated_polys);
    for(int i=0; i<list_size(all_barriers); i++) free(list_get_at(all_barriers, i));
    list_destroy(all_barriers);
}