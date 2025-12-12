#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include "lib/geo/geo.h"
#include "lib/qry/qry.h"
#include "lib/visibilidade/visibilidade.h"
#include "lib/svg/svg.h"

// Cores para output no terminal
#define COLOR_RED     "\033[1;31m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_YELLOW  "\033[1;33m"
#define COLOR_RESET   "\033[0m"

// Função auxiliar para verificar se arquivo existe
int file_exists(const char *path) {
    struct stat st;
    return (stat(path, &st) == 0 && S_ISREG(st.st_mode));
}

// Função auxiliar para verificar se diretório existe
int dir_exists(const char *path) {
    struct stat st;
    return (stat(path, &st) == 0 && S_ISDIR(st.st_mode));
}

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

// Função para imprimir uso do programa
void print_usage(const char *prog_name) {
    printf("\n" COLOR_YELLOW "Uso:" COLOR_RESET "\n");
    printf("  %s -f <arquivo.geo> -o <diretorio_saida> [opções]\n\n", prog_name);
    printf(COLOR_YELLOW "Argumentos obrigatórios:" COLOR_RESET "\n");
    printf("  -f <arquivo.geo>    Arquivo de entrada com formas geométricas\n");
    printf("  -o <diretório>      Diretório de saída para arquivos SVG e TXT\n\n");
    printf(COLOR_YELLOW "Argumentos opcionais:" COLOR_RESET "\n");
    printf("  -e <caminho_base>   Prefixo de caminho para arquivos de entrada\n");
    printf("  -q <arquivo.qry>    Arquivo de consultas (comandos de bomba)\n");
    printf("  -to <tipo>          Tipo de ordenação: 'q'(quicksort), 'm'(mergesort)\n");
    printf("  -i                  Usar insertion sort\n\n");
    printf(COLOR_YELLOW "Exemplos:" COLOR_RESET "\n");
    printf("  %s -f cidade.geo -o saida\n", prog_name);
    printf("  %s -e dados -f mapa.geo -o resultado -q comandos.qry\n\n", prog_name);
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

    // ========== VALIDAÇÃO DE ARGUMENTOS ==========
    
    // Verificar se há argumentos
    if (argc < 2) {
        printf(COLOR_RED "Erro:" COLOR_RESET " Nenhum argumento fornecido.\n");
        print_usage(argv[0]);
        return 1;
    }

    // Verificar argumento -f (arquivo geo)
    if (!geo_name) {
        printf(COLOR_RED "Erro:" COLOR_RESET " Argumento obrigatório -f (arquivo .geo) não especificado.\n");
        print_usage(argv[0]);
        return 1;
    }

    // Verificar argumento -o (diretório de saída)
    if (!output_dir) {
        printf(COLOR_RED "Erro:" COLOR_RESET " Argumento obrigatório -o (diretório de saída) não especificado.\n");
        print_usage(argv[0]);
        return 1;
    }

    // ========== VALIDAÇÃO DE ARQUIVOS ==========

    // Concatenar caminho base com arquivo .geo
    char *full_geo_path = concat_path(base_path, geo_name);
    
    // Verificar se arquivo .geo existe
    if (!file_exists(full_geo_path)) {
        printf(COLOR_RED "Erro:" COLOR_RESET " Arquivo .geo não encontrado: %s\n", full_geo_path);
        if (base_path) {
            printf("       Dica: Verifique se o caminho base (-e %s) está correto.\n", base_path);
        }
        free(full_geo_path);
        return 1;
    }

    // Verificar se arquivo .qry existe (se especificado)
    char *full_qry_path = NULL;
    if (query_file) {
        full_qry_path = concat_path(base_path, query_file);
        if (!file_exists(full_qry_path)) {
            printf(COLOR_RED "Erro:" COLOR_RESET " Arquivo .qry não encontrado: %s\n", full_qry_path);
            if (base_path) {
                printf("       Dica: Verifique se o caminho base (-e %s) está correto.\n", base_path);
                printf("       Nota: O arquivo .qry deve ser relativo ao caminho base.\n");
            }
            free(full_geo_path);
            free(full_qry_path);
            return 1;
        }
    }

    // Verificar se diretório de saída existe ou pode ser criado
    if (!dir_exists(output_dir)) {
        printf(COLOR_YELLOW "Aviso:" COLOR_RESET " Diretório de saída não existe: %s\n", output_dir);
        printf("       Tentando criar...\n");
        
        if (mkdir(output_dir, 0755) != 0) {
            printf(COLOR_RED "Erro:" COLOR_RESET " Não foi possível criar diretório: %s (%s)\n", 
                   output_dir, strerror(errno));
            free(full_geo_path);
            if (full_qry_path) free(full_qry_path);
            return 1;
        }
        printf(COLOR_GREEN "       Diretório criado com sucesso." COLOR_RESET "\n");
    }

    // ========== CONFIGURAÇÃO ==========

    // Configurar ordenação
    if (insertion_flag) {
        visibilidade_set_sort_method('i');
    } else if (sort_arg) {
        char c = sort_arg[0];
        if (c == 'm' || c == 'q' || c == 'i') {
             visibilidade_set_sort_method(c);
        } else {
            printf(COLOR_YELLOW "Aviso:" COLOR_RESET " Tipo de ordenação '%s' não reconhecido. Usando padrão (quicksort).\n", sort_arg);
        }
    }

    // ========== PROCESSAMENTO ==========

    // 1. Criar e ler Geo
    printf("\n" COLOR_GREEN "[1/3]" COLOR_RESET " Lendo arquivo .geo: %s\n", full_geo_path);
    Geo geo = geo_criar();
    geo_ler(geo, full_geo_path);

    // 2. Preparar arquivo de saída SVG
    char filename[256];
    extract_filename(geo_name, filename);
    
    char svg_path[1024];
    snprintf(svg_path, sizeof(svg_path), "%s/%s.svg", output_dir, filename);

    FILE *svg_file = fopen(svg_path, "w");
    if (!svg_file) {
        printf(COLOR_RED "Erro:" COLOR_RESET " Não foi possível criar arquivo SVG: %s (%s)\n", 
               svg_path, strerror(errno));
        geo_destruir(geo);
        free(full_geo_path);
        if (full_qry_path) free(full_qry_path);
        return 1;
    }

    // 3. Escrever SVG
    printf(COLOR_GREEN "[2/3]" COLOR_RESET " Gerando SVG inicial: %s\n", svg_path);
    double min_x, min_y, max_x, max_y;
    geo_get_bounding_box(geo, &min_x, &min_y, &max_x, &max_y);
    double margin = 100.0;
    
    svg_iniciar(svg_file, min_x - margin, min_y - margin, (max_x - min_x) + 2*margin, (max_y - min_y) + 2*margin);
    geo_escrever_svg(geo, svg_file);
    svg_finalizar(svg_file);
    fclose(svg_file);

    // 4. Processar Consultas (.qry) se fornecido
    if (query_file) {
        printf(COLOR_GREEN "[3/3]" COLOR_RESET " Processando consultas: %s\n", full_qry_path);
        qry_processar(geo, full_qry_path, output_dir, filename);
        free(full_qry_path);
    } else {
        printf(COLOR_GREEN "[3/3]" COLOR_RESET " Nenhum arquivo .qry especificado. Pulando consultas.\n");
    }
    
    // 5. Limpeza
    free(full_geo_path);
    geo_destruir(geo);

    printf("\n" COLOR_GREEN "Concluído com sucesso!" COLOR_RESET "\n\n");
    return 0;
}