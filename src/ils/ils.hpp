#pragma once
#include <iostream>
#include <fstream>
#include <chrono>
#include <cstdlib>
#include <random>
#include <climits>
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

        int SEED;


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
            ls.improveSolution2();
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
        // Pertubação nova
        //! Verificar melhor depois
        // ===============================
        void perturbation2(Srepresentation& solucao)
        {
            int n = inst.qtd_clientes;
            int m = inst.qtd_instalacoes;

            vector<int> facilitys_to_close;

            for(int i = 0; i < m; i++)
            {
                if(solucao.openfacilities[i] == 1)
                    facilitys_to_close.push_back(i);
            }

            if(facilitys_to_close.empty())
            {
                return; // Se não houver instalações abertas, não há o que perturbar
            }

            // =========================================================================
            // Seleciona as instalações para fechar
            int num_to_close = max(1, (int)(0.09 * facilitys_to_close.size())); // Fecha 9% das instalações com mais conflitos
            vector<int> penality_for_facility(m, 0); // {custo médio de penalidade, qtd_clientes}

            // Calcula a penalidade total para cada instalação
            for(auto penality: inst.penalidades_vetor)
            {
                int facility1 = solucao.assignments[penality.clientes.first];
                int facility2 = solucao.assignments[penality.clientes.second];

                if(facility1 == facility2)
                {
                    penality_for_facility[facility1] += penality.custo;
                }
            }

            // Conta a quantidade de clientes por instalação
            vector<int> clients_per_facility(m, 0);
            for(int i = 0; i < n; i++)
            {
                clients_per_facility[solucao.assignments[i]]++;
            }

            // Calcula a penalidade média por cliente para cada instalação
            vector<pair<double, int>> ranking; // {penalidade média por cliente, instalação}
            for(int f: facilitys_to_close)
            {
                if(clients_per_facility[f] > 0)
                {
                    ranking.push_back({(double)penality_for_facility[f] / clients_per_facility[f], f});
                }
            }

            // Ordena as instalações pela penalidade média por cliente
            sort(ranking.rbegin(), ranking.rend());

            // Seleciona as instalações para fechar, dando preferência às que têm maior penalidade média por cliente
            int rcl_size = min((int)ranking.size(), num_to_close * 2);
            vector<int> selected_to_close; // instalações que serão fechadas

            vector<bool> used(rcl_size, false);

            for(int i = 0; i < num_to_close; i++)
            {
                int idx;
                do {
                    idx = rand() % rcl_size;
                } while(used[idx]);

                used[idx] = true;
                selected_to_close.push_back(ranking[idx].second);
            }
            // =========================================================================

            // Identifica os clientes afetados pelas instalações selecionadas para fechar
            vector<int> removed_clients;
            for(int i = 0; i < n; i++)
            {
                for(int facility : selected_to_close)
                {
                    if(solucao.assignments[i] == facility)
                    {
                        removed_clients.push_back(i);
                        solucao.assignments[i] = -1; // Marca o cliente como não alocado
                        break;
                    }
                }
            }

            // Fecha as instalações selecionadas
            for(int facility : selected_to_close)
            {
                solucao.openfacilities[facility] = 0;
            }

            // Reatribui os clientes removidos para novas instalações
            for(int client : removed_clients)
            {
                vector<pair<int, int>> candidate_facilities; // {custo, instalação}

                for(int i = 0; i < m; i++)
                {
                    if(solucao.openfacilities[i] == 1)
                    {
                        int connection_cost = inst.custo_conexao[i][client].second;;
                        candidate_facilities.push_back({connection_cost, i});
                    }
                }

                if(candidate_facilities.empty())
                {
                    continue; // Se não houver instalações abertas para realocar, passa para o próximo cliente
                }

                // Ordena as instalações candidatas pelo custo de conexão
                sort(candidate_facilities.begin(), candidate_facilities.end());

                // Considera as 3 melhores opções e seleciona aleatoriamente entre elas
                int q = min(10, (int)candidate_facilities.size());
                // Seleciona aleatoriamente entre as q melhores opções
                int selected_facility = candidate_facilities[rand() % q].second;

                solucao.assignments[client] = selected_facility;
            }

            // Adiciona um pouco de randomização a solução
            /*int extra_moves = max(1, (int)(0.05 * n)); // Move 5% dos clientes para instalações aleatórias
            for(int i = 0; i < extra_moves; i++)
            {
                int client = rand() % n;
                int new_facility = solucao.assignments[client];

                // Garante que a nova instalação seja diferente da atual
                do 
                {
                    new_facility = rand() % m;
                } 
                while(solucao.openfacilities[new_facility] == 0 ||solucao.assignments[client] == new_facility);
            }*/
            return;
        }
        // ===============================


        // ===============================
        // Reconstrução da solução
        // ===============================
        void reconstruction(Srepresentation& atualSolution)
        {
            int n = inst.qtd_clientes;
            int m = inst.qtd_instalacoes;
            int i;

            for(i = 0; i < m; i++)
            {
                atualSolution.openfacilities[i] = 0;
            }

            for(i = 0; i < n; i++)
            {
                atualSolution.openfacilities[atualSolution.assignments[i]] = 1;
            }
            

            atualSolution.totalCost = calculocusto(inst, atualSolution);
        };
        // ===============================


        // ===============================
        // Critério de aceitação
        // ===============================
        bool acceptanceCriterion(const Srepresentation& candidate)
        {
            double margem = 0.2;
            return candidate.totalCost < atualSolution.totalCost /* (1 + margem)*/;
        };
        // ===============================


    public:
        // ==============================
        // Construtor
        // ==============================
        ILS(instance& instancia, int timelimit, int perturbation_size, int seed):
            inst(instancia), maxtime(timelimit), pertubsize(perturbation_size), SEED(seed)
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
            long long sum = 0;
            double mean = 0;
            int worst_value = 0;
            int iterations_to_the_best_solution = 0;
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
                perturbation2(candidateSolution);
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
                        iterations_to_the_best_solution = total_iterations;
                    }
                }
                if(candidateSolution.totalCost > worst_value)
                {
                    worst_value = candidateSolution.totalCost;
                }
                sum += atualSolution.totalCost;

                total_iterations++;
            }

            mean = (double)sum / total_iterations;

            cout << "Total de iterações: " << total_iterations << endl;
            cout << "Melhorias: " << improvement_iterations << endl;
            cout << "Tempo até a melhor solução: " << time_to_best << " segundos" << endl;
            cout << "Iterações até a melhor solução: " << iterations_to_the_best_solution << endl;
            cout << "Soma dos custos: " << sum << endl;
            cout << "Média dos custos: " << mean << endl;
            cout << "Pior valor encontrado: " << worst_value << endl;

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