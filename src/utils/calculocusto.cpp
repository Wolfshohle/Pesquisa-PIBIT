#include "calculocusto.hpp"
#include <iostream>

int calculocusto(const instance& inst, const Srepresentation& sol)
{
    int custo_total = 0;

    // Custo das instalações abertas
    for(int i : sol.openfacilities)
    {
        custo_total += inst.instalacoes[i].custo_abertura;
    }

    // Custo das conexões
    for(size_t cliente = 0; cliente < sol.assignments.size(); cliente++)
    {
        int instalacao_atribuida = sol.assignments[cliente];
        custo_total += inst.custo_conexao[instalacao_atribuida][cliente].first;
    }

    // Custo das penalidades
    for(const auto& pen : inst.penalidades)
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