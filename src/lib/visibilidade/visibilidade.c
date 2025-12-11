#include "visibilidade.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <float.h> // For DBL_MAX
//#include "../tree/tree.h"
#include "../utils/lista/lista.h"
#include "../geometria/geometria.h"
#include "../utils/sort/sort.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Global Sort Params
static char g_sort_type = 'q';
static int g_sort_threshold = 10;

void visibilidade_set_sort_params(char type, int threshold) {
    g_sort_type = type;
    g_sort_threshold = threshold;
}

// --- Constants & Globals --- //

// To avoid passing context everywhere to the tree comparator
static Ponto g_centro;
// Current sweep ray angle or a reference angle for BST ordering
static double g_angulo_atual; 

#define TIPO_INICIO 0
#define TIPO_FIM    1

// --- Internal Structures --- //

typedef struct {
    double angulo;
    double dist;
    int tipo; // TIPO_INICIO or TIPO_FIM
    Segmento *seg;
    Ponto p; // The actual vertex point
} Evento;

// --- Helper Functions: Orientation & Math --- //

// Returns true if ang1 < ang2, normalizing logic if needed
// Though we rely on qsort of events, BST comparison needs dynamic angle logic?
// No, BST comparison compares segments at 'g_angulo_atual'.

// --- event comparator for qsort --- //
int comparar_eventos(const void *a, const void *b) {
    Evento *e1 = *(Evento **)a;
    Evento *e2 = *(Evento **)b;

    // 1. By Angle
    if (fabs(e1->angulo - e2->angulo) > EPSILON) {
        return (e1->angulo < e2->angulo) ? -1 : 1;
    }

    // 2. Tie-breaker: Process START before END (to avoid gaps)
    if (e1->tipo != e2->tipo) {
        return (e1->tipo == TIPO_INICIO) ? -1 : 1;
    }

    // 3. Tie-breaker: Sort by distance (closer first if both are START)
    if (fabs(e1->dist - e2->dist) > EPSILON) {
        return (e1->dist < e2->dist) ? -1 : 1;
    }

    return 0;
}


// Comparators are for sort and tree. Since we removed Tree, we only need Event comparator.

// --- Helper: Clone/Prepare Segments --- //
// Copies input segments to a local list, adds BBox, and splits at Angle 0.
static LinkedList preparar_segmentos(Ponto centro, LinkedList barreiras) {
    LinkedList local = list_create();
    
    // 1. Copy original segments
    int n = list_size(barreiras);
    for(int i=0; i<n; i++) {
        Segmento *orig = (Segmento*)list_get_at(barreiras, i);
        Segmento *copy = malloc(sizeof(Segmento));
        *copy = *orig;
        list_insert_back(local, copy);
    }

    // 2. Add Bounding Box (Large coords)
    // Assuming world is somewhat bounded, typically 0-1000? 
    // We add a safe margin. -10000 to 10000.
    double min = -10000, max = 20000;
    Ponto p1={min,min}, p2={max,min}, p3={max,max}, p4={min,max};
    
    Segmento *b1 = malloc(sizeof(Segmento)); *b1 = (Segmento){p1, p2, -1};
    Segmento *b2 = malloc(sizeof(Segmento)); *b2 = (Segmento){p2, p3, -1};
    Segmento *b3 = malloc(sizeof(Segmento)); *b3 = (Segmento){p3, p4, -1};
    Segmento *b4 = malloc(sizeof(Segmento)); *b4 = (Segmento){p4, p1, -1};
    
    list_insert_back(local, b1);
    list_insert_back(local, b2);
    list_insert_back(local, b3);
    list_insert_back(local, b4);

    // 3. Split at Angle 0 (Ray from center to (MAX_X, center.y))
    // Ray: y = centro.y, x > centro.x
    // We iterate specific index to allow adding to end
    int count = list_size(local);
    for(int i=0; i<count; i++) {
        Segmento *s = (Segmento*)list_get_at(local, i);
        
        // Check intersection with Ray 0
        // Simplest check: One point has y > center.y, other < center.y? 
        // Or one y=center.y and x > center.x?
        // Use 'interseccao_raio_segmento' with angle 0?
        
        Ponto inter = interseccao_raio_segmento(centro, 0.0, *s);
        
        // If valid intersection and strict internal (not endpoint)?
        // If endpoint lies on ray, we rely on standard angle sorting (0 vs 2PI).
        // But if it crosses, we MUST split.
        // inter.x > centro.x is guaranteed by 'ray' logic.
        // Check if inter is strictly between p1 and p2.
        
        if (!isnan(inter.x)) {
            // Check strict interior
            double d_total = distancia(s->p1, s->p2);
            double d1 = distancia(s->p1, inter);
            double d2 = distancia(s->p2, inter);
            
            // Allow small epsilon tolerance for endpoints
            if (d1 > EPSILON && d2 > EPSILON && fabs((d1+d2)-d_total) < EPSILON) {
                // Split!
                Segmento *s_new = malloc(sizeof(Segmento));
                
                // s goes p1->p2.
                // We make s: p1 -> inter
                // s_new: inter -> p2
                // Wait, does it matter? Both parts are walls.
                
                Ponto old_p2 = s->p2;
                s->p2 = inter;
                
                s_new->p1 = inter;
                s_new->p2 = old_p2;
                s_new->id = s->id;
                
                list_insert_back(local, s_new);
            }
        }
    }
    
    return local;
}

