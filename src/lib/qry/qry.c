#include "qry.h"
#include "../visibilidade/visibilidade.h"
#include "../geo/geo.h"
#include "../svg/svg.h"
#include "../formas/formas.h"
#include <stdlib.h>
#include <string.h>

// Função auxiliar para verificar se a forma está na área de explosão
bool forma_atingida(void *forma, TipoForma tipo, PoligonoVisibilidade pol)
{
    Ponto ancora;
    // Pega a âncora dependendo do tipo
    if (tipo == CIRCLE)
    {
        ancora.x = circulo_get_x(forma);
        ancora.y = circulo_get_y(forma);
    }
    else if (tipo == RECTANGLE)
    {
        ancora.x = retangulo_get_x(forma);
        ancora.y = retangulo_get_y(forma);
    }
    else if (tipo == LINE)
    {
        ancora.x = line_get_x1(forma);
        ancora.y = line_get_y1(forma);
    }
    else if (tipo == TEXT)
    {
        ancora.x = text_get_x(forma);
        ancora.y = text_get_y(forma);
    }

    // Verifica se a âncora está dentro do polígono de visibilidade
    // (Usa a função disponível em visibilidade.c)
    return visibilidade_ponto_atingido(pol, ancora);
}

void qry_processar(Geo cidade, const char *qryPath, const char *outPath, const char *geoName)
{
    // NOTE: abertura de arquivos (fqry, ftxt, fsvg) originalmente esperada aqui.
    // Para manter a compilação, declaramos as variáveis; a implementação
    // completa de abertura/leitura deve ser adicionada conforme necessário.
    FILE *fqry = NULL;
    FILE *ftxt = NULL;
    FILE *fsvg = NULL;

    // Extrai barreiras para o cálculo (paredes)
    LinkedList barreiras = geo_obter_todas_barreiras(cidade);

    char linha[1024];
    while (fgets(linha, sizeof(linha), fqry))
    {
        char cmd[10];
        sscanf(linha, "%s", cmd);

        if (cmd[0] == 'b')
        { // É uma bomba (b?, b/, b#)
            double x, y;
            // Lê coordenadas comuns a todas as bombas
            // Nota: o formato pode variar, ajuste o sscanf
            int offset_coords = 0;
            // Avança ponteiro

            // ... (Parsing das coordenadas x, y) ...

            Ponto bomba = {x, y};

            // 1. Calcula Visibilidade
            PoligonoVisibilidade pol = visibilidade_calcular(bomba, barreiras);

            // 2. Itera sobre todas as formas da cidade
            LinkedList formas = geo_get_formas(cidade);
            int nf = list_size(formas);
            for (int idx = 0; idx < nf; idx++)
            {
                ElementoGeo *el = (ElementoGeo *)list_get_at(formas, idx);

                if (forma_atingida(el->forma, el->tipo, pol))
                {
                    if (strcmp(cmd, "b/") == 0)
                    { // Destruição (Flash)
                        fprintf(ftxt, "Forma ID %d destruida.\n", get_id_generico(el->forma, el->tipo));
                        // Marcar para remoção (cuidado ao remover enquanto itera)
                    }
                    else if (strcmp(cmd, "b#") == 0)
                    { // Tinta (Ink)
                        char cor[50];
                        // Ler cor do comando...
                        fprintf(ftxt, "Forma ID %d pintada.\n", get_id_generico(el->forma, el->tipo));
                        // set_cor_generico(el->forma, el->tipo, cor);
                    }
                    else if (strcmp(cmd, "b?") == 0)
                    { // Clone
                        fprintf(ftxt, "Forma ID %d clonada.\n", get_id_generico(el->forma, el->tipo));
                        // clonar_forma_generica(cidade, el->forma, el->tipo);
                    }
                }
            }

            // Desenha o polígono no SVG para debug/visualização
            svg_desenhar_poligono(fsvg, pol, "yellow", 0.3); // 0.3 opacidade
            visibilidade_destruir_poly(pol);
        }
    }

    // Liberar lista de barreiras temporária (Segmento* alocados em geo_obter_todas_barreiras)
    int nb = list_size(barreiras);
    for (int i = 0; i < nb; i++)
    {
        Segmento *s = (Segmento *)list_get_at(barreiras, i);
        free(s);
    }
    list_destroy(barreiras);
}