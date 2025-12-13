# Makefile atualizado para automatizar OBJETOS e dependências
.DEFAULT_GOAL := ted
PROJ_NAME = ted
LIBS = -lm

# Diretório para arquivos objeto
OBJ_DIR = obj

# Find all .c files but exclude main.c and any test_*.c files from the main source list
SAFE_SRC_FILES := $(shell find . -name "*.c" ! -name "main.c" ! -name "test_*.c" ! -path "./$(OBJ_DIR)/*" 2>/dev/null)
ifeq ($(SAFE_SRC_FILES),)
    # Fallback if find fails or implies different shell
    SAFE_SRC_FILES := $(filter-out main.c test_%.c, $(wildcard *.c) $(wildcard */*.c) $(wildcard */*/*.c))
endif

# Main is separate
MAIN_SRC := main.c
SRC_FILES := $(SAFE_SRC_FILES) $(MAIN_SRC)

# Objects
OBJETOS := $(addprefix $(OBJ_DIR)/,$(notdir $(SRC_FILES:.c=.o)))
SAFE_OBJETOS := $(addprefix $(OBJ_DIR)/,$(notdir $(SAFE_SRC_FILES:.c=.o)))

# Compilador e Flags
CC = gcc
CFLAGS = -ggdb -O0 -std=c99 -fstack-protector-all -Werror=implicit-function-declaration -I.
LDFLAGS = -O0

# Adiciona todos os diretórios de source ao VPATH
VPATH = $(sort $(dir $(SRC_FILES)))

# Regra principal
ted: $(OBJ_DIR) $(OBJETOS)
	$(CC) -o $(PROJ_NAME) $(LDFLAGS) $(OBJETOS) $(LIBS)

# Criar diretório obj
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# Regra genérica para .o
$(OBJ_DIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

# Tests
TEST_SRCS = $(wildcard tests/test_*.c)
TEST_EXES = $(TEST_SRCS:.c=)

test_lista: $(OBJ_DIR) $(SAFE_OBJETOS) tests/test_lista.c
	$(CC) $(CFLAGS) tests/test_lista.c $(SAFE_OBJETOS) -o test_lista $(LIBS)
	./test_lista

test_circulo: $(OBJ_DIR) $(SAFE_OBJETOS) tests/test_circulo.c
	$(CC) $(CFLAGS) tests/test_circulo.c $(SAFE_OBJETOS) -o test_circulo $(LIBS)
	./test_circulo

test_retangulo: $(OBJ_DIR) $(SAFE_OBJETOS) tests/test_retangulo.c
	$(CC) $(CFLAGS) tests/test_retangulo.c $(SAFE_OBJETOS) -o test_retangulo $(LIBS)
	./test_retangulo

test_linha: $(OBJ_DIR) $(SAFE_OBJETOS) tests/test_linha.c
	$(CC) $(CFLAGS) tests/test_linha.c $(SAFE_OBJETOS) -o test_linha $(LIBS)
	./test_linha

test_texto: $(OBJ_DIR) $(SAFE_OBJETOS) tests/test_texto.c
	$(CC) $(CFLAGS) tests/test_texto.c $(SAFE_OBJETOS) -o test_texto $(LIBS)
	./test_texto

test_geo: $(OBJ_DIR) $(SAFE_OBJETOS) tests/test_geo.c
	$(CC) $(CFLAGS) tests/test_geo.c $(SAFE_OBJETOS) -o test_geo $(LIBS)
	./test_geo

test_all: test_lista test_circulo test_retangulo test_linha test_texto test_geo

# Old test
test_sort:
	$(CC) $(CFLAGS) -o test_sort test_sort.c lib/utils/sort/sort.c
	./test_sort

# Target para limpeza
clean:
	rm -rf $(OBJ_DIR) $(PROJ_NAME) test_lista test_circulo test_retangulo test_linha test_texto test_geo test_sort test_sample.geo

.PHONY: clean debug run ted test_all test_lista test_circulo test_retangulo test_linha test_texto test_geo

# Target para debug (mostra variáveis)
debug:
	@echo "SRC_FILES: $(SRC_FILES)"
	@echo "OBJETOS: $(OBJETOS)"
	@echo "VPATH: $(VPATH)"

run:
	./$(PROJ_NAME) -f test/test.geo -o test/results -q test/test.qry