// --- Predicate: Is V in front of Biombo? --- //
static bool esta_na_frente(Ponto x, Ponto v, Segmento *biombo) {
    if (!biombo) return true;
    
    // Ray cast from X to V. Calculate dist to V and dist to Biombo intersection.
    // Since V is ON the ray (by definition), we compare distances.
    double d_v = distancia(x, v);
    
    // We compute angle of v relative to x
    double ang = angulo_polar(x, v);
    double d_biombo = distancia_raio_segmento(x, ang, *biombo);
    
    if (isnan(d_biombo)) return true; // Should ideally not happen if biombo covers angular range
    
    // If V is closer or equal (epsilon) to biombo intersection, it is in front.
    return (d_v <= d_biombo + EPSILON);
}

// --- Main Function --- //

static bool pontos_iguais(Ponto p1, Ponto p2) {
    return distancia(p1, p2) < EPSILON;
}

PoligonoVisibilidade visibilidade_calcular(Ponto centro, LinkedList barreiras) {
    g_centro = centro;
    LinkedList local_segs = preparar_segmentos(centro, barreiras);
    
    // 1. Create Events
    int qtd_segs = list_size(local_segs);
    int qtd_eventos = qtd_segs * 2;
    Evento **eventos = malloc(sizeof(Evento*) * qtd_eventos);
    int k = 0;
    
    for(int i=0; i<qtd_segs; i++) {
        Segmento *s = list_get_at(local_segs, i);
        double a1 = angulo_polar(centro, s->p1);
        double a2 = angulo_polar(centro, s->p2);
        
        // Ensure strictly 0..2PI range for logic? 'angulo_polar' usually atan2 (-PI..PI).
        // Let's normalize to 0..2PI to make Sweep straightforward (0 to 360).
        if (a1 < 0) a1 += 2*M_PI;
        if (a2 < 0) a2 += 2*M_PI;
        
        // "Angular interval" of segment: shorter arc? 
        // No, the segment exists in the interval defined by its endpoints.
        // But a segment might wrap 2PI-0? 
        // We split segments at 0 intersection, so NO segment should wrap 0!
        // This means for any segment, min(a1,a2) is start, max(a1,a2) is end. WRONG.
        // It depends on orientation? No, line segments between two points.
        // Since we split at 0, no segment crosses the positive X axis "cut".
        // So valid range is [min_ang, max_ang].
        // However, we need to know which endpoint is START and which is END for the sweep.
        // Start is the one with smaller angle?
        
        double ang_start, ang_end;
        Ponto p_start, p_end;
        
        if (a1 < a2) {
            ang_start=a1; ang_end=a2;
            p_start=s->p1; p_end=s->p2;
        } else {
            ang_start=a2; ang_end=a1;
            p_start=s->p2; p_end=s->p1;
        }
        
        eventos[k] = malloc(sizeof(Evento));
        eventos[k]->angulo = ang_start;
        eventos[k]->dist = distancia(centro, p_start);
        eventos[k]->tipo = TIPO_INICIO;
        eventos[k]->seg = s;
        eventos[k]->p = p_start;
        k++;
        
        eventos[k] = malloc(sizeof(Evento));
        eventos[k]->angulo = ang_end;
        eventos[k]->dist = distancia(centro, p_end);
        eventos[k]->tipo = TIPO_FIM;
        eventos[k]->seg = s;
        eventos[k]->p = p_end;
        k++;
    }
    // 4. Ordenar eventos radialmente
    sort(eventos, k, sizeof(Evento*), comparar_eventos, g_sort_type, g_sort_threshold);
    
    // 2. Active List (Linear Scan)
    LinkedList ativos = list_create();
    PoligonoVisibilidade poly = poligono_criar(centro);
    
    // Deduplication state
    Ponto ultimo_ponto = {0,0};
    bool tem_ultimo = false;
    
    Segmento *biombo_atual = NULL;
    // We don't need p_biombo_start if we just cast rays to current biombo.

    for(int i=0; i<k; i++) {
        Evento *e = eventos[i];
        g_angulo_atual = e->angulo; // Update global sweep angle context
        
        // --- 1. Update Active List ---
        if (e->tipo == TIPO_INICIO) {
            list_insert_back(ativos, e->seg);
        } else { // TIPO_FIM
            // Remove segment from active list
            // Assuming list_remove compares by pointer value or we search index
            // list_remove(list, val) typically removes first occurrence of val.
            // Since we insert pointers, it should work if list checks ptr equality.
            // If list checks value equality with comparator, might be tricky.
            // Let's assume list_remove searches for the pointer.
            // Checking list.h might be good, but standard usually does.
            // If not, we search manually.
            int idx = -1;
            for(int j=0; j<list_size(ativos); j++) {
                if (list_get_at(ativos, j) == e->seg) {
                    idx = j;
                    break;
                }
            }
            if (idx != -1) list_remove_at(ativos, idx);
        }

        // --- 2. Find Closest Barrier (Biombo) ---
        Segmento *novo_biombo = NULL;
        double min_dist = DBL_MAX;
        
        int n_active = list_size(ativos);
        for(int j=0; j<n_active; j++) {
            Segmento *s = list_get_at(ativos, j);
            double d = distancia_raio_segmento(centro, e->angulo, *s);
            
            if (!isnan(d) && d < min_dist - EPSILON) {
                min_dist = d;
                novo_biombo = s;
            } else if (!isnan(d) && fabs(d - min_dist) < EPSILON) {
                // Tie-breaker: pick one consistently (e.g. smallest ID)
                // This prevents flickering if overlapping
                if (novo_biombo == NULL || s->id < novo_biombo->id) {
                     min_dist = d;
                     novo_biombo = s;
                }
            }
        }
        
        // --- 3. Handle Transitions ---
        if (biombo_atual != novo_biombo) {
            // Transition detected!
            
            // Intersection with OLD biombo at current angle?
            if (biombo_atual) {
                Ponto inter_old = interseccao_raio_segmento(centro, e->angulo, *biombo_atual);
                if (!isnan(inter_old.x)) {
                    if (!tem_ultimo || !pontos_iguais(ultimo_ponto, inter_old)) {
                         poligono_adicionar_vertice(poly, inter_old);
                         ultimo_ponto = inter_old;
                         tem_ultimo = true;
                    }
                }
            }
            
            if (novo_biombo) {
                Ponto inter_new = interseccao_raio_segmento(centro, e->angulo, *novo_biombo);
                if (!isnan(inter_new.x)) {
                     if (!tem_ultimo || !pontos_iguais(ultimo_ponto, inter_new)) {
                        poligono_adicionar_vertice(poly, inter_new);
                        ultimo_ponto = inter_new;
                        tem_ultimo = true;
                     }
                }
            }
            
            biombo_atual = novo_biombo;
        } else {
             // Biombo didn't change, but we are at a vertex.
             // If this vertex belongs to the current biombo, we should add it (corner).
             if (biombo_atual && (e->seg == biombo_atual)) {
                 if (!tem_ultimo || !pontos_iguais(ultimo_ponto, e->p)) {
                     poligono_adicionar_vertice(poly, e->p);
                     ultimo_ponto = e->p;
                     tem_ultimo = true;
                 }
             }
        }
    }
    
    // Cleanup
    for(int i=0; i<k; i++) free(eventos[i]);
    free(eventos);
    
    // Destroy local segments
    int n_loc = list_size(local_segs);
    for(int i=0; i<n_loc; i++) free(list_get_at(local_segs, i));
    list_destroy(local_segs);
    
    list_destroy(ativos); // Don't free segments, they are owned by local_segs

    return poly;
}

