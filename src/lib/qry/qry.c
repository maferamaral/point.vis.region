#include "qry.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "../visibilidade/visibilidade.h"
#include "../geo/geo.h"
#include "../svg/svg.h"
#include "../geometria/geometria.h"
#include "../utils/lista/lista.h"
#include "../formas/circulo/circulo.h"
#include "../formas/retangulo/retangulo.h"
#include "../formas/linha/linha.h"
#include "../formas/texto/texto.h"
#include "../formas/formas.h"

typedef struct { TipoForma tipo; void *forma; } ElementoGeo;

static int obter_id(void* forma, TipoForma tipo) {
    if (tipo == CIRCLE) return circulo_get_id(forma);
    if (tipo == RECTANGLE) return retangulo_get_id(forma);
    if (tipo == LINE) return line_get_id(forma);
    if (tipo == TEXT) return text_get_id(forma);
    return -1;
}

static const char* obter_tipo_str(TipoForma tipo) {
    if (tipo == CIRCLE) return "c";
    if (tipo == RECTANGLE) return "r";
    if (tipo == LINE) return "l";
    if (tipo == TEXT) return "t";
    return "?";
}

static bool forma_foi_atingida(PoligonoVisibilidade pol, ElementoGeo* el) {
    if (!pol) return false;

    if (el->tipo == RECTANGLE) {
        double x = retangulo_get_x(el->forma);
        double y = retangulo_get_y(el->forma);
        double w = retangulo_get_largura(el->forma);
        double h = retangulo_get_altura(el->forma);
        
        Ponto p1 = {x, y};
        Ponto p2 = {x + w, y};
        Ponto p3 = {x + w, y + h};
        Ponto p4 = {x, y + h};
        
        if (visibilidade_ponto_atingido(pol, p1)) return true;
        if (visibilidade_ponto_atingido(pol, p2)) return true;
        if (visibilidade_ponto_atingido(pol, p3)) return true;
        if (visibilidade_ponto_atingido(pol, p4)) return true;
        
        Ponto centro = {x + w/2, y + h/2};
        if (visibilidade_ponto_atingido(pol, centro)) return true;

    } else if (el->tipo == LINE) {
        Ponto p1 = {line_get_x1(el->forma), line_get_y1(el->forma)};
        Ponto p2 = {line_get_x2(el->forma), line_get_y2(el->forma)};
        if (visibilidade_ponto_atingido(pol, p1)) return true;
        if (visibilidade_ponto_atingido(pol, p2)) return true;
        Ponto pm = {(p1.x + p2.x)/2, (p1.y + p2.y)/2};
        if (visibilidade_ponto_atingido(pol, pm)) return true;

    } else if (el->tipo == TEXT) {
        double x = text_get_x(el->forma);
        double y = text_get_y(el->forma);
        Ponto p1 = {x, y};
        if (visibilidade_ponto_atingido(pol, p1)) return true;

    } else if (el->tipo == CIRCLE) {
        double cx = circulo_get_x(el->forma);
        double cy = circulo_get_y(el->forma);
        double r = circulo_get_raio(el->forma);
        
        Ponto c = {cx, cy};
        if (visibilidade_ponto_atingido(pol, c)) return true;
        
        Ponto p1 = {cx + r, cy};
        Ponto p2 = {cx - r, cy};
        Ponto p3 = {cx, cy + r};
        Ponto p4 = {cx, cy - r};
        
        if (visibilidade_ponto_atingido(pol, p1)) return true;
        if (visibilidade_ponto_atingido(pol, p2)) return true;
        if (visibilidade_ponto_atingido(pol, p3)) return true;
        if (visibilidade_ponto_atingido(pol, p4)) return true;
    }
    
    return false;
}

static int g_anteparo_id_counter = 5000;

static void adicionar_forma_geo(LinkedList formas, void* nova_forma, TipoForma tipo) {
    ElementoGeo* novo_el = malloc(sizeof(ElementoGeo));
    novo_el->tipo = tipo;
    novo_el->forma = nova_forma;
    list_insert_back(formas, novo_el);
}

