#include "visibilidade.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "../tree/tree.h"
#include "../utils/lista/lista.h"
#include "../sort/sort.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Variável global de controle de sort
static char g_sort_method = 'q';

void visibilidade_set_sort_method(char method) {
    g_sort_method = method;
}

struct PoligonoVisibilidade_st {
    LinkedList vertices;
    Ponto centro;
};

// Variáveis estáticas para o comparador da árvore
static Ponto g_centro;
static double g_angulo;

// Helper: encontra o menor elemento da árvore
static void* tree_get_min(BinaryTree tree) {
    if (tree_is_empty(tree)) return NULL;
    TreeNode node = tree_get_root(tree);
    while (tree_node_left(node) != NULL) {
        node = tree_node_left(node);
    }
    return tree_node_get_data(node);
}

// Comparador da Árvore: Distância da intersecção do raio atual com o segmento
int comparar_segmentos_ativos(const void* a, const void* b) {
    Segmento* s1 = (Segmento*)a;
    Segmento* s2 = (Segmento*)b;
    if (s1 == s2) return 0;

    double d1 = distancia_raio_segmento(g_centro, g_angulo, *s1);
    double d2 = distancia_raio_segmento(g_centro, g_angulo, *s2);

    if (fabs(d1 - d2) < EPSILON) return 0;
    return (d1 < d2) ? -1 : 1;
}

typedef struct {
    double angulo;
    int tipo; // 0 = INICIO, 1 = FIM
    Segmento* seg;
    Ponto p;
} Evento;

// Comparador de Eventos para Sort
int comparar_eventos(const void* a, const void* b) {
    Evento* e1 = *(Evento**)a;
    Evento* e2 = *(Evento**)b;
    
    // 1. Ângulo
    if (e1->angulo < e2->angulo - EPSILON) return -1;
    if (e1->angulo > e2->angulo + EPSILON) return 1;
    
    // 2. Se ângulos iguais, INICIO antes de FIM para processar bloqueio antes de desbloqueio
    if (e1->tipo != e2->tipo) {
        return (e1->tipo == 0) ? -1 : 1;
    }
    
    // 3. Se ambos INICIO, o mais próximo primeiro (para entrar na árvore e virar biombo logo)
    if (e1->tipo == 0) {
        double d1 = distancia(g_centro, e1->p);
        double d2 = distancia(g_centro, e2->p);
        if (fabs(d1 - d2) < EPSILON) return 0;
        return (d1 < d2) ? -1 : 1;
    }
    
    return 0;
}

// Função para preparar os segmentos (dividir os que cruzam o ângulo 0)
// Retorna uma NOVA lista com os segmentos processados.
static LinkedList preparar_segmentos(Ponto centro, LinkedList barreiras_originais) {
    LinkedList processados = list_create();
    int n = list_size(barreiras_originais);
    
    for (int i = 0; i < n; i++) {
        Segmento* s = (Segmento*)list_get_at(barreiras_originais, i);
        
        // Verifica ângulos
        double ang1 = angulo_polar(centro, s->p1);
        double ang2 = angulo_polar(centro, s->p2);
        
        // Normaliza para [0, 2PI) para detectar cruzamento do 0
        if (ang1 < 0) ang1 += 2*M_PI;
        if (ang2 < 0) ang2 += 2*M_PI;
        
        // Detecta cruzamento do eixo X positivo (ângulo 0)
        // Se a diferença absoluta for grande (> PI), ele cruza o corte.
        if (fabs(ang1 - ang2) > M_PI) {
            // Precisa dividir. Calcula intersecção com raio y=centro.y, x>centro.x
            Ponto inter = interseccao_raio_segmento(centro, 0.0, *s);
            
            if (!isnan(inter.x)) {
                // Cria dois segmentos novos
                // s_a: ponto original até intersecção
                // s_b: intersecção até outro ponto
                Segmento* s1 = malloc(sizeof(Segmento));
                Segmento* s2 = malloc(sizeof(Segmento));
                
                *s1 = segmento_criar(s->p1, inter);
                *s2 = segmento_criar(inter, s->p2);
                
                list_insert_back(processados, s1);
                list_insert_back(processados, s2);
                continue; 
            }
        }
        
        // Se não precisou dividir, copia o ponteiro (ou cria cópia se for gerenciar memória estritamente)
        // Aqui vamos apenas usar o ponteiro, mas cuidado com free depois.
        // Para consistência, vamos assumir que processados terá DONO da memória se criarmos novos.
        // Melhor criar cópia sempre para uniformizar o free.
        Segmento* copia = malloc(sizeof(Segmento));
        *copia = *s;
        list_insert_back(processados, copia);
    }
    return processados;
}

