---
trigger: always_on
description: When you need to change somethin on the reg vis algorithm
---

# Implementation Guide: Angular Sweep Visibility Algorithm

## 1\. Context & Objective

Implement the **Angular Plane Sweep Algorithm** to calculate the visibility polygon $V(x)$ from a source point $x$ given a set of line segment obstacles $S$.

  * [cite\_start]**Source:** `point-vis-region.pdf`[cite: 8, 10].
  * [cite\_start]**Architecture:** C99 Standard, Opaque Pointers (ADT), Modular design (`lib/` directory)[cite: 321, 353].
  * [cite\_start]**Complexity Goal:** $O(n \log n)$[cite: 106].

-----

## 2\. Data Structures (Internal Representations)

Although headers (`.h`) must use opaque pointers (`void*`), the implementation (`.c`) requires specific structures.

### 2.1. Basic Primitives

  * **Point:** $(x, y)$ coordinates.
  * **Segment:**
      * [cite\_start]`p_initial`, `p_final`: Endpoints[cite: 11].
      * `id`: Original shape identifier.
      * `helper`: Pointer to the node in the Active Segment Tree (for fast deletion).
  * **Vertex (Event):**
      * `point`: The coordinate.
      * [cite\_start]`type`: `START` or `END` of a segment[cite: 137].
      * [cite\_start]`segment`: Pointer to the owning segment[cite: 138].
      * [cite\_start]`angle`: Polar angle relative to source $x$[cite: 12].
      * [cite\_start]`distance`: Euclidean distance to source $x$[cite: 143].

-----

## 3\. Geometric Predicates (Math Helpers)

Implement these helper functions strictly based on the PDF definitions.

### 3.1. Orientation (Cross Product)

[cite\_start]Determines if point $Z$ is to the Left, Right, or Collinear relative to segment $\vec{XY}$[cite: 98, 99].
$$2 \cdot Area = (y.x - x.x)(z.y - x.y) - (y.y - x.y)(z.x - x.x)$$

  * Result $> 0$: **Left Turn**.
  * Result $< 0$: **Right Turn**.
  * Result $= 0$: **Collinear**.

### 3.2. Ray Intersection

[cite\_start]Calculates point $y$ where a ray from source $x$ passing through vertex $v$ intersects segment $s$[cite: 47, 72].

-----

## 4\. The Algorithm: Phase 1 (Preprocessing)

Before the sweep loop, prepare the data:

1.  [cite\_start]**Bounding Box:** Insert 4 segments forming a large box around all geometry to ensure the ray always hits *something*[cite: 155].
2.  **Angle 0 Splitting:**
      * Trace a ray from $x$ at $0^\circ$ (horizontal right).
      * If any segment intersects this ray, split it into two new segments:
          * $s_{final}$: from original start to intersection point.
          * $s_{initial}$: from intersection point to original end.
      * [cite\_start]This ensures no segment "wraps around" the $360^\circ \rightarrow 0^\circ$ transition [cite: 164-166].
3.  **Event Creation:** Extract all endpoints from segments as vertices.
4.  [cite\_start]**Sorting:** Sort the vertex list using `qsort` or `mergesort` with this priority [cite: 141-144]:
    1.  **Angle:** Ascending ($0 \dots 2\pi$).
    2.  **Distance:** Descending (if angles are equal, process farther points first?). *Correction per PDF:* If angles are equal, check logic strictly:
          * [cite\_start]If types differ: Process `START` before `END`[cite: 144].
          * If types are same: Sort by distance (closer vertices might block farther ones).

-----

## 5\. The Algorithm: Phase 2 (The Sweep Loop)

### 5.1. Active Segment Tree (BST)

[cite\_start]Maintain a Balanced Binary Search Tree (BST) of "Active Segments" (segments currently intersecting the sweep ray)[cite: 107].

  * [cite\_start]**Comparison Logic:** Given two segments $s_1$ and $s_2$ in the tree, $s_1 < s_2$ if $s_1$ intersects the current ray **closer** to $x$ than $s_2$[cite: 108].
  * **Operations Required:** `Insert`, `Delete`, `FindMin` (closest segment).

### 5.2. Initialization

  * Create an empty list for the Output Polygon ($V$).
  * Find all segments intersecting the ray at $t=0$ (just after the split step). Insert them into the BST.
  * [cite\_start]Set `current_biombo` (blocking segment) = `BST.FindMin()`[cite: 149].

