#pragma once
#include <iostream>
#include <fstream>
#include "../io/inputload.hpp"
#include "../greedy/greedy.hpp"
#include "../localsearch/localsearch.hpp"

using namespace std;

//----------------------------------------------------------------------//
// Classe ILS
//----------------------------------------------------------------------//
class ILS
{
    private:
        instance& inst;

        Srepresentation atualSolution;
        Srepresentation bestSolution;

        int qtdvezesmelhorResultado = 0;

        int maxIter;
        int pertubsize;


        // ===============================
        // Solução inicial (Greedy)
        // ===============================
        void initialSolution()
        {
            greedUFL(inst, atualSolution);
        }
        // ===============================


        // ===============================
        // Busca local na solução atual
        // ===============================
        void localSearch(Srepresentation& solucao)
        {
            LocalSearch ls(inst, solucao);
            ls.improveSolution();
            solucao = ls.getSolution();
        };


        // ===============================
        // Perturbação da solução atual
        // ===============================
        void perturbation(Srepresentation& solucao)
        {
            int n = inst.qtd_clientes;
            int m = inst.qtd_instalacoes;

            for(int i = 0; i < pertubsize; i++)
            {
                // Seleciona um cliente aleatório
                int client = rand() % n;
                
                int new_facility = solucao.assignments[client];

                // Garante que a nova instalação seja diferente da atual
                while(solucao.assignments[client] == new_facility)
                {
                    new_facility = rand() % m;
                }

                solucao.assignments[client] = new_facility;
            }
        };
        // ===============================


        // ===============================
        // Reconstrução da solução
        // ===============================
        void reconstruction(Srepresentation& atualSolution)
        {
            int n = inst.qtd_clientes;
            int m = inst.qtd_instalacoes;
            int i;

            vector<int> ocupacao(m, 0);
            atualSolution.openfacilities.clear();

            for(i = 0; i < n; i++)
            {
                ocupacao[atualSolution.assignments[i]]++;
            }

            for(i = 0; i < m; i++)
            {
                if(ocupacao[i] > 0)
                {
                    atualSolution.openfacilities.push_back(i);
                }
            }

            atualSolution.totalCost = calculocusto(inst, atualSolution);
        };
        // ===============================


        // ===============================
        // Critério de aceitação
        // ===============================
        bool acceptanceCriterion(const Srepresentation& candidate)
        {
            return candidate.totalCost < atualSolution.totalCost;
        };
        // ===============================


    public:
        // ==============================
        // Construtor
        // ==============================
        ILS(instance& instancia, int max_iterations, int perturbation_size, int seed):
            inst(instancia), maxIter(max_iterations), pertubsize(perturbation_size)
            {
                if(seed == -1)
                {
                    srand(time(NULL));
                }
                else
                {
                    srand(seed);
                }
            }
        // ==============================


        // ==============================
        // ! Loop principal
        // ==============================
        void run()
        {
            int runtime = 0;

            initialSolution();

            localSearch(atualSolution);

            bestSolution = atualSolution;

            for(int i = 0; i < maxIter; i++)
            {
                Srepresentation candidateSolution = atualSolution;

                // Aplica perturbação jogando um cliente aleatório para outra instalação
                perturbation(candidateSolution);

                // Reconstrói a solução para ajustar instalações abertas e fechadas
                reconstruction(candidateSolution);

                // Aplica busca local na solução perturbada
                localSearch(candidateSolution);
                
                // Critério de aceitação
                if(acceptanceCriterion(candidateSolution))
                {
                    atualSolution = candidateSolution;

                    if(atualSolution.totalCost < bestSolution.totalCost)
                    {
                        bestSolution = atualSolution;
                    }
                }

                if(candidateSolution.totalCost == bestSolution.totalCost)
                {
                    qtdvezesmelhorResultado++;
                }

                runtime++;
            }

            atualSolution = bestSolution;
            cout << "Melhor resultado encontrado " << qtdvezesmelhorResultado << " vezes." << endl;
            cout << "Número de iterações: " << runtime << endl;
        }
        // ==============================


        // ==============================
        // Obtém a melhor solução encontrada
        // ==============================
        Srepresentation getBestSolution()
        {
            return bestSolution;
        }
        // ==============================
};