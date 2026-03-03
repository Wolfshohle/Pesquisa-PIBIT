#pragma once
#include <iostream>
#include <fstream>
#include <chrono>
#include <cstdlib>
#include "../io/inputload.hpp"
#include "../greedy/greedy.hpp"
#include "../localsearch/localsearch.hpp"
#include "../utils/print.hpp"

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

        int maxtime;
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
        ILS(instance& instancia, int timelimit, int perturbation_size, int seed):
            inst(instancia), maxtime(timelimit), pertubsize(perturbation_size)
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
            // Inicia o cronômetro para controle do tempo de execução
            using clock = std::chrono::steady_clock;
            auto start_time = clock::now();

            // Variável para contar o número total de iterações
            int total_iterations = 0;
            int improvement_iterations = 0;
            double time_to_best = 0.0;

            // Gera solução inicial usando o método guloso
            initialSolution();
            cout << "Solução inicial:" << endl;
            printSolution(atualSolution);

            // Aplica busca local para melhorar a solução inicial
            localSearch(atualSolution);
            cout << "Solução após busca local:" << endl;
            printSolution(atualSolution);

            // Define a melhor solução como a solução atual
            bestSolution = atualSolution;

            while(true)
            {
                // Verifica o tempo decorrido para garantir que não ultrapasse o limite
                auto current_time = clock::now();
                double elapsed_time = chrono::duration<double>(current_time - start_time).count();
                if(elapsed_time >= maxtime)
                {
                    break;
                }


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
                        improvement_iterations++;
                        time_to_best = elapsed_time;
                    }
                }


                total_iterations++;
            }

            cout << "Total de iterações: " << total_iterations << endl;
            cout << "Melhorias: " << improvement_iterations << endl;
            cout << "Tempo até a melhor solução: " << time_to_best << " segundos" << endl;

            atualSolution = bestSolution;

            cout << "Melhor solução encontrada:" << endl;
            printSolution(atualSolution);
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