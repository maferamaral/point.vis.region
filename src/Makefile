# Makefile atualizado para automatizar OBJETOS e dependências
.DEFAULT_GOAL := ted
PROJ_NAME = ted
LIBS = -lm

# Diretório para arquivos objeto
OBJ_DIR = obj

# Tenta find primeiro, se falhar usa wildcard
SRC_FILES := $(shell find . -name "*.c" ! -name "test_sort.c" ! -path "./$(OBJ_DIR)/*" 2>/dev/null)
ifeq ($(SRC_FILES),)
    SRC_FILES := $(filter-out test_sort.c, $(wildcard *.c) $(wildcard */*.c) $(wildcard */*/*.c))
endif

# Converte paths de .c para .o com nomes únicos baseados no basename
# ./main.c -> obj/main.o
# ./lib/geo/geo.c -> obj/geo.o
OBJETOS := $(addprefix $(OBJ_DIR)/,$(notdir $(SRC_FILES:.c=.o)))

# Compilador e Flags
CC = gcc
CFLAGS = -ggdb -O0 -std=c99 -fstack-protector-all -Werror=implicit-function-declaration
LDFLAGS = -O0

# Adiciona todos os diretórios de source ao VPATH para que make encontre os .c
VPATH = $(sort $(dir $(SRC_FILES)))

# Regra principal
ted: $(OBJ_DIR) $(OBJETOS)
	$(CC) -o $(PROJ_NAME) $(LDFLAGS) $(OBJETOS) $(LIBS)

# Criar diretório obj
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# Regra para compilar arquivos .c em .o dentro de obj/
$(OBJ_DIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

# Target para limpeza
clean:
	rm -rf $(OBJ_DIR) $(PROJ_NAME)

.PHONY: clean debug run ted

# Target para debug (mostra variáveis)
debug:
	@echo "SRC_FILES: $(SRC_FILES)"
	@echo "OBJETOS: $(OBJETOS)"
	@echo "VPATH: $(VPATH)"

test_sort:
	$(CC) $(CFLAGS) -o test_sort test_sort.c lib/utils/sort/sort.c
	./test_sort

run:
	./$(PROJ_NAME) -f test/test.geo -o test/results -q test/test.qry