#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "geo_handler/geo_handler.h"

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
    }

    if (!inputs_idx || !output_dir)
    {
        printf("Uso: %s -f <arquivo.geo> -o <diretorio_saida> [-q <arquivo.qry>]\n", argv[0]);
        return 1;
    }

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
    // Opcional: definir viewBox se soubermos os limites, mas por enquanto livre.
    
    geo_escrever_svg(geo, svg_file);
    
    fprintf(svg_file, "</svg>\n");
    fclose(svg_file);

    // 4. Limpeza
    geo_destruir(geo);

    printf("SVG gerado com sucesso em: %s\n", svg_path);

    return 0;
}
