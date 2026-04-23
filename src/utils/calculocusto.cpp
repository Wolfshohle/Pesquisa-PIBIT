#include "calculocusto.hpp"
#include <iostream>

int calculocusto(const instance& inst, const Srepresentation& sol)
{
    int custo_total = 0;
    int i, j, size;

    size = sol.openfacilities.size();
    // Custo das instalações abertas
    for(i = 0; i < size; i++)
    {
        if(sol.openfacilities[i] == 1)
        {
            custo_total += inst.instalacoes[i].custo_abertura;
        }
    }

    // Custo das conexões
    size = sol.assignments.size();
    for(j = 0; j < size; j++)
    {
        int instalacao_atribuida = sol.assignments[j];
        custo_total += inst.custo_conexao[instalacao_atribuida][j].first;
    }

    // Custo das penalidades
    for(const auto& pen : inst.penalidades_vetor)
    {
        int c1 = pen.clientes.first;
        int c2 = pen.clientes.second;
        if(sol.assignments[c1] == sol.assignments[c2])
        {
            custo_total += pen.custo;
        }
    }

    return custo_total;
}