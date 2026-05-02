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



# Para teste rápido
execute2: $(BIN)
	tempo=180;\
	pertubacao=5; \
	seed=24; \
	./$(BIN) -T $$tempo -s $$seed -P $$pertubacao "Data/raw/80_P40_10.txt"



# ==============================
# ===== CPLEX CONFIG ===========
# ==============================
CPLEXDIR := /opt/ibm/ILOG/CPLEX_Studio2211
CPLEX_BIN := exact

CPLEX_INC := -I$(CPLEXDIR)/cplex/include \
             -I$(CPLEXDIR)/concert/include

CPLEX_LIB := -L$(CPLEXDIR)/cplex/lib/x86-64_linux/static_pic \
             -L$(CPLEXDIR)/concert/lib/x86-64_linux/static_pic

CPLEX_FLAGS := -lilocplex -lcplex -lconcert -lm -lpthread -DIL_STD

# Usa código existente do projeto + exact
CPLEX_SRC := src/ExactAlgorithm/exact.cpp \
             $(wildcard src/io/*.cpp) \
             $(wildcard src/utils/*.cpp) \
             $(wildcard src/strutures/*.cpp)

# ==============================
# Build do modelo exato
# ==============================
cplex:
	$(CXX) $(CXXFLAGS) $(CPLEX_SRC) $(INCLUDES) $(CPLEX_INC) $(CPLEX_LIB) $(CPLEX_FLAGS) -o $(CPLEX_BIN)

# Rodar direto
run_cplex: cplex
	instancias="Data/raw"; \
	logs="logs/cplex_test"; \
	mkdir -p $$logs; \
	for i in $$instancias/*.txt; do \
		nome=$$(basename "$$i" .txt); \
		if [ -f "$$logs/$$nome.log" ]; then \
			echo "Log para $$i já existe, pulando..."; \
			continue; \
		fi; \
		echo "Processando $$i..."; \
		./$(CPLEX_BIN) "$$i" > "$$logs/$$nome.log"; \
	done

PDIG:
	for p in 0.05 0.1 0.2 0.3 0.4 0.5 0.6; do \
		for i in $$(seq 1 10); do \
			echo "---------Execução $$i (p=$$p)---------"; \
			echo "------------------------------"; \
			./Data/examples/PDIG -p $$p;\
			echo "------------------------------"; \
		done; \
	done

clean:
	rm -f $(OBJS_DIR)/*.o $(BIN) $(CPLEX_BIN)