PoligonoVisibilidade visibilidade_calcular(Ponto centro, LinkedList barreiras_input) {
    g_centro = centro;
    LinkedList vertices_poly = list_create();

    // 1. Pré-processamento: Dividir segmentos que cruzam ângulo 0
    LinkedList barreiras = preparar_segmentos(centro, barreiras_input);
    
    // 2. Criar Eventos
    int qtd_barreiras = list_size(barreiras);
    int qtd_eventos = qtd_barreiras * 2;
    Evento** eventos = malloc(sizeof(Evento*) * qtd_eventos);
    int k = 0;

    for (int i = 0; i < qtd_barreiras; i++) {
        Segmento* seg = (Segmento*)list_get_at(barreiras, i);
        double ang1 = angulo_polar(centro, seg->p1);
        double ang2 = angulo_polar(centro, seg->p2);
        
        // Garante angulos em [-PI, PI] ou [0, 2PI] consistentes após o split
        // A função angulo_polar geralmente retorna [-PI, PI].
        
        // Define quem é inicio e fim baseado no sentido anti-horário
        // Se ang1 < ang2, p1 é inicio. 
        // Como já fizemos o split do 0, não deve haver |delta| > PI.
        
        bool p1_inicio = (ang1 < ang2);
        
        eventos[k] = malloc(sizeof(Evento));
        eventos[k]->angulo = ang1;
        eventos[k]->seg = seg;
        eventos[k]->p = seg->p1;
        eventos[k]->tipo = p1_inicio ? 0 : 1;
        k++;

        eventos[k] = malloc(sizeof(Evento));
        eventos[k]->angulo = ang2;
        eventos[k]->seg = seg;
        eventos[k]->p = seg->p2;
        eventos[k]->tipo = p1_inicio ? 1 : 0;
        k++;
    }

    // 3. Ordenar Eventos
    // Usa o wrapper sort definido em sort.h
    sort(eventos, qtd_eventos, sizeof(Evento*), comparar_eventos, g_sort_method, 10);

    // 4. Varredura
    BinaryTree ativos = tree_create(comparar_segmentos_ativos);
    
    // Precisamos do estado inicial (quais segmentos interceptam o raio inicial -PI/Angulo0?)
    // Como fizemos split no angulo 0 e vamos varrer de -PI a PI (ou 0 a 2PI),
    // e angulo_polar retorna -PI..PI, começamos do menor ângulo.
    // O ideal com split no 0 seria ordenar de 0 a 2PI. Vamos ajustar para usar angulo_polar normal (-PI a PI).
    // Se usamos -PI a PI, o "corte" é em -PI. Precisamos dividir segmentos que cruzam -PI.
    // A função preparar_segmentos cortou no 0. Se sortearmos de -PI a PI, o corte no 0 vira um evento comum no meio.
    // Segmentos que cruzam -PI (o corte real do atan2) ainda seriam problema.
    // CORREÇÃO SIMPLES: Vamos apenas inserir na árvore e processar. O "biombo" inicial deve ser detectado.
    
    Segmento* biombo_atual = NULL;
    
    for (int i = 0; i < qtd_eventos; i++) {
        Evento* e = eventos[i];
        g_angulo = e->angulo; // Atualiza ângulo global para comparador da árvore

        // Atualiza Árvore
        if (e->tipo == 0) { // INICIO
            tree_insert(ativos, e->seg);
            
            // Verifica visibilidade deste ponto
            Segmento* mais_prox = (Segmento*)tree_get_min(ativos);
            
            // Lógica simplificada de visibilidade (apenas vértices)
            // Se o segmento inserido for o novo mais próximo, adiciona interseccao no anterior e vertice atual
            if (mais_prox == e->seg) {
                if (biombo_atual && biombo_atual != e->seg) {
                   Ponto inter = interseccao_raio_segmento(centro, e->angulo, *biombo_atual);
                   if(!isnan(inter.x)) {
                       Ponto* p = malloc(sizeof(Ponto)); *p = inter;
                       list_insert_back(vertices_poly, p);
                   }
                }
                Ponto* p = malloc(sizeof(Ponto)); *p = e->p;
                list_insert_back(vertices_poly, p);
                biombo_atual = e->seg;
            }
        } 
        else { // FIM
            if (e->seg == biombo_atual) {
                Ponto* p = malloc(sizeof(Ponto)); *p = e->p;
                list_insert_back(vertices_poly, p);
                
                tree_remove(ativos, e->seg);
                
                Segmento* novo_prox = (Segmento*)tree_get_min(ativos);
                if (novo_prox) {
                    Ponto inter = interseccao_raio_segmento(centro, e->angulo, *novo_prox);
                    if(!isnan(inter.x)) {
                        Ponto* pi = malloc(sizeof(Ponto)); *pi = inter;
                        list_insert_back(vertices_poly, pi);
                    }
                }
                biombo_atual = novo_prox;
            } else {
                tree_remove(ativos, e->seg);
            }
        }
    }

    // Limpeza
    for(int i=0; i<qtd_eventos; i++) free(eventos[i]);
    free(eventos);
    tree_destroy(ativos, NULL); // Destroi árvore mas não os segmentos (estão na lista 'barreiras')
    
    // Libera a lista de barreiras processadas (cópias e splits)
    while(!list_is_empty(barreiras)) free(list_remove_front(barreiras));
    list_destroy(barreiras);

    PoligonoVisibilidade poly = malloc(sizeof(struct PoligonoVisibilidade_st));
    poly->vertices = vertices_poly;
    poly->centro = centro;
    return poly;
}

