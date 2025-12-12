#ifndef TREE_H
#define TREE_H

#include <stdlib.h>

// Tipo opaco para árvore binária
typedef void *BinaryTree;

// Tipo opaco para nó da árvore
typedef void *TreeNode;

// Callbacks para operações customizáveis
typedef int (*CompareFunc)(const void *a, const void *b);
typedef void (*FreeFunc)(void *data);
typedef void (*VisitFunc)(void *data);

// Cria uma nova árvore binária vazia
BinaryTree tree_create(CompareFunc compare);

// Insere um valor na árvore
void tree_insert(BinaryTree tree, void *value);

// Busca um valor na árvore (retorna o nó ou NULL)
TreeNode tree_search(BinaryTree tree, const void *value);

// Remove um valor da árvore
int tree_remove(BinaryTree tree, const void *value);

// Retorna o valor armazenado em um nó
void *tree_node_get_data(TreeNode node);

// Retorna o nó raiz da árvore
TreeNode tree_get_root(BinaryTree tree);

// Retorna o nó filho esquerdo
TreeNode tree_node_left(TreeNode node);

// Retorna o nó filho direito
TreeNode tree_node_right(TreeNode node);

// Verifica se a árvore está vazia
int tree_is_empty(BinaryTree tree);

// Retorna o tamanho (número de nós) da árvore
int tree_size(BinaryTree tree);

// Retorna a altura da árvore
int tree_height(BinaryTree tree);

// Percurso em ordem (inorder)
void tree_inorder_walk(BinaryTree tree, VisitFunc visit);

// Percurso pré-ordem (preorder)
void tree_preorder_walk(BinaryTree tree, VisitFunc visit);

// Percurso pós-ordem (postorder)
void tree_postorder_walk(BinaryTree tree, VisitFunc visit);

// Destrói a árvore liberando todos os nós
void tree_destroy(BinaryTree tree, FreeFunc free_func);

// Retorna o menor elemento da árvore
void *tree_get_min(BinaryTree tree);

// Retorna o maior elemento da árvore
void *tree_get_max(BinaryTree tree);

// Busca o predecessor (maior elemento menor que value, ou value se existir)
void *tree_find_predecessor(BinaryTree tree, const void *value);

// Busca o sucessor (menor elemento maior que value, ou value se existir)
void *tree_find_successor(BinaryTree tree, const void *value);

#endif // TREE_H
