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
	tempo=30; \
	pertubacao=5; \
	for i in $$(seq 1 10); do \
		echo "---------Execução $$i---------"; \
		echo "------------------------------"; \
		./$(BIN) -T $$tempo -s $$i -P $$pertubacao "Data/examples/wlp18-convertida.txt" >"Data/examples/wlp18-convertida_heuristic_seed_$${i}.log";\
		echo "------------------------------"; \
	done



# Para teste rápido
run_individual_heuristic: $(BIN)
	tempo=30;\
	pertubacao=5; \
	seed=25; \
	archive_name="Data/examples/wlp18-convertida.txt"; \
	./$(BIN) -T $$tempo -s $$seed -P $$pertubacao $$archive_name > "$${archive_name}_heuristic.log"



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
	logs="logs/exato"; \
	mkdir -p $$logs; \
	for i in $$instancias/*.txt; do \
		nome=$$(basename "$$i" .txt); \
		if [ -f "$$logs/$$nome.log" ]; then \
			echo "EXATO já rodou para $$i, pulando..."; \
			continue; \
		fi; \
		echo "Rodando EXATO na instância $$i"; \
		./$(CPLEX_BIN) "$$i" > "$$logs/$$nome.log"; \
	done
# ============================================================

run_individual_cplex: cplex
	archive_name="Data/examples/wlp02-convertida.txt"; \
	echo "Rodando o exato para o arquivo"; \
	./$(CPLEX_BIN) $$archive_name > "$${archive_name}_exact.log"

run_heuristic: $(BIN)
	instancias="Data/raw"; \
	logs="logs/heuristica"; \
	mkdir -p $$logs; \
	tempo=30; \
	perturbacao=5; \
	for arquivo in $$instancias/*.txt; do \
		nome=$$(basename "$$arquivo" .txt); \
		for seed in $$(seq 1 10); do \
			if [ -f "$$logs/$${nome}_seed$${seed}.log" ]; then \
				echo "HEURÍSTICA já rodou para $$arquivo seed $$seed, pulando..."; \
			else \
				echo "Rodando HEURÍSTICA na instância $$arquivo com seed $$seed"; \
				./$(BIN) -T $$tempo -s $$seed -P $$perturbacao "$$arquivo" > "$$logs/$${nome}_seed$${seed}.log"; \
			fi; \
		done; \
	done


# ============================================================
# GERADOR DE INSTÂNCIAS PDIG
# ============================================================
PDIG_BIN := Data/examples/PDIG

PDIG:
	for m in 0 1; do \
		for p in 0.05 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0; do \
			for i in $$(seq 1 10); do \
				echo "---------Execução $$i | mode=$$m | p=$$p ---------"; \
				echo "------------------------------"; \
				./$(PDIG_BIN) -p $$p -m $$m -s $$i; \
				echo "------------------------------"; \
			done; \
		done; \
	done
# ============================================================


build:
	make all
	make cplex

experiment:
	make build
	make run_cplex
	make run_heuristic
	make clean

individual_experiment:
	make build
	make run_individual_cplex
	execute
	make clean

# ============================================================
# Limpeza
# ============================================================
clean:
	rm -f $(OBJS_DIR)/*.o $(BIN) $(CPLEX_BIN)