void qry_processar(Geo cidade, const char* qryPath, const char* outPath, const char* geoName) {
    char *qryBase = strrchr(qryPath, '/');
    qryBase = (qryBase) ? qryBase + 1 : (char*)qryPath;
    char qryNoExt[100]; strcpy(qryNoExt, qryBase); 
    char *dot = strrchr(qryNoExt, '.'); if(dot) *dot = 0;

    char svgFinalPath[512], txtPath[512];
    sprintf(svgFinalPath, "%s/%s-%s.svg", outPath, geoName, qryNoExt);
    sprintf(txtPath, "%s/%s-%s.txt", outPath, geoName, qryNoExt);

    FILE *fqry = fopen(qryPath, "r");
    FILE *ftxt = fopen(txtPath, "w");
    FILE *fsvg_final = fopen(svgFinalPath, "w");

    double min_x, min_y, max_x, max_y;
    geo_get_bounding_box(cidade, &min_x, &min_y, &max_x, &max_y);
    double margin = 100.0; 
    
    // Store visibility polygons to draw at the end (after all commands are processed)
    LinkedList visibility_polygons = list_create();

    if (!fqry) {
        if(ftxt) fclose(ftxt);
        if(fsvg_final) fclose(fsvg_final);
        list_destroy(visibility_polygons);
        return;
    }

    char linha[1024];
    while (fgets(linha, sizeof(linha), fqry)) {
        char cmd[10];
        if (sscanf(linha, "%s", cmd) != 1) continue;

        double x = 0, y = 0;
        char sfx[100] = "";
        char cor[50] = "";
        double dx = 0, dy = 0;
        bool is_bomb = false;

        int id_ini, id_fim;
        char orientacao = 'v';

        if (strcmp(cmd, "a") == 0) {
            int lidos = sscanf(linha, "%*s %d %d %c", &id_ini, &id_fim, &orientacao);
            if (lidos < 3) orientacao = 'v';

            fprintf(ftxt, "[*] a\n");

            LinkedList formas = geo_get_formas(cidade);
            LinkedList novas_formas = list_create();
            LinkedList ids_remover = list_create();

            int n = list_size(formas);
            for (int i = 0; i < n; i++) {
                ElementoGeo* el = (ElementoGeo*)list_get_at(formas, i);
                int id = obter_id(el->forma, el->tipo);

                if (id >= id_ini && id <= id_fim) {
                    if (el->tipo == LINE) {
                        continue; 
                    }

                    int* ptr_id = malloc(sizeof(int)); *ptr_id = id;
                    list_insert_back(ids_remover, ptr_id);

                    if (el->tipo == CIRCLE) {
                        void* c = el->forma;
                        double cx = circulo_get_x(c);
                        double cy = circulo_get_y(c);
                        double r = circulo_get_raio(c);
                        const char* cor_borda = circulo_get_cor_borda(c);

                        if (orientacao == 'h') {
                            int id1 = g_anteparo_id_counter++;
                            void* l1 = line_create(id1, cx - r, cy, cx, cy, cor_borda);
                            adicionar_forma_geo(novas_formas, l1, LINE);
                            fprintf(ftxt, "\t%d (%s) -> %d (anteparo) %.2f %.2f %.2f %.2f\n", 
                                    id, obter_tipo_str(CIRCLE), id1, cx - r, cy, cx, cy);

                            int id2 = g_anteparo_id_counter++;
                            void* l2 = line_create(id2, cx, cy, cx + r, cy, cor_borda);
                            adicionar_forma_geo(novas_formas, l2, LINE);
                            fprintf(ftxt, "\t%d (%s) -> %d (anteparo) %.2f %.2f %.2f %.2f\n", 
                                    id, obter_tipo_str(CIRCLE), id2, cx, cy, cx + r, cy);
                        } else {
                            int id1 = g_anteparo_id_counter++;
                            void* l1 = line_create(id1, cx, cy - r, cx, cy, cor_borda);
                            adicionar_forma_geo(novas_formas, l1, LINE);
                            fprintf(ftxt, "\t%d (%s) -> %d (anteparo) %.2f %.2f %.2f %.2f\n", 
                                    id, obter_tipo_str(CIRCLE), id1, cx, cy - r, cx, cy);

                            int id2 = g_anteparo_id_counter++;
                            void* l2 = line_create(id2, cx, cy, cx, cy + r, cor_borda);
                            adicionar_forma_geo(novas_formas, l2, LINE);
                            fprintf(ftxt, "\t%d (%s) -> %d (anteparo) %.2f %.2f %.2f %.2f\n", 
                                    id, obter_tipo_str(CIRCLE), id2, cx, cy, cx, cy + r);
                        }
                    }
                    else if (el->tipo == RECTANGLE) {
                        void* r = el->forma;
                        double rx = retangulo_get_x(r);
                        double ry = retangulo_get_y(r);
                        double w = retangulo_get_largura(r);
                        double h = retangulo_get_altura(r);
                        const char* cor_borda = retangulo_get_cor_borda(r);

                        double coords[4][4] = {
                            {rx, ry, rx + w, ry},
                            {rx + w, ry, rx + w, ry + h},
                            {rx + w, ry + h, rx, ry + h},
                            {rx, ry + h, rx, ry}
                        };

                        for(int k=0; k<4; k++) {
                            int new_id = g_anteparo_id_counter++;
                            void* l = line_create(new_id, coords[k][0], coords[k][1], coords[k][2], coords[k][3], cor_borda);
                            adicionar_forma_geo(novas_formas, l, LINE);
                            fprintf(ftxt, "\t%d (%s) -> %d (anteparo) %.2f %.2f %.2f %.2f\n", 
                                    id, obter_tipo_str(RECTANGLE), new_id, coords[k][0], coords[k][1], coords[k][2], coords[k][3]);
                        }
                    }
                    else if (el->tipo == TEXT) {
                        void* t = el->forma;
                        double tx = text_get_x(t);
                        double ty = text_get_y(t);
                        char anchor = text_get_anchor(t);
                        const char* conteudo = text_get_text(t);
                        const char* cor_borda = text_get_border_color(t);
                        
                        double len = (double)strlen(conteudo);
                        double width = 10.0 * len;
                        
                        double x1, x2;
                        if (anchor == 'i' || anchor == 's') {
                            x1 = tx; 
                            x2 = tx + width;
                        } else if (anchor == 'f' || anchor == 'e') {
                            x1 = tx - width;
                            x2 = tx;
                        } else {
                            x1 = tx - width/2.0;
                            x2 = tx + width/2.0;
                        }
                        double y1 = ty; double y2 = ty;

                        int new_id = g_anteparo_id_counter++;
                        void* l = line_create(new_id, x1, y1, x2, y2, cor_borda);
                        adicionar_forma_geo(novas_formas, l, LINE);
                        
                        fprintf(ftxt, "\t%d (%s) -> %d (anteparo) %.2f %.2f %.2f %.2f\n", 
                                id, obter_tipo_str(TEXT), new_id, x1, y1, x2, y2);
                    }
                }
            }

            while(!list_is_empty(ids_remover)) {
                int* ptr = (int*)list_remove_front(ids_remover);
                geo_remover_forma(cidade, *ptr);
                free(ptr);
            }
            list_destroy(ids_remover);

            LinkedList lista_cidade = geo_get_formas(cidade);
            while(!list_is_empty(novas_formas)) {
                ElementoGeo* el = list_remove_front(novas_formas);
                list_insert_back(lista_cidade, el);
            }
            list_destroy(novas_formas);
        }
        else if (strcmp(cmd, "d") == 0) {
            sscanf(linha, "%*s %lf %lf %s", &x, &y, sfx);
            fprintf(ftxt, "[*] d x=%.2f y=%.2f\n", x, y);
            is_bomb = true;
        } 
        else if (strcmp(cmd, "p") == 0) {
            sscanf(linha, "%*s %lf %lf %s %s", &x, &y, cor, sfx);
            fprintf(ftxt, "[*] p x=%.2f y=%.2f %s\n", x, y, cor);
            is_bomb = true;
        }
        else if (strcmp(cmd, "cln") == 0) {
            sscanf(linha, "%*s %lf %lf %lf %lf %s", &x, &y, &dx, &dy, sfx);
            fprintf(ftxt, "[*] cln x=%.2f y=%.2f dx=%.2f dy=%.2f\n", x, y, dx, dy);
            is_bomb = true;
        }

        if (is_bomb) {
            Ponto bomba = {x, y};

            LinkedList barreiras = geo_obter_todas_barreiras(cidade);
            LinkedList biombo = geo_gerar_biombo(cidade, bomba);

            while(!list_is_empty(biombo)) {
                list_insert_back(barreiras, list_remove_front(biombo));
            }
            list_destroy(biombo);

            PoligonoVisibilidade pol = visibilidade_calcular(bomba, barreiras);

            LinkedList formas = geo_get_formas(cidade);
            int n = list_size(formas);
            LinkedList to_remove_ids = list_create(); 

            for(int i = 0; i < n; i++) {
                ElementoGeo* el = (ElementoGeo*)list_get_at(formas, i);
                
                if (forma_foi_atingida(pol, el)) {
                    int id = obter_id(el->forma, el->tipo);
                    
                    if (strcmp(cmd, "d") == 0) {
                        fprintf(ftxt, "\t%d %s\n", id, obter_tipo_str(el->tipo));
                        int* id_ptr = malloc(sizeof(int)); *id_ptr = id;
                        list_insert_back(to_remove_ids, id_ptr);
                    }
                    else if (strcmp(cmd, "p") == 0) {
                        fprintf(ftxt, "\t%d %s\n", id, obter_tipo_str(el->tipo));
                        geo_alterar_cor(cidade, id, cor);
                    }
                    else if (strcmp(cmd, "cln") == 0) {
                        fprintf(ftxt, "\t%d %s (clone do %d %s)\n", 
                            id + 10000, obter_tipo_str(el->tipo), id, obter_tipo_str(el->tipo));
                        geo_clonar_forma(cidade, id, dx, dy);
                    }
                }
            }

            while(!list_is_empty(to_remove_ids)) {
                int* id_ptr = (int*)list_remove_front(to_remove_ids);
                geo_remover_forma(cidade, *id_ptr);
                free(id_ptr);
            }
            list_destroy(to_remove_ids);

            if (strcmp(sfx, "-") == 0) {
                // Store the polygon to draw at the end (after city is drawn with final state)
                list_insert_back(visibility_polygons, pol);
            } else {
                char snapshotPath[512];
                sprintf(snapshotPath, "%s/%s-%s-%s.svg", outPath, geoName, qryNoExt, sfx);
                FILE *fsnap = fopen(snapshotPath, "w");
                if (fsnap) {
                    svg_iniciar(fsnap, min_x - margin, min_y - margin, (max_x - min_x) + 2*margin, (max_y - min_y) + 2*margin);
                    svg_desenhar_cidade(fsnap, cidade);
                    svg_desenhar_poligono(fsnap, pol, "yellow", 0.5);
                    svg_finalizar(fsnap);
                    fclose(fsnap);
                }
                visibilidade_destruir(pol);
            }
            while(!list_is_empty(barreiras)) free(list_remove_front(barreiras));
            list_destroy(barreiras);
        }
    }

    // Draw final SVG with city in current state (after all commands) and all visibility polygons
    if (fsvg_final) {
        svg_iniciar(fsvg_final, min_x - margin, min_y - margin, (max_x - min_x) + 2*margin, (max_y - min_y) + 2*margin);
        svg_desenhar_cidade(fsvg_final, cidade);
        
        // Draw all stored visibility polygons
        while(!list_is_empty(visibility_polygons)) {
            PoligonoVisibilidade pol = (PoligonoVisibilidade)list_remove_front(visibility_polygons);
            svg_desenhar_poligono(fsvg_final, pol, "yellow", 0.5);
            visibilidade_destruir(pol);
        }
        
        svg_finalizar(fsvg_final);
        fclose(fsvg_final);
    }
    
    list_destroy(visibility_polygons);
    if (ftxt) fclose(ftxt);
    if (fqry) fclose(fqry);
}