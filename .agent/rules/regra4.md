---
trigger: always_on
---

### Arquivo 4: `04_Algoritmo_Visibilidade.md`

Este é o núcleo complexo do trabalho, baseado nos slides `regvis-slides.pdf` e `point-vis-region.pdf`.

```markdown
# Instruções: Algoritmo de Visibilidade (Varredura Angular)

## Conceito
Dado um ponto de origem (Bomba) e um conjunto de segmentos (Barreiras), determinar o polígono que representa a área iluminada.

## TAD `Visibilidade`

### Entrada
- Ponto `O` (Origem/Bomba).
- Lista de `Segmentos` (Barreiras). **Nota:** Formas como retângulos devem ser decompostas em 4 segmentos.

### Estruturas Auxiliares
- **Eventos**: Cada extremidade de um segmento gera um evento.
  - Tipo: `INICIO` ou `FIM`.
  - Atributo: Ângulo polar em relação a `O`.
  - Ordenação: Os eventos devem ser ordenados angularmente (usando o módulo de Ordenação criado).

### O Algoritmo (Sweep Line)
1. **Pré-processamento**:
   - Decompor todas as formas "Barreira" em segmentos.
   - Criar lista de Eventos (pontos finais dos segmentos).
   - Ordenar eventos por ângulo.

2. **Varredura**:
   - Inicializar a **Árvore de Segmentos Ativos** (BST).
   - A árvore deve ordenar os segmentos pela distância da intersecção com o "raio de varredura" atual.
   - Percorrer a lista de eventos ordenados:
     - Se o evento é `INICIO` de um segmento: Inserir segmento na Árvore.
     - Se o evento é `FIM` de um segmento: Remover segmento da Árvore.
     - Para cada evento, o segmento que estiver na "raiz" (ou mínimo) da árvore é o segmento visível mais próximo.
     - Calcular a intersecção do raio atual com este segmento mais próximo.
     - Adicionar este ponto de intersecção ao `Poligono` de visibilidade.

### Tratamento de Casos Especiais (Conforme Slides)
- Segmentos colineares com o raio de visão.
- Vértices que são início e fim de múltiplos segmentos simultaneamente.
- Uso de "Sentinelas" ou "Biombos" (bounding box) para garantir que o raio sempre bata em algo.

### Saída
- Um objeto `Poligono` contendo a sequência de vértices que formam a região iluminada.
