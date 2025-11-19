#pragma once
#include <vector>
#include <utility>

struct instalacao
{
    int custo_abertura; // Custo de abrir a instalação
    std::vector<std::pair<int, int>> ordem_por_custo; // Custo da instalação i atender o cliente j em ordem crescente pelo custo
    std::vector<int> Dlinha; // Conjunto de clientes já atendidos e que podem mudar para nova instalação
    std::vector<int> atribuicao; // guarda a atribuição dos clientes depois de ordenados
    std::vector<int> Y; // Clientes que ainda não foram atendidos
};