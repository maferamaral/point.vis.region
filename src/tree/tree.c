#include "tree.h"
#include <string.h>

// Estrutura interna do nó
typedef struct tree_node_t
{
    void *data;
    struct tree_node_t *left;
    struct tree_node_t *right;
    int height;
} TreeNodeImpl;

// Estrutura interna da árvore
typedef struct tree_t
{
    TreeNodeImpl *root;
    CompareFunc compare;
    int size;
} *TreeImpl;

// ==================== Funções auxiliares ====================

static TreeNodeImpl *node_create(void *data)
{
    TreeNodeImpl *node = (TreeNodeImpl *)malloc(sizeof(TreeNodeImpl));
    if (node == NULL)
        return NULL;

    node->data = data;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

static int node_get_height(TreeNodeImpl *node)
{
    if (node == NULL)
        return 0;
    return node->height;
}

static void node_update_height(TreeNodeImpl *node)
{
    if (node == NULL)
        return;

    int left_height = node_get_height(node->left);
    int right_height = node_get_height(node->right);
    node->height = (left_height > right_height ? left_height : right_height) + 1;
}

static TreeNodeImpl *node_insert(TreeImpl impl, TreeNodeImpl *node, void *value)
{
    if (node == NULL)
    {
        impl->size++;
        return node_create(value);
    }

    int cmp = impl->compare(value, node->data);

    if (cmp < 0)
    {
        node->left = node_insert(impl, node->left, value);
    }
    else if (cmp > 0)
    {
        node->right = node_insert(impl, node->right, value);
    }
    else
    {
        // Valor duplicado, não insere
        return node;
    }

    node_update_height(node);
    return node;
}

static TreeNodeImpl *node_search(TreeNodeImpl *node, const void *value, CompareFunc compare)
{
    if (node == NULL)
        return NULL;

    int cmp = compare(value, node->data);

    if (cmp == 0)
        return node;
    else if (cmp < 0)
        return node_search(node->left, value, compare);
    else
        return node_search(node->right, value, compare);
}

static TreeNodeImpl *node_find_min(TreeNodeImpl *node)
{
    if (node == NULL)
        return NULL;

    while (node->left != NULL)
        node = node->left;

    return node;
}

static TreeNodeImpl *node_remove(TreeImpl impl, TreeNodeImpl *node, const void *value, int *removed)
{
    if (node == NULL)
        return NULL;

    int cmp = impl->compare(value, node->data);

    if (cmp < 0)
    {
        node->left = node_remove(impl, node->left, value, removed);
    }
    else if (cmp > 0)
    {
        node->right = node_remove(impl, node->right, value, removed);
    }
    else
    {
        // Nó encontrado
        *removed = 1;
        impl->size--;

        // Caso 1: Nó sem filhos
        if (node->left == NULL && node->right == NULL)
        {
            free(node);
            return NULL;
        }

        // Caso 2: Nó com apenas um filho
        if (node->left == NULL)
        {
            TreeNodeImpl *temp = node->right;
            free(node);
            return temp;
        }

        if (node->right == NULL)
        {
            TreeNodeImpl *temp = node->left;
            free(node);
            return temp;
        }

        // Caso 3: Nó com dois filhos
        TreeNodeImpl *min_right = node_find_min(node->right);
        node->data = min_right->data;
        node->right = node_remove(impl, node->right, min_right->data, removed);
        *removed = 1; // Reajusta porque a remoção recursiva já foi contada
    }

    node_update_height(node);
    return node;
}

static int node_tree_size(TreeNodeImpl *node)
{
    if (node == NULL)
        return 0;
    return 1 + node_tree_size(node->left) + node_tree_size(node->right);
}

static int node_tree_height(TreeNodeImpl *node)
{
    if (node == NULL)
        return 0;

    int left_height = node_tree_height(node->left);
    int right_height = node_tree_height(node->right);
    return (left_height > right_height ? left_height : right_height) + 1;
}

static void node_inorder_walk(TreeNodeImpl *node, VisitFunc visit)
{
    if (node == NULL)
        return;

    node_inorder_walk(node->left, visit);
    visit(node->data);
    node_inorder_walk(node->right, visit);
}

static void node_preorder_walk(TreeNodeImpl *node, VisitFunc visit)
{
    if (node == NULL)
        return;

    visit(node->data);
    node_preorder_walk(node->left, visit);
    node_preorder_walk(node->right, visit);
}

static void node_postorder_walk(TreeNodeImpl *node, VisitFunc visit)
{
    if (node == NULL)
        return;

    node_postorder_walk(node->left, visit);
    node_postorder_walk(node->right, visit);
    visit(node->data);
}

static void node_destroy(TreeNodeImpl *node, FreeFunc free_func)
{
    if (node == NULL)
        return;

    node_destroy(node->left, free_func);
    node_destroy(node->right, free_func);

    if (free_func != NULL)
        free_func(node->data);

    free(node);
}

// ==================== API Pública ====================

BinaryTree tree_create(CompareFunc compare)
{
    TreeImpl impl = (TreeImpl)malloc(sizeof(struct tree_t));
    if (impl == NULL)
        return NULL;

    impl->root = NULL;
    impl->compare = compare;
    impl->size = 0;

    return (BinaryTree)impl;
}

void tree_insert(BinaryTree tree, void *value)
{
    TreeImpl impl = (TreeImpl)tree;
    if (impl == NULL)
        return;

    impl->root = node_insert(impl, impl->root, value);
}

TreeNode tree_search(BinaryTree tree, const void *value)
{
    TreeImpl impl = (TreeImpl)tree;
    if (impl == NULL)
        return NULL;

    return (TreeNode)node_search(impl->root, value, impl->compare);
}

int tree_remove(BinaryTree tree, const void *value)
{
    TreeImpl impl = (TreeImpl)tree;
    if (impl == NULL)
        return 0;

    int removed = 0;
    impl->root = node_remove(impl, impl->root, value, &removed);
    return removed;
}

void *tree_node_get_data(TreeNode node)
{
    if (node == NULL)
        return NULL;

    TreeNodeImpl *impl = (TreeNodeImpl *)node;
    return impl->data;
}

TreeNode tree_get_root(BinaryTree tree)
{
    TreeImpl impl = (TreeImpl)tree;
    if (impl == NULL)
        return NULL;

    return (TreeNode)impl->root;
}

TreeNode tree_node_left(TreeNode node)
{
    if (node == NULL)
        return NULL;

    TreeNodeImpl *impl = (TreeNodeImpl *)node;
    return (TreeNode)impl->left;
}

TreeNode tree_node_right(TreeNode node)
{
    if (node == NULL)
        return NULL;

    TreeNodeImpl *impl = (TreeNodeImpl *)node;
    return (TreeNode)impl->right;
}

int tree_is_empty(BinaryTree tree)
{
    TreeImpl impl = (TreeImpl)tree;
    if (impl == NULL)
        return 1;

    return impl->root == NULL;
}

int tree_size(BinaryTree tree)
{
    TreeImpl impl = (TreeImpl)tree;
    if (impl == NULL)
        return 0;

    return impl->size;
}

int tree_height(BinaryTree tree)
{
    TreeImpl impl = (TreeImpl)tree;
    if (impl == NULL)
        return 0;

    return node_tree_height(impl->root);
}

void tree_inorder_walk(BinaryTree tree, VisitFunc visit)
{
    TreeImpl impl = (TreeImpl)tree;
    if (impl == NULL || visit == NULL)
        return;

    node_inorder_walk(impl->root, visit);
}

void tree_preorder_walk(BinaryTree tree, VisitFunc visit)
{
    TreeImpl impl = (TreeImpl)tree;
    if (impl == NULL || visit == NULL)
        return;

    node_preorder_walk(impl->root, visit);
}

void tree_postorder_walk(BinaryTree tree, VisitFunc visit)
{
    TreeImpl impl = (TreeImpl)tree;
    if (impl == NULL || visit == NULL)
        return;

    node_postorder_walk(impl->root, visit);
}

void tree_destroy(BinaryTree tree, FreeFunc free_func)
{
    TreeImpl impl = (TreeImpl)tree;
    if (impl == NULL)
        return;

    node_destroy(impl->root, free_func);
    free(impl);
}
