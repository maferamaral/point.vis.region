---
trigger: always_on
---

# Instruções: Entrada, Saída e Regras do Jogo

## 1. Leitura GEO (`geo_handler`)
- Ler arquivo `.geo`.
- Comandos:
  - `c`: Círculo (id, x, y, r, cor_borda, cor_preenchimento).
  - `r`: Retângulo (id, x, y, w, h, ...).
  - `t`: Texto.
  - `l`: Linha.
- Armazenar tudo na estrutura `Cidade`.

## 2. Leitura QRY (`qry_handler`)
- Ler arquivo `.qry`.
- Comandos de Bomba:
  - `b? x y`: **Clonar**. Se atingido, cria uma cópia da forma (verificar regras de deslocamento no PDF).
  - `b/ x y`: **Destruição**. Se atingido, a forma é removida da memória e da lista.
  - `b# x y cor`: **Tinta**. Se atingido, altera a cor da forma.

## 3. Processamento de uma Consulta
Para cada comando de bomba:
1. Obter todas as formas atuais que funcionam como barreira.
2. Converter essas formas em uma lista de `Segmentos`.
3. Invocar `Visibilidade(bomba, segmentos)` -> Retorna `Poligono`.
4. Percorrer todas as formas da lista principal:
   - Verificar `isInside(Poligono, forma_ancora)`.
   - Se VERDADEIRO: Aplicar efeito (Clonar/Destruir/Pintar).
5. Gerar report no `.txt` (ex: "Bomba em (x,y) atingiu forma ID 5").

## 4. Geração de SVG
- Criar funções para desenhar:
  - Formas básicas (rect, circle, line, text).
  - O Polígono de Visibilidade (geralmente com opacidade reduzida para debug ou visualização do efeito da bomba).
- O arquivo SVG deve refletir o estado da cidade *após* todas as consultas.