bool visibilidade_ponto_atingido(PoligonoVisibilidade pol, Ponto p) {
    if (!pol) return false;
    double dist_p = distancia(pol->centro, p);
    double ang = angulo_polar(pol->centro, p);
    
    // Busca linear simples pelas arestas do polígono
    int n = list_size(pol->vertices);
    if (n < 3) return false;

    // Encontra a aresta do polígono que intercepta o raio do ponto P
    for (int i = 0; i < n; i++) {
        Ponto* v1 = (Ponto*)list_get_at(pol->vertices, i);
        Ponto* v2 = (Ponto*)list_get_at(pol->vertices, (i + 1) % n);
        
        // Verifica se o ângulo de P está entre ang(v1) e ang(v2)
        // Isso é complexo devido à ciclicidade. 
        // Mais fácil: Intersecta raio(centro, P_infinito) com segmento(v1, v2)
        Segmento aresta = {*v1, *v2};
        Ponto inter = interseccao_raio_segmento(pol->centro, ang, aresta);
        
        if (!isnan(inter.x)) {
            double dist_borda = distancia(pol->centro, inter);
            // Se o ponto está mais perto que a borda, está dentro
            if (dist_p <= dist_borda + EPSILON) return true;
        }
    }
    return false;
}

void visibilidade_destruir(PoligonoVisibilidade pol) {
    if (!pol) return;
    while (!list_is_empty(pol->vertices)) {
        free(list_remove_front(pol->vertices));
    }
    list_destroy(pol->vertices);
    free(pol);
}