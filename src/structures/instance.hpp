#pragma once

#include <vector>
#include <utility>

#include "facilities.hpp"
#include "penality.hpp"

// Struct simples que agrega os dados da instância.
struct instance {
    int qtd_instalacoes = 0;
    int qtd_clientes = 0;
    int qtd_penalidades = 0;

    std::vector<instalacao> instalacoes; // Vetor de instalações
    std::vector<std::vector<std::pair<int,int>>> custo_conexao; // Matriz de custos de conexão (cliente -> (instalação, custo))
    std::vector<penalidade> penalidades; // Guarda as penalidades entre clientes ((cliente1, cliente2), (custo))
};
