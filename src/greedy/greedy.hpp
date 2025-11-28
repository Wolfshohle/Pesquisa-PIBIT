#pragma once
#include <vector>
#include "../structures/facilities.hpp"
#include "../structures/penality.hpp"
#include "../structures/solution.hpp"

// Algoritmo ganancioso para o problema de localização de instalações
Srepresentation greedUFL(std::vector<instalacao>& instalacoes, std::vector<std::vector<std::pair<int, int>>>& custo_de_conexao, std::vector<penalidade>& penalidades);