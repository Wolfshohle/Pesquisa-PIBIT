#pragma once
#include <vector>
#include <utility>
#include "solutioninfo.hpp"

struct Srepresentation
{
    std::vector<int> openfacilities; // Instalações abertas
    std::vector<int> assignments; // Atribuições de clientes a instalações

    long long totalCost; // Custo total da solução

    solutioninfo info; // Informações adicionais sobre a solução
};
