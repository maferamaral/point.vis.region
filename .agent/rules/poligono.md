---
trigger: always_on
---

# Especificação Técnica: TAD Polígono

## 1. Definição e Propósito
O TAD `PoligonoVisibilidade` é uma estrutura opaca responsável por armazenar a sequência ordenada de vértices que delimitam a região iluminada/visível calculada pelo algoritmo. Ele serve como o "contêiner de resposta" da função `visibilidade_calcular`.

## 2. Requisitos de Implementação

### 2.1. Ocultamento de Informação (Encapsulamento)
* [cite_start]**Regra de Ouro:** É terminantemente proibido definir a `struct` no arquivo de cabeçalho (`.h`)[cite: 636].
* **No arquivo `.h`:** Apenas o `typedef` do ponteiro:
    ```c
    typedef void* PoligonoVisibilidade;
    ```
* [cite_start]**No arquivo `.c`:** A definição completa da estrutura[cite: 637]:
    ```c
    typedef struct {
        LinkedList vertices; // Lista encadeada de (Ponto*)
        Ponto centro;        // Fonte de luz/observador (opcional, mas útil)
    } PoligonoVisibilidadeImpl;
    ```

### 2.2. Gerenciamento de Memória
Como o algoritmo de visibilidade gera novos pontos de intersecção dinamicamente (usando `malloc`), o TAD Polígono assume a "propriedade" (ownership) desses dados.
* **Criação:** Aloca a estrutura principal e inicializa a lista interna.
* **Destruição:** Deve ser profunda (Deep Free).
    * Iterar sobre a lista de vértices.
    * Liberar (`free`) cada `Ponto*` armazenado.
    * Destruir a lista.
    * Liberar a estrutura do polígono.

### 2.3. Interação com Estruturas Auxiliares
* [cite_start]**Listas:** O projeto exige o uso de listas para armazenar as formas[cite: 640]. Portanto, o campo `vertices` deve ser do tipo `LinkedList` (ou similar) da sua biblioteca `utils`.
* **Pontos:** Os vértices armazenados são coordenadas `double (x, y)`.

---

## 3. Interface Pública (API Sugerida)

As funções abaixo devem ser expostas no `.h` para uso pelo módulo `visibilidade.c` e pelo desenhador SVG.

### Construtor e Destrutor
* `PoligonoVisibilidade poligono_criar(Ponto centro);`
    * Inicializa a lista vazia.
* `void poligono_destruir(PoligonoVisibilidade p);`
    * Libera toda a memória associada.

### Manipulação
* `void poligono_adicionar_vertice(PoligonoVisibilidade p, Ponto pt);`
    * Recebe um ponto (por valor ou referência), aloca uma cópia dinâmica dele e insere no final da lista. Isso garante que o Polígono tenha sua própria cópia dos dados.

### Acesso (Getters)
* `LinkedList poligono_get_vertices(PoligonoVisibilidade p);`
    * Retorna a lista para iteração (necessário para gerar o SVG e para verificações de atingimento).
* `Ponto poligono_get_centro(PoligonoVisibilidade p);`
    * Retorna o centro (fonte de luz).

---

## 4. Uso no Fluxo do Projeto

1.  **Entrada:** O módulo `visibilidade` cria uma instância vazia: `p = poligono_criar(centro)`.
2.  **Processamento:** Durante a varredura, sempre que o `biombo` muda ou um evento de vértice ocorre, um novo ponto é calculado e adicionado: `poligono_adicionar_vertice(p, ponto_intersec)`.
3.  [cite_start]**Verificação (Queries):** Para comandos como `d` (destruição) ou `P` (pintura), o programa recupera a lista de vértices via `poligono_get_vertices` e verifica se as formas alvo interceptam este polígono (Bounding Box check -> Point in Polygon check) [cite: 621-633].
4.  **Saída:** O módulo SVG percorre a lista de vértices para desenhar o `<polygon points="..." />`.