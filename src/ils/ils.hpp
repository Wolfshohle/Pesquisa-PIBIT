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

        Srepresentation atual_solution;
        Srepresentation global_solution;
        Srepresentation candidate_solution;
        int localBestcost;


        int maxtime;
        int perturbsize;

        // É usada para definir o quão aleatório é a escolha das instalações a serem fechadas
        // Usado na perturbação PGP para aumentar um pouco a lista de instalações aumentando a aleatoridade
        int restricted_candidate_list_factor = 2;

        // É usada para definir o tamanho da lista de instalações consideradas para realocar os clientes
        // Usado na perturbação PGP para dar um limite máximo de instalações consideradas
        int restricted_candidate_list_size = 10;

        // É usado para controlar qual perturbação ou busca local aplicar
        int iteration = 0;

        int SEED;


        // ===============================
        // Solução inicial (Greedy)
        // ===============================
        void initialSolution()
        {
            greedUFL(inst, atual_solution);
        }
        // ===============================


        // ===============================
        // Busca local na solução atual
        //! MODIFICAR AQUI PARA DEIXAR A BUSCA LOCAL DINÂMICA, DEIXAR DO TIPO VND
        // ===============================
        void localSearch(Srepresentation& solucao)
        {
            LocalSearch ls(inst, solucao);
            ls.improveByBatchRelocation();
            ls.improveBySingleClientRelocation();
            solucao = ls.getSolution();
        };


        // ===============================
        // Perturbação Random client reassigment pertubation (RCRP)
        // Ou perturbação de realocação aleatória de clientes
        // ===============================
        void RCRP(Srepresentation& solucao)
        {
            int n = inst.qtd_clientes;
            int m = inst.qtd_instalacoes;

            for(int i = 0; i < perturbsize; i++)
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
        // Pertubação Penality-Guided Perturbation (PGP)
        // Ou perturbação guiada por penalidades
        // ===============================
        void PGP(Srepresentation& solucao)
        {
            int n = inst.qtd_clientes;
            int m = inst.qtd_instalacoes;

            vector<int> facilitys_to_close;

            for(int i = 0; i < m; i++)
            {
                if(solucao.openfacilities[i] == 1)
                    facilitys_to_close.push_back(i);
            }

            // Se não houver instalações abertas, não há o que perturbar
            if(facilitys_to_close.empty())
            {
                return;
            }

            // =========================================================================
            // Seleciona as instalações para fechar
            // Fecha uma quantidade de instalações baseada no tamanho da perturbação
            int num_to_close = min(perturbsize, (int)facilitys_to_close.size());

            // Vetor para armazenar a penalidade total associada a cada instalação
            vector<int> penality_for_facility(m, 0);

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
            // {penalidade média por cliente, instalação}
            vector<pair<double, int>> ranking;
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
            int restricted_candidate_close_size = min((int)ranking.size(), num_to_close * restricted_candidate_list_factor);
            vector<int> selected_to_close; // instalações que serão fechadas

            vector<bool> used(restricted_candidate_close_size, false);

            for(int i = 0; i < num_to_close; i++)
            {
                int idx;
                do {
                    idx = rand() % restricted_candidate_close_size;
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
                int interval_selected = min(restricted_candidate_list_size, (int)candidate_facilities.size());

                // Seleciona aleatoriamente entre as q melhores opções
                int selected_facility = candidate_facilities[rand() % interval_selected].second;

                solucao.assignments[client] = selected_facility;
            }

            return;
        }
        // ===============================


        // ===============================
        // Escolhe qual perturbação aplicar
        // ===============================
        
        // ===============================


        // ===============================
        // Reconstrução da solução
        // ===============================
        void reconstruction(Srepresentation& atual_solution)
        {
            int n = inst.qtd_clientes;
            int m = inst.qtd_instalacoes;
            int i;

            for(i = 0; i < m; i++)
            {
                atual_solution.openfacilities[i] = 0;
            }

            for(i = 0; i < n; i++)
            {
                atual_solution.openfacilities[atual_solution.assignments[i]] = 1;
            }
            

            atual_solution.totalCost = calculocusto(inst, atual_solution);
        };
        // ===============================


        // ===============================
        // Critério de aceitação
        //! MODIFICAR AQUI PARA DEIXAR A ATUALIZACAO DE PERTURBSIZE MAIS DINÂMICA, DEIXAR DO TIPO ANEALING
        // ===============================
        bool acceptanceCriterion(const Srepresentation& candidate)
        {
            double margem = 0.2;
            return candidate.totalCost < atual_solution.totalCost /* (1 + margem)*/;
        };
        // ===============================


    public:
        // ==============================
        // Construtor
        // ==============================
        ILS(instance& instancia, int timelimit, int perturbation_size, int seed):
            inst(instancia), maxtime(timelimit), perturbsize(perturbation_size), SEED(seed)
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
            // ===========================================
            // Inicia o cronômetro para controle do tempo de execução e métricas de desempenho
            // ===========================================
            using clock = std::chrono::steady_clock;
            auto start_time = clock::now();

            int worst_value = 0;
            int iterations_to_the_best_solution = 0;
            int improvement_iterations = 0;
            double time_to_best = 0.0;
            // ===========================================


            // ===========================================
            // Gera solução inicial usando o método guloso
            // ===========================================
            initialSolution();
            cout << "Solução inicial:" << endl;
            printSolution(atual_solution);
            // ===========================================


            // ===========================================
            // Aplica busca local para melhorar a solução inicial
            // ===========================================
            localSearch(atual_solution);
            cout << "Solução após busca local:" << endl;
            printSolution(atual_solution);
            // ===========================================


            // ===========================================
            // Define a melhor solução como a solução atual
            // ===========================================
            global_solution = atual_solution;
            // ===========================================


            // ===========================================
            // Loop principal do ILS
            // ===========================================
            while(true)
            {
                // Verifica o tempo decorrido para garantir que não ultrapasse o limite
                auto current_time = clock::now();
                double elapsed_time = chrono::duration<double>(current_time - start_time).count();
                if(elapsed_time >= maxtime)
                {
                    break;
                }


                candidate_solution = atual_solution;

                // Aplica perturbação jogando um cliente aleatório para outra instalação
                //! MODIFICAR AQUI PARA PARA DEIXAR DINÂMICO O TIPO DE PERTURBAÇÃO A SER APLICADA
                PGP(candidate_solution);
                RCRP(candidate_solution);

                // Reconstrói a solução para ajustar instalações abertas e fechadas
                reconstruction(candidate_solution);

                // Aplica busca local na solução perturbada
                localSearch(candidate_solution);
                
                // Critério de aceitação
                if(acceptanceCriterion(candidate_solution))
                {
                    atual_solution = candidate_solution;

                    if(atual_solution.totalCost < global_solution.totalCost)
                    {
                        global_solution = atual_solution;
                        improvement_iterations++;
                        time_to_best = elapsed_time;
                        iterations_to_the_best_solution = iteration;
                    }
                }
                if(candidate_solution.totalCost > worst_value)
                {
                    worst_value = candidate_solution.totalCost;
                }

                iteration++;
            }
            // ===========================================


            // ===========================================
            // Imprime os resultados e a melhor solução encontrada
            // ===========================================
            cout << "Total de iterações: " << iteration << endl;
            cout << "Melhorias: " << improvement_iterations << endl;
            cout << "Tempo até a melhor solução: " << time_to_best << " segundos" << endl;
            cout << "Iterações até a melhor solução: " << iterations_to_the_best_solution << endl;
            cout << "Pior valor encontrado: " << worst_value << endl;

            atual_solution = global_solution;

            cout << "Melhor solução encontrada:" << endl;
            printSolution(atual_solution);
            // ===========================================
        }
        // ==============================


        // ==============================
        // Obtém a melhor solução encontrada
        // ==============================
        Srepresentation getglobal_solution()
        {
            return global_solution;
        }
        // ==============================
};