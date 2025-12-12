---
trigger: manual
---

# Refatoração Crítica: Polígono, Visibilidade e Cálculos

Este documento detalha as alterações necessárias para adequar a implementação atual aos requisitos de arquitetura (TADs) e regras de negócio (Testes de Sobreposição da Figura 3 do PDF).

## 1. Criação do Módulo Polígono (Novo TAD)
Atualmente, o polígono é uma struct interna em `visibilidade.c`. Deve-se promover isso para um módulo independente.

* **Arquivos:** `lib/geometria/poligono/poligono.h` e `.c`
* **Regras:** Ponteiro opaco (`typedef void* Poligono;` no .h).

### Funcionalidades Exigidas no TAD
1.  **Construtor/Destrutor:** `poligono_criar()` e `poligono_destruir()`.
2.  **Manipulação:** `poligono_inserir_vertice(Poligono p, double x, double y)`.
3.  **Serialização (Adapter):** Como o módulo `calculos.c` opera com vetores de `double`, o Polígono deve oferecer uma função para exportar seus dados:
    * `int poligono_obter_vertices_array(Poligono p, double **vertices_out);`
    * *Aloca e preenche um vetor [x0, y0, x1, y1...] e retorna o número de vértices.*

## 2. Refatoração de `visibilidade.c`
* **Remover:** A definição de `struct poligono_internal`.
* **Atualizar:** Alterar `calcular_visibilidade` para utilizar as funções do novo TAD `Poligono` (criar, inserir vértices) em vez de manipular listas diretamente.
* **Interface:** A função deve retornar o tipo opaco `Poligono` definido no novo header.

---

## 3. Correção de `calculos.c` (Requisito Crítico: Figura 3)
A função atual `forma_no_poligono` está **incompleta**. Ela verifica apenas se a forma está *dentro* do polígono, mas falha se o polígono estiver *dentro* da forma ou se houver apenas intersecção de bordas.

Deve-se implementar os 3 casos de teste exigidos pelo PDF para cada forma:

### A. Para Retângulos
Implementar lógica `retangulo_intersepta_poligono(...)` que retorne `true` se:
1.  **Vértice do Retângulo no Polígono:** (Já implementado).
2.  **Vértice do Polígono no Retângulo:** Iterar sobre todos os vértices do polígono e testar se `min_x <= vx <= max_x` e `min_y <= vy <= max_y`.
3.  **Intersecção de Arestas:** Verificar se alguma aresta do retângulo cruza alguma aresta do polígono (segmento x segmento).

### B. Para Círculos
Implementar lógica `circulo_intersepta_poligono(...)` que retorne `true` se:
1.  **Centro do Círculo no Polígono:** (Já implementado).
2.  **Vértice do Polígono no Círculo:** Iterar sobre vértices do polígono e testar se `dist(vertice, centro) <= raio`.
3.  **Aresta do Polígono próxima ao Centro:** Verificar se a distância da reta de alguma aresta do polígono até o centro do círculo é `<= raio` (e se a projeção cai dentro do segmento da aresta).

### C. Para Linhas (Segmentos)
1.  **Extremos no Polígono:** (Já implementado).
2.  **Intersecção:** Verificar se o segmento da linha cruza qualquer aresta do polígono.

---

## 4. Integração do Fluxo (Adapter)
Para fazer a "ponte" entre o objeto `Poligono` retornado pela visibilidade e as funções matemáticas de `calculos.c`:

1.  O módulo que gerencia as Bombas (ex: `main` ou `processar_consultas`) chama `calcular_visibilidade`.
2.  Recebe um objeto `Poligono`.
3.  Chama `poligono_obter_vertices_array` para obter um vetor `double[]`.
4.  Passa esse vetor para a função corrigida `forma_no_poligono` em `calculos.c`.
5.  Libera o vetor temporário e destrói o polígono.

---

## Resumo das Tarefas para o Assistente
1.  Criar `lib/geometria/poligono/`.
2.  Mover lógica de armazenamento de vértices de `visibilidade.c` para lá.
3.  Em `calculos.c`, expandir `forma_no_poligono` para incluir os testes inversos (polígono dentro da forma) e testes de aresta.