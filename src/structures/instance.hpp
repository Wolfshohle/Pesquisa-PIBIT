#pragma once
#include <vector>
#include <utility>
#include "penality.hpp"
#include "facilities.hpp"

struct instance
{
    int qtd_instalacoes = 0;
    int qtd_clientes = 0;
    int qtd_penalidades = 0;

    std::vector<instalacao> instalacoes;
    std::vector<std::vector<std::pair<int,int>>> custo_conexao;

    // Estrutura para guardar as penalidades entre os clientes
    std::vector<penalidade> penalidades_vetor;

    // Grafo de penalidades
    std::vector<std::vector<std::pair<int, int>>> penalidades_grafo;
};
