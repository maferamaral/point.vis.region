---
trigger: always_on
---

# Instruções do Projeto: Bocha Geométrica (Visibilidade) - Visão Geral

## Objetivo
Desenvolver um sistema em C para simular explosões em um ambiente 2D repleto de formas geométricas. O núcleo do projeto é o cálculo da **Região de Visibilidade** usando um algoritmo de varredura angular (Sweep Line) para determinar quais objetos são atingidos por bombas, considerando que algumas formas atuam como anteparos (barreiras).

## Estrutura de Diretórios Sugerida
O projeto deve seguir estritamente o uso de TADs (Tipos Abstratos de Dados) opacos.
```text
src/
├── main.c              // Orquestrador principal (leitura de args e loop principal)
├── lib/
│   ├── geo/            // Leitura e armazenamento das formas (.geo)
│   ├── qry/            // Processamento das bombas e consultas (.qry)
│   ├── svg/            // Geração de saída gráfica
│   ├── geometria/      // Matemática: Ponto, Segmento, Vetor, Intersecções
│   ├── poligono/       // TAD Polígono (Região de Visibilidade)
│   ├── visibilidade/   // Algoritmo de Varredura Angular
│   └── utils/          // Estruturas genéricas: Lista, Fila, Árvore Binária, Ordenação
└── include/            // Headers públicos