// --- Point in Polygon --- //
bool visibilidade_ponto_atingido(PoligonoVisibilidade pol, Ponto p) {
    if (!pol) return false;

    // Simple ray casting or winding number check?
    // Since it's a star-shaped polygon (vis region), we can use angular check + distance.
    Ponto centro = poligono_get_centro(pol);
    double dist_p = distancia(centro, p);
    double ang_p = angulo_polar(centro, p);
    if (ang_p < 0) ang_p += 2*M_PI;
    
    // Find edge covering this angle
    LinkedList vertices = poligono_get_vertices(pol);
    int n = list_size(vertices);
    if (n < 3) return false;
    
    for(int i=0; i<n; i++) {
        Ponto *v1 = list_get_at(vertices, i);
        Ponto *v2 = list_get_at(vertices, (i+1)%n);
        
        double a1 = angulo_polar(centro, *v1); if (a1<0) a1+=2*M_PI;
        double a2 = angulo_polar(centro, *v2); if (a2<0) a2+=2*M_PI;
        
        // Check if ang_p is between a1 and a2?
        // Handle wrap around.
        // But simpler: just intersect ray(ang_p) with segment(v1,v2).
        
        Ponto inter = interseccao_raio_segmento(centro, ang_p, (Segmento){*v1, *v2, 0});
        if (!isnan(inter.x)) {
            double d_edge = distancia(centro, inter);
            if (dist_p <= d_edge + EPSILON) return true;
        }
    }
    return false;
}

void visibilidade_destruir_poly(PoligonoVisibilidade pol) {
    poligono_destruir(pol);
}