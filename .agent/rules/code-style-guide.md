---
trigger: always_on
---

# Code Style Guide & Architecture Standards

This document outlines the coding standards, conventions, and architectural patterns for the **Data Structures I - Project II** (Alien Visibility).

All code must strictly adhere to the **C99 standard** and use **Opaque Pointers** for Abstract Data Types (ADTs).

---

## 1. General Settings

* [cite_start]**Language Standard:** C99 (`-std=c99`). [cite: 353]
* [cite_start]**Compilation Flags:** `-fstack-protector-all` (Security flag required by project specs). [cite: 353]
* **Language:** All code, variable names, function names, and comments must be written in **English**.
* **Indentation:** 4 spaces (no tabs).

---

## 2. Modularization & Opaque Pointers (ADT)

To ensure strict encapsulation, we follow the **Opaque Pointer** pattern. The internal structure of data types must **never** be exposed in header files.

### 2.1. Header Files (`.h`) - The Contract
* [cite_start]**NO `struct` definitions** allowed in `.h` files. [cite: 321]
* Use `typedef void* TypeName;` to define the handle.
* Prototypes should operate on these opaque handles.
* [cite_start]Document every function (inputs, outputs, side effects) in the header. [cite: 323]

**Example (`lib/circle.h`):**
```c
#ifndef CIRCLE_H
#define CIRCLE_H

// Opaque pointer definition (Client doesn't know the struct size/content)
typedef void* Circle;

/**
 * Creates a new Circle instance.
 * @param id Unique identifier
 * @param x Center X coordinate
 * @param y Center Y coordinate
 * @param r Radius
 * @return Pointer to the new Circle object
 */
Circle circle_create(int id, double x, double y, double r);

/**
 * Gets the X coordinate of the circle.
 * @param c Circle handle
 * @return The X coordinate
 */
double circle_get_x(Circle c);

/**
 * Frees the memory allocated for the circle.
 * @param c Circle handle
 */
void circle_destroy(Circle c);

#endif
````

### 2.2. Source Files (`.c`) - The Implementation

  * Define the actual `struct` here.
  * Cast the `void*` handle to the concrete struct pointer to access fields.

**Example (`lib/circle.c`):**

```c
#include "circle.h"
#include <stdlib.h>

// Internal struct definition (hidden from the world)
typedef struct {
    int id;
    double x;
    double y;
    double r;
} CircleImpl;

Circle circle_create(int id, double x, double y, double r) {
    CircleImpl *new_circle = malloc(sizeof(CircleImpl));
    if (new_circle == NULL) {
        return NULL;
    }
    new_circle->id = id;
    new_circle->x = x;
    new_circle->y = y;
    new_circle->r = r;
    
    // Returns as void* (Circle)
    return (Circle) new_circle;
}

double circle_get_x(Circle c) {
    // Cast opaque pointer back to internal struct
    CircleImpl *impl = (CircleImpl*) c;
    return impl->x;
}

void circle_destroy(Circle c) {
    CircleImpl *impl = (CircleImpl*) c;
    free(impl);
}
```

-----

## 3\. Naming Conventions

### 3.1. Files

  * Use **snake\_case** for filenames.
  * Keep implementations in `lib/` and tests in `tests/`.
  * Example: `lib/linked_list.c`, `lib/linked_list.h`.

### 3.2. Types

  * Use **PascalCase** or **CamelCase** for ADT type names (e.g., `Circle`, `RbTree`, `List`).
  * Internal implementation structs should be suffixed with `Impl` (e.g., `CircleImpl`).

### 3.3. Functions

  * Use **snake\_case**.
  * **Prefix** functions with the module name to emulate namespaces.
      * `list_insert(...)`
      * `tree_remove(...)`
      * `vis_calculate(...)`

### 3.4. Variables

  * Use **snake\_case** (e.g., `current_node`, `max_value`).
  * Avoid single-letter names except for loop counters (`i`, `j`) or coordinates (`x`, `y`).

-----

## 4\. Generic Data Structures

Since we cannot use C++ templates, data structures (Lists, Trees) must store generic pointers (`void*`).

**Example (`lib/list.h`):**

```c
typedef void* List;
typedef void* Node;

// The list stores 'void* item', meaning it can store a Circle, a Rectangle, etc.
void list_insert(List l, void* item);
void* list_get_first(List l);
```

-----

## 5\. C99 Specifics

We are using strict C99. You may utilize:

  * **Variable declaration in for-loops:**
    ```c
    for (int i = 0; i < n; i++) { ... } // Allowed and encouraged
    ```
  * **Boolean type:**
    Use `<stdbool.h>` instead of defining TRUE/FALSE macros manually.
    ```c
    #include <stdbool.h>
    bool is_visible = true;
    ```
  * **Inline comments:**
    Use `//` for single-line comments.

-----

## 6\. Directory Structure

Ensure your file placement respects the modularization requirements:

```text
project_root/
├── Makefile           # Must include -fstack-protector-all -std=c99
├── src/               # Optional: if you want to separate main
│   └── main.c         # Argument parsing only
├── lib/               # ALL ADT modules (source + headers)
│   ├── geo.c
│   ├── geo.h
│   ├── list.c
│   └── list.h
└── tests/             # Unit tests for each module
    ├── test_list.c
    └── test_geo.c
```

```