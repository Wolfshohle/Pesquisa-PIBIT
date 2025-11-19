#pragma once
#include <vector>
#include "../structures/instalacao.hpp"
#include "../structures/penalidade.hpp"

// Algoritmo ganancioso para o problema de localização de instalações
void greedUFL(std::vector<instalacao>& instalacoes, std::vector<std::vector<std::pair<int, int>>>& custo_de_conexao, std::vector<penalidade>& penalidades);