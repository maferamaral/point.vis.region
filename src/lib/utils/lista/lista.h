#ifndef LISTA_H
#define LISTA_H

// Tipo opaco da lista encadeada
typedef void *LinkedList;

// Cria uma lista vazia
LinkedList list_create();

// Insere um elemento no início da lista
void list_insert_front(LinkedList list, void *value);

// Insere um elemento no final da lista
void list_insert_back(LinkedList list, void *value);

// Remove e retorna o primeiro elemento
void *list_remove_front(LinkedList list);

// Remove e retorna o último elemento
void *list_remove_back(LinkedList list);

// Retorna o primeiro elemento sem remover
void *list_front(LinkedList list);

// Retorna o último elemento sem remover
void *list_back(LinkedList list);

// Retorna 1 se vazia, 0 caso contrário
int list_is_empty(LinkedList list);

// Retorna o tamanho da lista
int list_size(LinkedList list);

// Destroi a lista liberando nós internos (não libera os dados apontados)
void list_destroy(LinkedList list);

// Acesso by index (0 = primeiro elemento)
void *list_get_at(LinkedList list, int index);

#endif // LISTA_H
