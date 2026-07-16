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

        // Verificador de solução
        void checkresult(Srepresentation& solucao, const instance& inst)
        {
            // Verifica se o número de instalações abertas é consistente
            int openFacilitiesCount = 0;
            for(int i = 0; i < inst.qtd_instalacoes; i++)
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

        // Verificador de ammountOpenFacilities
        void checkammountOpenFacilities(const Srepresentation& solucao, const string &message)
        {
            int facilities_opened = 0;
            int facilities_size = solucao.openfacilities.size();

            for(int i = 0; i < facilities_size; i++)
            {
                if(solucao.openfacilities[i] == 1)
                {
                    facilities_opened++;
                }
            }

            check(facilities_opened == solucao.info.amountOpenFacilities, message);
            check(facilities_opened == solucao.info.OpenFacilities.size(), message);
        }

        // Verificador de OpenedFacilities
        void checkOpenedFacilities(const Srepresentation& solucao, const string &message)
        {
            int OpenedFacilities_size = solucao.info.OpenFacilities.size();

            for(int i = 0; i < OpenedFacilities_size; i++)
            {
                int facility = solucao.info.OpenFacilities[i];
                check(solucao.openfacilities[facility] == 1, message);
            }
        }

        // Verificador de ClosedFacilites
        void checkClosedFacilities(const Srepresentation& solucao, const string &message)
        {
            int ClosedFacilties_size = solucao.info.ClosedFacilities.size();

            for(int i = 0; i < ClosedFacilties_size; i++)
            {
                int facility = solucao.info.ClosedFacilities[i];
                check(solucao.openfacilities[facility] == 0, message);
            }
        }

        // Verificador de ClientsPerFacilities
        void checkClientsPerFacilities(const Srepresentation& solucao, const string &message)
        {
            int facilits_size = solucao.openfacilities.size();
            int clients_size = solucao.assignments.size();
            vector<int> clients_in_facility(facilits_size, 0);

            for(int i = 0; i < clients_size; i++)
            {
                clients_in_facility[solucao.assignments[i]]++;
            }

            for(int i = 0; i < facilits_size; i++)
            {
                int clientsPerFacility = solucao.info.ClientsPerFacility[i];
                check(clients_in_facility[i] == clientsPerFacility, message);
            }
        }

        // Verificador de penalidades por facilidades
        void checkPenalitysPerFacilities(const Srepresentation& solucao, const instance& inst, const string& message)
        {
            int facility_size = solucao.openfacilities.size();
            vector<int> check_penality(facility_size, 0);

            for(const auto& pen: inst.penalidades_vetor)
            {
                int c1 = pen.clientes.first;
                int c2 = pen.clientes.second;
                int cost = pen.custo;

                if(solucao.assignments[c1] == solucao.assignments[c2])
                {
                    check_penality[solucao.assignments[c1]] += cost;
                }
            }

            for(int i = 0; i < facility_size; i++)
            {
                int solpenalityPerFacility = solucao.info.PenalityPerFacility[i];
                check(check_penality[i] == solpenalityPerFacility, message);
            }
        }

        // Verificador de posições
        void checkpositions(const Srepresentation& solucao, const string& message)
        {
            int openedfacilitysposition_size = solucao.info.positionopen.size();
            int closedfacilitysposition_size = solucao.info.positionclose.size();
            int facilitys_size = solucao.openfacilities.size();

            for(int i = 0; i < facilitys_size; i++)
            {
                if(solucao.info.positionopen[i] != -1)
                {
                    int pos = solucao.info.positionopen[i];
                    check(solucao.info.OpenFacilities[pos] == i, message);
                }
                if(solucao.info.positionclose[i] != -1)
                {
                    int pos = solucao.info.positionclose[i];
                    check(solucao.info.ClosedFacilities[pos] == i, message);
                }
            }
        }
};