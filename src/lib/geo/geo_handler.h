#ifndef GEO_HANDLER_H
#define GEO_HANDLER_H

#include "../manipuladorDeArquivo/manipuladorDeArquivo.h"

// Tipo opaco para armazenar o contexto de execução de comandos geométricos
typedef void *Ground;

// Executa os comandos geométricos lidos do arquivo
Ground execute_geo_commands(FileData fileData, const char *output_path,
                            const char *command_suffix);

// Destroi o contexto e libera memória
void destroy_geo_waste(Ground ground);

// Retorna a lista de formas do contexto
void *get_ground_queue(Ground ground);

// Retorna a lista de formas para limpeza
void *get_ground_shapes_stack_to_free(Ground ground);

#endif // GEO_HANDLER_H
