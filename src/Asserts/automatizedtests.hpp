#pragma once
#include <iostream>
#include <string>
#include <cstdlib>
#include "../structures/solution.hpp"
#include "../structures/instance.hpp"
#include "../utils/calculocusto.hpp"

using namespace std;

class asserts
{
    public:
        void check(bool condition, const string& message)
        {
            if(!condition)
            {
                cerr << "Assertion failed: " << message << endl;
                abort();
            }
        }

        void checkresult(Srepresentation& solucao, const instance& inst)
        {
            // Verifica se o número de instalações abertas é consistente
            int openFacilitiesCount = 0;
            for(int i = 0; i < inst.qtd_clientes; i++)
            {
                if(solucao.openfacilities[i] == 1)
                {
                    openFacilitiesCount++;
                }
            }

            check(openFacilitiesCount == solucao.info.amountOpenFacilities, "inconsistent number of open facilities in the solution");

            // Verifica se todos os clientes estão atribuídos a instalações abertas
            for(int i = 0; i < inst.qtd_clientes; i++)
            {
                int assignedFacility = solucao.assignments[i];
                check(assignedFacility >= 0 && assignedFacility < inst.qtd_instalacoes, "client assigned to an invalid facility");
                check(solucao.openfacilities[assignedFacility] == 1, "client assigned to a closed facility");
            }

            // Verifica se o custo total da solução é consistente
            long long calculatedCost = calculocusto(inst, solucao);
            check(calculatedCost == solucao.totalCost, "inconsistent total cost in the solution");
        }
};