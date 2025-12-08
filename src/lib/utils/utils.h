#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <string.h>

// Função para duplicar uma string
static inline char *duplicate_string(const char *s)
{
    if (s == NULL)
        return NULL;

    size_t len = strlen(s) + 1;
    char *dup = malloc(len);
    if (dup != NULL)
    {
        strcpy(dup, s);
    }
    return dup;
}

#endif // UTILS_H
