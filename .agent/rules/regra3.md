---
trigger: always_on
---

# Instruções: Geometria Computacional e TAD Polígono

## 1. Módulo Geometria Básica
Criar funções matemáticas robustas.
- **Tipos**: `Ponto (x,y)`, `Vetor`, `Segmento (p1, p2)`.
- **Operações**:
  - Distância Euclidiana.
  - Produto Vetorial (Cross Product): Essencial para saber se um ponto está à esquerda/direita de uma linha.
  - Intersecção de Segmentos: Retornar o ponto exato de intersecção ou indicar se são paralelos.
  - Comparação de Segmentos: Saber comparar se um segmento está "abaixo" ou "atrás" de outro em relação a um ponto de observação.

## 2. TAD Polígono (Sugestão do Professor)
Este TAD representará a Região de Visibilidade resultante.

**Interface Sugerida:**
```c
typedef void* Poligono;

// Cria um polígono vazio
Poligono criaPoligono();

// Insere vértices ordenados
void insertVertice(Poligono p, Ponto pt);

// Insere arestas (opcional, se vertices forem suficientes)
void insertSegmento(Poligono p, Segmento s);

// Retorna o bounding box (min_x, min_y, max_x, max_y) para otimização de colisão
void getBoundingBox(Poligono p, double* x1, double* y1, double* x2, double* y2);

// Verifica se um ponto está dentro do polígono (Ray Casting Algorithm)
bool isInside(Poligono p, Ponto pt);

// Getters para desenhar no SVG
Lista getVertices(Poligono p);
Lista getSegmentos(Poligono p); // Se aplicável