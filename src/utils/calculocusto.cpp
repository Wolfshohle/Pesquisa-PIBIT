#include "calculocusto.hpp"
#include <iostream>
#include <vector>

int calculocusto(const instance& inst, Srepresentation& sol)
{
    int custo_total = 0;
    int i, j, size;
    std::vector<int> openedFacilities;
    std::vector<int> closedFacilities;



    size = sol.openfacilities.size();

    sol.info.resetFacilitiesCount();

    // Custo das instalações abertas
    for(i = 0; i < size; i++)
    {
        if(sol.openfacilities[i] == 1)
        {
            custo_total += inst.instalacoes[i].custo_abertura;

            openedFacilities.push_back(i);
            sol.info.incrementFacilitiesCount();
        }
        else
        {
            closedFacilities.push_back(i);
        }
    }

    // Atualiza as informações da solução
    sol.info.OpenFacilities = openedFacilities;
    sol.info.ClosedFacilities = closedFacilities;
    sol.info.atualizeAllPositionOpenClose();

    fill(sol.info.ClientsPerFacility.begin(), sol.info.ClientsPerFacility.end(), 0);

    // Custo das conexões
    size = sol.assignments.size();
    for(j = 0; j < size; j++)
    {
        int instalacao_atribuida = sol.assignments[j];
        custo_total += inst.custo_conexao[instalacao_atribuida][j].first;

        sol.info.ClientsPerFacility[instalacao_atribuida]++;
    }

    fill(sol.info.PenaltyPerFacility.begin(), sol.info.PenaltyPerFacility.end(), 0);

    // Custo das penalidades
    for(const auto& pen : inst.penalidades_vetor)
    {
        int c1 = pen.clientes.first;
        int c2 = pen.clientes.second;
        if(sol.assignments[c1] == sol.assignments[c2])
        {
            custo_total += pen.custo;

            sol.info.PenaltyPerFacility[sol.assignments[c1]] += pen.custo;
        }
    }

    return custo_total;
}