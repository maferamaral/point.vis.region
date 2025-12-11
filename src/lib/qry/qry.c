#include "qry.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../visibilidade/visibilidade.h"
#include "../geo/geo.h"
#include "../svg/svg.h"
#include "../geometria/geometria.h"
#include "../utils/lista/lista.h"
// Inclua os headers das formas
#include "../formas/circulo/circulo.h"
#include "../formas/retangulo/retangulo.h"
#include "../formas/linha/linha.h"
#include "../formas/texto/texto.h"
#include "../formas/formas.h"

// Definição local de ElementoGeo para acesso (deve ser compatível com geo.c)
typedef struct { TipoForma tipo; void *forma; } ElementoGeo;

// Função auxiliar para obter a âncora da forma
Ponto obter_ancora(void* forma, TipoForma tipo) {
    Ponto p = {0,0};
    if (tipo == CIRCLE) { p.x = circulo_get_x(forma); p.y = circulo_get_y(forma); }
    else if (tipo == RECTANGLE) { p.x = retangulo_get_x(forma); p.y = retangulo_get_y(forma); }
    else if (tipo == LINE) { p.x = line_get_x1(forma); p.y = line_get_y1(forma); }
    else if (tipo == TEXT) { p.x = text_get_x(forma); p.y = text_get_y(forma); }
    return p;
}

int obter_id(void* forma, TipoForma tipo) {
    if (tipo == CIRCLE) return circulo_get_id(forma);
    if (tipo == RECTANGLE) return retangulo_get_id(forma);
    if (tipo == LINE) return line_get_id(forma);
    if (tipo == TEXT) return text_get_id(forma);
    return -1;
}

void qry_processar(Geo cidade, const char* qryPath, const char* outPath, const char* geoName) {
    // Nomes dos arquivos de saída
    char *qryBase = strrchr(qryPath, '/');
    qryBase = (qryBase) ? qryBase + 1 : (char*)qryPath;
    char qryNoExt[100]; strcpy(qryNoExt, qryBase); 
    char *dot = strrchr(qryNoExt, '.'); if(dot) *dot = 0;

    char svgPath[512], txtPath[512];
    sprintf(svgPath, "%s/%s-%s.svg", outPath, geoName, qryNoExt);
    sprintf(txtPath, "%s/%s-%s.txt", outPath, geoName, qryNoExt);

    FILE *fqry = fopen(qryPath, "r");
    FILE *ftxt = fopen(txtPath, "w");
    FILE *fsvg = fopen(svgPath, "w");

    // Inicia SVG com a cidade
    if(fsvg) {
        double min_x, min_y, max_x, max_y;
        geo_get_bounding_box(cidade, &min_x, &min_y, &max_x, &max_y);
        double margin = 100.0;
        svg_iniciar(fsvg, min_x - margin, min_y - margin, (max_x - min_x) + 2*margin, (max_y - min_y) + 2*margin);
        svg_desenhar_cidade(fsvg, cidade);
    }

    if (!fqry) return;

    char linha[1024];
    while (fgets(linha, sizeof(linha), fqry)) {
        char cmd[10];
        sscanf(linha, "%s", cmd);

        if (cmd[0] == 'b') { // Bomba
            double bx, by;
            char cor[50] = "red"; 
            
            // Parsing simples (ajuste conforme PDF)
            // Formato: b? x y  OU  b/ x y  OU  b# x y cor
            int offset = 0;
            char temp[10];
            sscanf(linha, "%s %lf %lf%n", temp, &bx, &by, &offset);
            if (strcmp(cmd, "b#") == 0) sscanf(linha + offset, "%s", cor);

            Ponto bomba = {bx, by};
            fprintf(ftxt, "\nBomba %s em (%.2f, %.2f):\n", cmd, bx, by);

            // 1. Obter Barreiras (Paredes)
            LinkedList barreiras = geo_obter_todas_barreiras(cidade);
            
            // 2. Gerar e adicionar o Biombo (Universo)
            LinkedList biombo = geo_gerar_biombo(cidade, 500.0);
            
            // Transferir biombo para barreiras
            while(!list_is_empty(biombo)) {
                void* seg = list_remove_front(biombo);
                list_insert_back(barreiras, seg);
            }
            list_destroy(biombo); // Destroi apenas a estrutura da lista

            // 3. Calcular Visibilidade
            PoligonoVisibilidade pol = visibilidade_calcular(bomba, barreiras);

            // 4. Verificar Atingidos
            // ATENÇÃO: Aqui assumimos que geo_get_formas retorna a lista interna de ElementoGeo*
            LinkedList formas = geo_get_formas(cidade);
            int nformas = list_size(formas);
            
            for (int i = 0; i < nformas; i++) {
                ElementoGeo* el = (ElementoGeo*)list_get_at(formas, i);
                Ponto ancora = obter_ancora(el->forma, el->tipo);
                
                if (visibilidade_ponto_atingido(pol, ancora)) {
                    int id = obter_id(el->forma, el->tipo);
                    if (strcmp(cmd, "b/") == 0) {
                        fprintf(ftxt, " -> Forma ID %d destruida.\n", id);
                        // TODO: Implementar remoção segura da lista
                    } else if (strcmp(cmd, "b#") == 0) {
                        fprintf(ftxt, " -> Forma ID %d pintada de %s.\n", id, cor);
                        // TODO: Chamar set_cor
                    } else if (strcmp(cmd, "b?") == 0) {
                        fprintf(ftxt, " -> Forma ID %d clonada.\n", id);
                        // TODO: Chamar clone
                    }
                }
            }

            // Desenha o polígono no SVG para visualização
            if(fsvg) svg_desenhar_poligono(fsvg, pol, "yellow", 0.4);

            // Limpeza da iteração
            visibilidade_destruir(pol);
            while(!list_is_empty(barreiras)) free(list_remove_front(barreiras));
            list_destroy(barreiras);
        }
    }

    if(fsvg) { svg_finalizar(fsvg); fclose(fsvg); }
    if(fqry) fclose(fqry);
    if(ftxt) fclose(ftxt);
}