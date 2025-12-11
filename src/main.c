#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/geo/geo.h"
#include "lib/qry/qry.h"
#include "lib/visibilidade/visibilidade.h"

// Função auxiliar para extrair o nome do arquivo sem extensão e caminho
void extract_filename(const char *path, char *dest) {
    const char *slash = strrchr(path, '/');
    const char *start = slash ? slash + 1 : path;
    const char *dot = strrchr(start, '.');
    if (dot) {
        size_t len = dot - start;
        strncpy(dest, start, len);
        dest[len] = '\0';
    } else {
        strcpy(dest, start);
    }
}

int main(int argc, char *argv[])
{
    char *inputs_idx = NULL;
    char *output_dir = NULL;
    char *query_file = NULL;

    int insertion_limit = 10;
    char sort_type = 'q';

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-f") == 0 && i + 1 < argc)
        {
            inputs_idx = argv[++i];
        }
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc)
        {
            output_dir = argv[++i];
        }
        else if (strcmp(argv[i], "-q") == 0 && i + 1 < argc)
        {
            query_file = argv[++i];
        }
        else if (strcmp(argv[i], "-to") == 0 && i + 1 < argc)
        {
            sort_type = argv[++i][0];
        }
        else if (strcmp(argv[i], "-in") == 0 && i + 1 < argc)
        {
            insertion_limit = atoi(argv[++i]);
        }
    }

    if (!inputs_idx || !output_dir)
    {
        printf("Uso: %s -f <arquivo.geo> -o <diretorio_saida> [-q <arquivo.qry>] [-to <sort_type>] [-in <threshold>]\n", argv[0]);
        return 1;
    }
    
    // Configure sort globally
    visibilidade_set_sort_params(sort_type, insertion_limit);

    // 1. Criar e ler Geo
    Geo geo = geo_criar();
    geo_ler(geo, inputs_idx);

    // 2. Preparar arquivo de saída SVG
    char filename[256];
    extract_filename(inputs_idx, filename);
    
    char svg_path[512];
    sprintf(svg_path, "%s/%s.svg", output_dir, filename);

    FILE *svg_file = fopen(svg_path, "w");
    if (!svg_file) {
        printf("Erro ao criar arquivo SVG: %s\n", svg_path);
        geo_destruir(geo);
        return 1;
    }

    // 3. Escrever SVG
    fprintf(svg_file, "<svg xmlns=\"http://www.w3.org/2000/svg\">\n");
    geo_escrever_svg(geo, svg_file);
    fprintf(svg_file, "</svg>\n");
    fclose(svg_file);

    printf("SVG gerado com sucesso em: %s\n", svg_path);

    // 4. Processar Consultas (.qry) se fornecido
    if (query_file) {
        char qryName[256];
        extract_filename(query_file, qryName);

        char outPath[512];
        // Formato base: saida/nomegeo-nomeqry
        sprintf(outPath, "%s/%s-%s", output_dir, filename, qryName);

        qry_processar(geo, query_file, outPath, filename);
    }
    
    // 5. Limpeza
    geo_destruir(geo);

    return 0;
}
