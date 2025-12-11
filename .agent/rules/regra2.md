---
trigger: always_on
---

Arquivo 2: `02_Estruturas_e_Utilitarios.md`

Este arquivo instrui a criação das estruturas de dados genéricas e algoritmos de ordenação exigidos.

```markdown
# Instruções: Estruturas de Dados e Utilitários

## 1. Lista Encadeada Genérica (`Lista`)
Implementar uma lista duplamente encadeada que armazene `void*`.
- Funções: `insert`, `remove`, `get`, `iteradores`, `map`.

## 2. Árvore Binária de Busca (`Arvore`)
**Essencial para o Algoritmo de Visibilidade.**
- Deve armazenar `Segmentos` ativos durante a varredura.
- **Requisito Especial**: A função de comparação da árvore deve ser dinâmica ou aceitar um contexto, pois a ordem dos segmentos muda dependendo do ângulo da linha de varredura.
- Operações: `insert`, `delete`, `min`, `max`, `predecessor`, `sucessor`.

## 3. Módulo de Ordenação (`Sort`)
O projeto exige a implementação de múltiplos algoritmos de ordenação para vetores/listas genéricas.
- **Interface**: Deve aceitar um array/lista e um ponteiro de função comparadora `int (*cmp)(const void*, const void*)`.
- **Algoritmos**:
  1. `quicksort`: Pode usar o `qsort` da `stdlib.h` ou implementar o próprio.
  2. `mergesort`: Implementação obrigatória.
  3. `insertionsort` (opcional/fallback): Implementação obrigatória se solicitada.
- O argumento `-to` define qual usar.

## 4. Fila/Pilha
Se necessário para processamento auxiliar.