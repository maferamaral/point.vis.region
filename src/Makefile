# Makefile atualizado para automatizar OBJETOS e dependências
.DEFAULT_GOAL := ted
PROJ_NAME = ted
LIBS = -lm
# Tenta find primeiro, se falhar usa wildcard
SRC_FILES := $(shell find . -name "*.c" ! -name "test_sort.c" 2>/dev/null)
ifeq ($(SRC_FILES),)
    # Exclude test_sort.c manually if find fails (fallback)
    SRC_FILES := $(filter-out test_sort.c, $(wildcard *.c) $(wildcard */*.c) $(wildcard */*/*.c))
endif
OBJETOS := $(SRC_FILES:.c=.o)       # Substitui .c por .o

# Compilador e Flags
CC = gcc
CFLAGS = -ggdb -O0 -std=c99 -fstack-protector-all -Werror=implicit-function-declaration
LDFLAGS = -O0

# Regra principal
ted: $(OBJETOS)
	$(CC) -o $(PROJ_NAME) $(LDFLAGS) $(OBJETOS) $(LIBS)

# Regra para compilar arquivos .c em .o com dependências
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@


# Target para limpeza
clean:
	rm -f $(OBJETOS) $(PROJ_NAME)

.PHONY: clean debug run ted

# Target para debug (mostra variáveis)
debug:
	@echo "SRC_FILES: $(SRC_FILES)"
	@echo "OBJETOS: $(OBJETOS)"

test_sort:
	$(CC) $(CFLAGS) -o test_sort test_sort.c lib/utils/sort/sort.c
	./test_sort

run:
	./$(PROJ_NAME) -f test/test.geo -o test/results -q test/test.qry