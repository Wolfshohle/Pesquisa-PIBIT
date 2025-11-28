#pragma once
#include <vector>
#include <utility>

struct Srepresentation
{
    std::vector<int> openfacilities; // Instalações abertas
    std::vector<int> assignments; // Atribuições de clientes a instalações

    int totalCost; // Custo total da solução
};
