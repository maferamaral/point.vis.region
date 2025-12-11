#include "lista.h"
#include <stdlib.h>

typedef struct node_t
{
    void *data;
    struct node_t *next;
} Node;

typedef struct list_t
{
    Node *head;
    Node *tail;
    int size;
} *ListImpl;

static ListImpl as_impl(LinkedList list) { return (ListImpl)list; }

LinkedList list_create()
{
    ListImpl impl = (ListImpl)malloc(sizeof(*impl));
    if (impl == NULL)
    {
        return NULL;
    }
    impl->head = NULL;
    impl->tail = NULL;
    impl->size = 0;
    return (LinkedList)impl;
}

void list_insert_front(LinkedList list, void *value)
{
    ListImpl impl = as_impl(list);
    if (impl == NULL)
    {
        return;
    }

    Node *node = (Node *)malloc(sizeof(Node));
    if (node == NULL)
    {
        return;
    }

    node->data = value;
    node->next = impl->head;
    impl->head = node;

    if (impl->tail == NULL)
    {
        impl->tail = node;
    }
    impl->size++;
}

void list_insert_back(LinkedList list, void *value)
{
    ListImpl impl = as_impl(list);
    if (impl == NULL)
    {
        return;
    }

    Node *node = (Node *)malloc(sizeof(Node));
    if (node == NULL)
    {
        return;
    }

    node->data = value;
    node->next = NULL;

    if (impl->tail == NULL)
    {
        impl->head = node;
        impl->tail = node;
    }
    else
    {
        impl->tail->next = node;
        impl->tail = node;
    }
    impl->size++;
}

void *list_remove_front(LinkedList list)
{
    ListImpl impl = as_impl(list);
    if (impl == NULL || impl->head == NULL)
    {
        return NULL;
    }

    Node *temp = impl->head;
    void *value = temp->data;
    impl->head = temp->next;

    if (impl->head == NULL)
    {
        impl->tail = NULL;
    }
    impl->size--;
    free(temp);
    return value;
}

void *list_remove_back(LinkedList list)
{
    ListImpl impl = as_impl(list);
    if (impl == NULL || impl->tail == NULL)
    {
        return NULL;
    }

    void *value = impl->tail->data;

    if (impl->head == impl->tail)
    {
        free(impl->head);
        impl->head = NULL;
        impl->tail = NULL;
    }
    else
    {
        Node *curr = impl->head;
        while (curr->next != impl->tail)
        {
            curr = curr->next;
        }
        free(impl->tail);
        impl->tail = curr;
        curr->next = NULL;
    }
    impl->size--;
    return value;
}

void *list_front(LinkedList list)
{
    ListImpl impl = as_impl(list);
    if (impl == NULL || impl->head == NULL)
    {
        return NULL;
    }
    return impl->head->data;
}

void *list_back(LinkedList list)
{
    ListImpl impl = as_impl(list);
    if (impl == NULL || impl->tail == NULL)
    {
        return NULL;
    }
    return impl->tail->data;
}

int list_is_empty(LinkedList list)
{
    ListImpl impl = as_impl(list);
    if (impl == NULL)
    {
        return 1;
    }
    return impl->head == NULL;
}

int list_size(LinkedList list)
{
    ListImpl impl = as_impl(list);
    if (impl == NULL)
    {
        return 0;
    }
    return impl->size;
}

void *list_get_at(LinkedList list, int index)
{
    ListImpl impl = as_impl(list);
    if (impl == NULL)
    {
        return NULL;
    }

    if (index < 0 || index >= impl->size)
    {
        return NULL;
    }

    Node *curr = impl->head;
    for (int i = 0; i < index; i++)
    {
        curr = curr->next;
    }
    return curr->data;
}

void *list_remove_at(LinkedList list, int index)
{
    ListImpl impl = as_impl(list);
    if (impl == NULL || index < 0 || index >= impl->size)
    {
        return NULL;
    }

    if (index == 0)
    {
        return list_remove_front(list);
    }
    if (index == impl->size - 1)
    {
        return list_remove_back(list);
    }

    Node *prev = impl->head;
    for (int i = 0; i < index - 1; i++)
    {
        prev = prev->next;
    }

    Node *to_remove = prev->next;
    void *value = to_remove->data;
    prev->next = to_remove->next;

    free(to_remove);
    impl->size--;
    return value;
}

void list_destroy(LinkedList list)
{
    ListImpl impl = as_impl(list);
    if (impl == NULL)
    {
        return;
    }

    Node *curr = impl->head;
    while (curr != NULL)
    {
        Node *next = curr->next;
        free(curr);
        curr = next;
    }
    free(impl);
}
