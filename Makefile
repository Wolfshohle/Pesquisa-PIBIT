# Configurações de compilação
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -O2

# Diretórios
SRCS_DIR := src
OBJS_DIR := obj
BIN := main

# Subdiretórios de fontes
SRCS_SUBFILES := greedy ils io localsearch strutures utils

# Pega a lista de arquivos fonte
SRC := $(SRCS_DIR)/main.cpp
SRC += $(foreach dir, $(SRCS_SUBFILES), $(wildcard $(SRCS_DIR)/$(dir)/*.cpp))

# Gera a lista de arquivos objeto correspondentes
OBJ := $(notdir $(SRC))
OBJ := $(OBJ:.cpp=.o)
OBJ := $(addprefix $(OBJS_DIR)/, $(OBJ))

# Cabeçalhos adicionais
INCLUDES := $(addprefix -I$(SRCS_DIR)/, $(SRCS_SUBFILES))

# Regras de compilação
all: $(BIN)

# Regra para criar o executável
$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Regra para compilar arquivos objeto
$(OBJS_DIR)/%.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(filter %/$*.cpp,$(SRC)) -o $@

# Testar mais tarde
execute: $(BIN)
	tempo=60; \
	pertubacao=30; \
	for i in $$(seq 1 10); do \
		echo "---------Execução $$i---------"; \
		echo "------------------------------"; \
		./$(BIN) -T $$tempo -s $$i -P $$pertubacao "Data/raw/in1 - large.txt";\
		echo "------------------------------"; \
	done

execute2: $(BIN)
	tempo=60;\
	pertubacao=5; \
	seed=24; \
	./$(BIN) -T $$tempo -s $$seed -P $$pertubacao "Data/raw/in1 - large.txt"

clean:
	rm -f $(OBJS_DIR)/*.o $(BIN)