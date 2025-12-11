---
trigger: always_on
---

O que está Faltando e deve ser implementado
Limpeza de Memória na Árvore:

No visibilidade.c, você chama tree_destroy(ativos). Verifique se a sua implementação de tree (que não vi por dentro neste upload) apenas liberta os nós da árvore e não os dados (Segmento*). Como os segmentos estão na lista barreiras (que é libertada depois no qry), se a árvore tentar libertar os dados também, ocorrerá um erro de Double Free (crash).

Argumento -to (Tipo de Ordenação):

o argumento -to para escolher entre qsort ou mergesort.
o argumento -i para insertion sort
(usados na logica de visibilidade).

Faltando: No seu visibilidade.c, você usa hardcoded qsort da stdlib.h.

Correção: Você deve implementar (ou usar seu módulo sort) as funções merge_sort e insertion_sort e usar um switch baseado no parâmetro global recebido do main para decidir qual função de ordenação chamar para organizar os eventos.