#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/geo/geo.h"
#include "lib/qry/qry.h"
#include "lib/visibilidade/visibilidade.h"
#include "lib/svg/svg.h"

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

// Função auxiliar para juntar caminhos (path + filename)
char *concat_path(const char *dir, const char *file) {
    if (!dir) return strdup(file);
    
    size_t len = strlen(dir) + strlen(file) + 2;
    char *full_path = malloc(len);
    
    // Verifica se o diretório termina com '/'
    if (dir[strlen(dir) - 1] == '/') {
        snprintf(full_path, len, "%s%s", dir, file);
    } else {
        snprintf(full_path, len, "%s/%s", dir, file);
    }
    return full_path;
}

// Função auxiliar para verificar presença de flag
int has_flag(int argc, char *argv[], const char *flag) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], flag) == 0) {
            return 1;
        }
    }
    return 0;
}

// Função auxiliar para obter valor de argumento
const char *get_arg_value(int argc, char *argv[], const char *flag) {
    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], flag) == 0) {
            return argv[i + 1];
        }
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    // Pegar argumentos
    const char *base_path = get_arg_value(argc, argv, "-e");
    const char *geo_name = get_arg_value(argc, argv, "-f");
    const char *output_dir = get_arg_value(argc, argv, "-o");
    const char *query_file = get_arg_value(argc, argv, "-q");

    const char *sort_arg = get_arg_value(argc, argv, "-to");
    int insertion_flag = has_flag(argc, argv, "-i");

    // Verificar argumentos obrigatórios
    if (!geo_name || !output_dir) {
        printf("Uso: %s [-e <base_path>] -f <arquivo.geo> -o <diretorio_saida> [-q <arquivo.qry>] [-to <sort_type>] [-i]\n", argv[0]);
        return 1;
    }

    // Configurar ordenação
    if (insertion_flag) {
        visibilidade_set_sort_method('i');
    } else if (sort_arg) {
        // Pode ser "mergesort", "m", etc. Vamos pegar a primeira letra se for m/q/i
        char c = sort_arg[0];
        if (c == 'm' || c == 'q' || c == 'i') {
             visibilidade_set_sort_method(c);
        }
    }

    // Concatenar caminho base com arquivo .geo
    char *full_geo_path = concat_path(base_path, geo_name);
    
    // 1. Criar e ler Geo
    Geo geo = geo_criar();
    geo_ler(geo, full_geo_path);

    // 2. Preparar arquivo de saída SVG
    char filename[256];
    extract_filename(geo_name, filename);
    
    char svg_path[1024];
    snprintf(svg_path, sizeof(svg_path), "%s/%s.svg", output_dir, filename);

    FILE *svg_file = fopen(svg_path, "w");
    if (!svg_file) {
        printf("Erro ao criar arquivo SVG: %s\n", svg_path);
        geo_destruir(geo);
        free(full_geo_path);
        return 1;
    }

    // 3. Escrever SVG
    double min_x, min_y, max_x, max_y;
    geo_get_bounding_box(geo, &min_x, &min_y, &max_x, &max_y);
    double margin = 100.0;
    
    svg_iniciar(svg_file, min_x - margin, min_y - margin, (max_x - min_x) + 2*margin, (max_y - min_y) + 2*margin);
    geo_escrever_svg(geo, svg_file);
    svg_finalizar(svg_file);
    fclose(svg_file);

    printf("SVG gerado com sucesso em: %s\n", svg_path);

    // 4. Processar Consultas (.qry) se fornecido
    if (query_file) {
        char *full_qry_path = concat_path(base_path, query_file);
        
        char qryName[256];
        extract_filename(query_file, qryName);

        qry_processar(geo, full_qry_path, output_dir, filename);
        
        free(full_qry_path);
    }
    
    // 5. Limpeza
    free(full_geo_path);
    geo_destruir(geo);

    return 0;
}