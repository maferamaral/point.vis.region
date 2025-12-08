#ifndef MANIPULADOR_DE_ARQUIVO_H
#define MANIPULADOR_DE_ARQUIVO_H
#include "../utils/lista/lista.h"
// Estrutura que armazena as linhas do arquivo em uma lista encadeada
typedef void *FileData;
// Lê um arquivo e retorna um ponteiro para FileData contendo as linhas em uma
// lista encadeada
FileData readFile(const char *filepath);
// cria o arquivo
FileData file_data_create(const char *filepath);

// Retorna a lista de linhas de um FileData
LinkedList getLinesQueue(FileData fileData);

// Libera a memória alocada para FileData
void destroyFileData(FileData fileData);

char *getFileName(FileData fileData);

#endif // MANIPULADOR_DE_ARQUIVO_H
