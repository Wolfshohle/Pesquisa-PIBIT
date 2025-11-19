#pragma once
#include <string>
#include <vector>
#include "../structures/instalacao.hpp"
#include "../structures/penalidade.hpp"

bool carregarInstancia(std::ifstream &arquivo,
                       int &qtd_inst,
                       int &qtd_cli,
                       int &qtd_pen,
                       std::vector<instalacao> &instalacoes,
                       std::vector<std::vector<std::pair<int,int>>> &custo_conexao,
                       std::vector<penalidade> &penalidades);