### 5.3. Processing Events

Iterate through the sorted vertices ($v$):

[cite\_start]**CASE A: Vertex $v$ is START of Segment $s_v$** [cite: 35]

1.  **Check Visibility:** Is $s_v$ blocked by the `current_biombo`?
      * Use geometry: Is $v$ behind `current_biombo`?
2.  **If BLOCKED (Behind):**
      * [cite\_start]Just insert $s_v$ into the BST[cite: 38].
3.  [cite\_start]**If VISIBLE (In Front):** [cite: 44]
      * This implies $s_v$ obscures the current `biombo`.
      * Calculate intersection point $I$ between ray $(x, v)$ and `current_biombo`.
      * [cite\_start]Add segment $[Biombo_{start}, I]$ to $V$[cite: 55].
      * Add segment $[I, v]$ to $V$.
      * [cite\_start]Update `current_biombo` = $s_v$[cite: 58].
      * Insert $s_v$ into BST.

[cite\_start]**CASE B: Vertex $v$ is END of Segment $s_v$** [cite: 59]

1.  **Check Visibility:** Is $s_v$ the `current_biombo`?
2.  [cite\_start]**If NOT Biombo (Hidden):** [cite: 60]
      * Just remove $s_v$ from BST. No visual change.
3.  [cite\_start]**If IS Biombo (Visible):** [cite: 68]
      * The obstruction ends. We must see what's behind it.
      * Add segment $[Biombo_{start}, v]$ to $V$.
      * Remove $s_v$ from BST.
      * Find new `biombo` = `BST.FindMin()` (the segment immediately behind $s_v$).
      * Calculate intersection point $I$ between ray $(x, v)$ and the *new* `biombo`.
      * [cite\_start]Add segment $[v, I]$ to $V$[cite: 71].
      * Update `current_biombo` start point to $I$.

-----

## 6\. Output Generation

  * The algorithm produces a list of segments/points representing the visibility polygon.
  * Store this in a generic List ADT.
  * **SVG Output:** Convert this polygon into an SVG path `<path d="M x0 y0 L x1 y1 ... Z" />`.

-----

## 7\. Critical Edge Cases & Constraints

1.  **Robustness:** Use an epsilon ($\epsilon = 10^{-9}$) for floating-point comparisons (`double`). Do not use `==`.
2.  **Collinearity:** If source $x$, vertex $v_1$, and vertex $v_2$ are collinear, strict sorting order is mandatory to avoid processing the "hidden" vertex before the "blocking" one.
3.  **Modularization:**
      * `lib/vis.c`: Contains the sweep logic.
      * `lib/bst.c`: Contains the tree logic.
      * `lib/geo.c`: Contains math predicates.
4.  **Memory:** Every `malloc` must have a corresponding `free`. The BST nodes must be freed when segments are removed.

-----

## 8\. Pseudo-Code Mapping (Algorithm 1 from PDF)

```c
// Simplified C-style logic mapping
List calculate_visibility(List segments, Point x) {
    List events = extract_vertices(segments);
    sort_vertices(events, x); // Angle sort
    
    BST active_segments = bst_create();
    Segment biombo = find_initial_closest(segments, x); // Ray at angle 0
    
    // Initial segments at angle 0 must be in BST
    bst_insert_initial(active_segments, segments_at_zero);

    foreach (Vertex v in events) {
        if (v.type == START) {
             Segment s = v.segment;
             // Check if v is strictly "in front" of biombo
             if (is_in_front(v, biombo, x)) {
                 Point y = intersect(x, v, biombo);
                 output_add(biombo.start, y);
                 output_add(y, v.point);
                 biombo = s;
                 biombo.start = v.point; // Update virtual start
             }
             bst_insert(active_segments, s);
        } else { // END
             Segment s = v.segment;
             if (s == biombo) {
                 output_add(biombo.start, v.point);
                 bst_remove(active_segments, s);
                 
                 Segment next = bst_find_min(active_segments);
                 Point y = intersect(x, v, next);
                 output_add(v.point, y);
                 
                 biombo = next;
                 biombo.start = y;
             } else {
                 bst_remove(active_segments, s);
             }
        }
    }
    return output;
}
```