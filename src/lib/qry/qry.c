#include "qry.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

// Definição local de ElementoGeo (compatível com geo.c)
typedef struct { TipoForma tipo; void *forma; } ElementoGeo;

// Funções auxiliares locais
static Ponto obter_ancora(void* forma, TipoForma tipo) {
    Ponto p = {0,0};
    if (tipo == CIRCLE) { p.x = circulo_get_x(forma); p.y = circulo_get_y(forma); }
    else if (tipo == RECTANGLE) { p.x = retangulo_get_x(forma); p.y = retangulo_get_y(forma); }
    else if (tipo == LINE) { p.x = line_get_x1(forma); p.y = line_get_y1(forma); }
    else if (tipo == TEXT) { p.x = text_get_x(forma); p.y = text_get_y(forma); }
    return p;
}

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

static int g_anteparo_id_counter = 5000;

static void adicionar_forma_geo(LinkedList formas, void* nova_forma, TipoForma tipo) {
    ElementoGeo* novo_el = malloc(sizeof(ElementoGeo));
    novo_el->tipo = tipo;
    novo_el->forma = nova_forma;
    list_insert_back(formas, novo_el);
}

void qry_processar(Geo cidade, const char* qryPath, const char* outPath, const char* geoName) {
    // Preparar nomes de arquivos
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

    // Inicializa o SVG Final (acumulador)
    double min_x, min_y, max_x, max_y;
    geo_get_bounding_box(cidade, &min_x, &min_y, &max_x, &max_y);
    double margin = 100.0; // Margem de segurança visual
    
    if(fsvg_final) {
        svg_iniciar(fsvg_final, min_x - margin, min_y - margin, (max_x - min_x) + 2*margin, (max_y - min_y) + 2*margin);
        svg_desenhar_cidade(fsvg_final, cidade);
    }

    if (!fqry) {
        if(ftxt) fclose(ftxt);
        if(fsvg_final) fclose(fsvg_final);
        return;
    }

    char linha[1024];
    while (fgets(linha, sizeof(linha), fqry)) {
        char cmd[10];
        // Lê o comando
        if (sscanf(linha, "%s", cmd) != 1) continue;

        // Variáveis comuns às bombas
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

                        double x1, y1, x2, y2;
                        if (orientacao == 'h') {
                            x1 = cx - r; y1 = cy;
                            x2 = cx + r; y2 = cy;
                        } else {
                            x1 = cx; y1 = cy - r;
                            x2 = cx; y2 = cy + r;
                        }

                        int new_id = g_anteparo_id_counter++;
                        void* nova_linha = line_create(new_id, x1, y1, x2, y2, cor_borda);
                        adicionar_forma_geo(novas_formas, nova_linha, LINE);

                        fprintf(ftxt, "\t%d (%s) -> %d (anteparo) %.2f %.2f %.2f %.2f\n", 
                                id, obter_tipo_str(CIRCLE), new_id, x1, y1, x2, y2);
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
        else if (strcmp(cmd, "d") == 0) { // Destruição: d x y sfx
            sscanf(linha, "%*s %lf %lf %s", &x, &y, sfx);
            fprintf(ftxt, "[*] d x=%.2f y=%.2f\n", x, y);
            is_bomb = true;
        } 
        else if (strcmp(cmd, "p") == 0) { // Pintura: p x y cor sfx
            sscanf(linha, "%*s %lf %lf %s %s", &x, &y, cor, sfx); // cor vem antes do sfx no qry normal? Ver PDF.
            // PDF: p x y cor sfx. Ajustado.
            fprintf(ftxt, "[*] p x=%.2f y=%.2f %s\n", x, y, cor);
            is_bomb = true;
        }
        else if (strcmp(cmd, "cln") == 0) { // Clonagem: cln x y dx dy sfx
            sscanf(linha, "%*s %lf %lf %lf %lf %s", &x, &y, &dx, &dy, sfx);
            fprintf(ftxt, "[*] cln x=%.2f y=%.2f dx=%.2f dy=%.2f\n", x, y, dx, dy);
            is_bomb = true;
        }

        if (is_bomb) {
            Ponto bomba = {x, y};

            // 1. Preparar Barreiras (Segmentos)
            LinkedList barreiras = geo_obter_todas_barreiras(cidade);
            LinkedList biombo = geo_gerar_biombo(cidade, bomba);

            while(!list_is_empty(biombo)) {
                list_insert_back(barreiras, list_remove_front(biombo));
            }
            list_destroy(biombo);

            // 2. Calcular Polígono de Visibilidade
            PoligonoVisibilidade pol = visibilidade_calcular(bomba, barreiras);

            // 3. Processar Atingidos (Destruir, Pintar, Clonar)
            LinkedList formas = geo_get_formas(cidade);
            int n = list_size(formas);
            
            // Lista temporária para remoções (segurança de memória)
            LinkedList to_remove_ids = list_create(); 

            for(int i = 0; i < n; i++) {
                ElementoGeo* el = (ElementoGeo*)list_get_at(formas, i);
                Ponto anc = obter_ancora(el->forma, el->tipo);
                
                if (visibilidade_ponto_atingido(pol, anc)) {
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
                        geo_clonar_forma(cidade, id); // Nota: clonagem pode precisar de dx/dy na API geo
                    }
                }
            }

            // Aplicar remoções fora do loop principal
            while(!list_is_empty(to_remove_ids)) {
                int* id_ptr = (int*)list_remove_front(to_remove_ids);
                geo_remover_forma(cidade, *id_ptr);
                free(id_ptr);
            }
            list_destroy(to_remove_ids);

            // 4. Desenhar SVG
            if (strcmp(sfx, "-") == 0) {
                // Acumula no final
                if (fsvg_final) svg_desenhar_poligono(fsvg_final, pol, "yellow", 0.5);
            } else {
                // Cria arquivo snapshot separado
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
            }

            // Limpeza
            visibilidade_destruir(pol);
            // Liberar memória dos segmentos barreira
            while(!list_is_empty(barreiras)) free(list_remove_front(barreiras));
            list_destroy(barreiras);
        }
    }

    if (fsvg_final) {
        // Redesenha a cidade por cima ou por baixo? O PDF diz que o polígono deve aparecer. 
        // Se desenharmos a cidade DEPOIS, ela fica em cima do polígono (melhor para ver as formas).
        // Se desenharmos ANTES (já feito no init), o polígono cobre.
        // Vamos finalizar.
        svg_finalizar(fsvg_final);
        fclose(fsvg_final);
    }
    if (ftxt) fclose(ftxt);
    if (fqry) fclose(fqry);
}