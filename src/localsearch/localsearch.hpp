#pragma once
#include <vector>
#include <queue>
#include <algorithm>
#include <iostream>
#include "../structures/instance.hpp"
#include "../structures/solution.hpp"


class LocalSearch 
{
    private:
        instance &inst; // Dados da instância
        Srepresentation sol; // Solução atual

        std::vector<int> ocupacao; // Quantos clientes estão atribuídos a cada instalação
        std::vector<short int> openflag; // Flag de instalações abertas

        // ===============================
        // Calcula a diferença de custo ao mover um cliente para uma nova instalação (Versão para movimentação cliente a cliente)
        // ===============================
        int delta_move(int client, int new_facility)
        {
            int current_facility = sol.assignments[client];
            int delta = 0;

            // Calcula a diferença de custo da movimentação
            delta += inst.custo_conexao[new_facility][client].first - 
            inst.custo_conexao[current_facility][client].first;

            // Se vai abrir uma instalação nova
            if(openflag[new_facility] == 0)
                delta += inst.instalacoes[new_facility].custo_abertura;

            // Se vai fechar a instalação antiga
            if(ocupacao[current_facility] == 1)
                delta -= inst.instalacoes[current_facility].custo_abertura;

            // Penalidades (soma/remover conforme vizinhos do grafo)
            for(auto conflito : inst.penalidades_grafo[client])
            {
                int conflicted_client = conflito.first;
                int penalty_cost = conflito.second;

                if(sol.assignments[conflicted_client] == current_facility)
                {
                    delta -= penalty_cost;
                }
                if(sol.assignments[conflicted_client] == new_facility)
                {
                    delta += penalty_cost;
                }
            }

            return delta;
        }
        // ===============================


        // ===============================
        // Calcula a diferença de custo ao mover um cliente para uma nova instalação (versão para movimentação em lote)
        // ===============================
        int delta_move2(int client, int new_facility, std::vector<int>& assignments_backup)
        {
            int current_facility = assignments_backup[client];
            int old_facility_close = inst.instalacoes[current_facility].custo_abertura;
            int delta = 0;

            //* calcula o delta de custo antigo
            int old_cost = inst.custo_conexao[current_facility][client].first;
            

            //* calcula o delta de custo novo
            int new_cost = inst.custo_conexao[new_facility][client].first;

            //? Calcula as penalidades
            for(auto conflito : inst.penalidades_grafo[client])
            {
                int conflicted_client = conflito.first;
                int penalty_cost = conflito.second;
                // Acha os cliente conflitante na antiga instalação
                if(assignments_backup[conflicted_client] == current_facility)
                {
                    old_cost += penalty_cost;
                }

                // Acha os clientes conflitantes na nova instalação
                if(assignments_backup[conflicted_client] == new_facility)
                {
                    new_cost += penalty_cost;
                }
            }

            //* Verifica se a instalação antiga vai ser fechada
            for(int i = 0; i < inst.qtd_clientes; i++)
            {
                if(assignments_backup[i] == current_facility)
                {
                    old_facility_close = 0;
                    break;
                }
            }

            delta = new_cost - old_cost - old_facility_close;
            
            return delta;
        }
        // ===============================


        // ===============================
        // Aplica a movimentação (Versão para movimentação cliente a cliente)
        // ===============================
        void apply_move(int best_client, int best_facility, int best_delta)
        {
            int current_facility = sol.assignments[best_client];

            // Atualiza a atribuição do cliente
            sol.assignments[best_client] = best_facility;

            // Atualiza o custo total da solução
            sol.totalCost += best_delta;

            // Atualiza a ocupação das instalações
            ocupacao[current_facility]--;
            ocupacao[best_facility]++;

            // Verifica se é necessário fechar ou abrir instalações
            if(ocupacao[current_facility] == 0)
            {
                sol.openfacilities[current_facility] = 0;
                openflag[current_facility] = 0;
            }

            // Verifica se a instalação nova precisa ser marcada como aberta
            if(openflag[best_facility] == 0)
            {
                openflag[best_facility] = 1;
                sol.openfacilities[best_facility] = 1;
            }
        }
        // ===============================


        // ===============================
        // Aplica a movimentação (Versão para movimentação em lote)
        // ===============================
        void apply_move2(const std::vector<int>& clients, int facility, int delta_total)
        {
            int i;
            std::vector<int> facilitys_to_close(inst.qtd_instalacoes, 0);

            // Atualiza as atribuições dos clientes
            for(int client : clients)
            {
                sol.assignments[client] = facility;
            }

            // Atualiza custo uma única vez
            sol.totalCost += delta_total;

            // Atualiza a ocupação das instalações
            for(i = 0; i < inst.qtd_clientes; i++)
            {
                facilitys_to_close[sol.assignments[i]]++;
            }
            
            // Verifica se é necessário fechar ou abrir instalações
            for(i = 0; i < inst.qtd_instalacoes; i++)
            {
                if(facilitys_to_close[i] > 0)
                {
                    sol.openfacilities[i] = 1;
                }
                else
                {
                    sol.openfacilities[i] = 0;
                }
            }
        }
        // ===============================


        // ===============================
        // Encontra a melhor movimentação possível (Versão para movimentação cliente a cliente)
        bool bestmove()
        {
            int best_delta = 0;
            int best_client = -1, best_new_facility = -1;

            int num_clients = inst.qtd_clientes;
            int num_facilities = inst.qtd_instalacoes;

            // Itera sobre todos os clientes
            for(int client = 0; client < num_clients; client++)
            {
                // Pega a instalação atual do cliente
                int current_facility = sol.assignments[client];

                // Tenta mover o cliente para todas as outras instalações
                for(int newfacility = 0; newfacility < num_facilities; newfacility++)
                {
                    // Ignora a instalação atual
                    if(newfacility == current_facility)
                        continue;

                    int delta = 0;

                    // Calcula o delta de custo para a movimentação
                    delta = delta_move(client, newfacility);

                    // Verifica se é a melhor movimentação até agora
                    if(delta < best_delta)
                    {
                        best_delta = delta;
                        best_client = client;
                        best_new_facility = newfacility;
                    }
                }
            }

            // Aplica a melhor movimentação encontrada
            if(best_client != -1 && best_new_facility != -1)
            {
                apply_move(best_client, best_new_facility, best_delta);
                return true;
            }

            return false;
        }
        // ===============================


        // ===============================
        // Encontra a melhor movimentação possível (Versão para movimentação em lote)
        //! RESOLVER MELHOR ESSA BESTMOVE2, ESTÁ MUITO FEIA E INEFICIENTE
        // Verificar depois se é possível mover clientes que estão em instalações diferentes, mas que ainda assim geram uma redução de custo
        // ===============================
        bool bestmove2()
        {
            // ----------------------------------------------------------
            // Variaveis para armazenar a melhor movimentação encontrada
            // ----------------------------------------------------------
            int best_delta = INT_MAX; // Melhor delta encontrado
            int best_facility = -1; // Melhor instalação para mover
            std::vector<int> best_move; // Melhor conjunto de clientes a mover
            // ----------------------------------------------------------


            // -----------------------------------------------------------
            // Escolhe as instalações candidatas para mover os clientes
            // -----------------------------------------------------------
            std::vector<int> facilitys_to_open;
            for(int f = 0; f < inst.qtd_instalacoes; f++)
            {
                if(openflag[f] == 0)
                {
                    facilitys_to_open.push_back(f);
                }
            }
            // -----------------------------------------------------------


            // -----------------------------------------------------------
            // Ordena as instalações candidatas por custo de abertura
            // -----------------------------------------------------------
            std::sort(facilitys_to_open.begin(), facilitys_to_open.end(),
                [this](int a, int b) {
                    return inst.instalacoes[a].custo_abertura <
                        inst.instalacoes[b].custo_abertura;
                });
            // -----------------------------------------------------------


            // -----------------------------------------------------------
            // Itera sobre as instalações candidatas para encontrar a melhor movimentação
            // -----------------------------------------------------------
            for(int facility : facilitys_to_open)
            {
                int open_cost = inst.instalacoes[facility].custo_abertura; // Custo de abrir a nova instalação
                //double limit = 0.1 * open_cost; // Limite para considerar uma movimentação como viável (pode ser ajustado)

                // ----------------------------
                // Classificação dos clientes
                // ----------------------------
                std::vector<std::pair<int,int>> good, neutral;

                for(int client = 0; client < inst.qtd_clientes; client++)
                {
                    int delta = delta_move2(client, facility, sol.assignments);

                    if(delta < 0)
                    {
                        good.push_back({delta, client});
                    }
                }
                // ----------------------------

                if(good.empty())
                {
                    continue; // Se não houver clientes bons para mover, passa para a próxima instalação candidata
                }


                // ----------------------------
                // Inicialização
                // ----------------------------
                std::vector<int> S;
                std::vector<bool> in_queue(inst.qtd_clientes, false);
                std::vector<int> assignments_backup = sol.assignments;

                std::priority_queue<
                    std::pair<int,int>,
                    std::vector<std::pair<int,int>>,
                    std::greater<std::pair<int,int>>
                > next_clients;

                // ----------------------------
                // Coloca na fila os primeiros k clientes bons para serem movidos
                // ----------------------------
                int k = good.size(); // Limite para o número de clientes bons a considerar (pode ser ajustado)
                for(int i = 0; i < k; i++)
                {
                    next_clients.push(good[i]);
                    in_queue[good[i].second] = true;
                }
                // ----------------------------
                

                // ----------------------------
                // Processa a fila de clientes a serem movidos
                // ----------------------------
                int delta_atual = 0;
                //int neutro_index = 0;
                //int neutro_size = neutral.size();
                while(!next_clients.empty())
                {
                    auto top = next_clients.top();
                    int client = top.second;
                    next_clients.pop();

                    int delta = delta_move2(client, facility, assignments_backup);
                    if(delta + delta_atual < delta_atual)
                    {
                        S.push_back(client);
                        delta_atual += delta;

                        // Atualiza o backup das atribuições para refletir a movimentação
                        assignments_backup[client] = facility;

                        for(auto conflito : inst.penalidades_grafo[client])
                        {
                            int conflicted_client = conflito.first;

                            if(!in_queue[conflicted_client])
                            {
                                int new_delta = delta_move2(conflicted_client, facility, assignments_backup);
                                next_clients.push({new_delta, conflicted_client});
                                in_queue[conflicted_client] = true;
                            }
                        }

                        // Se houver clientes neutros, adiciona um
                        /*if(neutro_index < neutro_size && in_queue[neutral[neutro_index].first] == false)
                        {
                            next_clients.push(neutral[neutro_index].first);
                            in_queue[neutral[neutro_index].first] = true;
                            neutro_index++;
                        }
                        else if(neutro_index < neutro_size)
                        {
                            neutro_index++;
                        }*/
                    }
                }
                // ----------------------------

                int delta_total = delta_atual + open_cost;
                if(delta_total < best_delta)
                {
                    best_delta = delta_total;
                    best_facility = facility;
                    best_move = S;
                }
            }
            // -----------------------------------------------------------

            // -----------------------------------------------------------
            // Aplica a melhor movimentação encontrada
            // -----------------------------------------------------------
            if(best_facility != -1 && !best_move.empty() && best_delta < 0)
            {
                apply_move2(best_move, best_facility, best_delta);
                return true;
            }
            // -----------------------------------------------------------

            return false;
        }
        // ===============================

    public:
        // ===============================
        // Construtor
        // ===============================
        LocalSearch(instance &instancia, Srepresentation solucao):
            inst(instancia), sol(solucao)
            {
                initializeAuxStructures();
            }
        // ===============================


        // ===============================
        // Inicializa estruturas auxiliares
        // ===============================
        void initializeAuxStructures()
        {
            int f = inst.qtd_instalacoes;
            ocupacao.assign(f, 0);
            openflag.assign(f, 0);

            // Marca instalações abertas e conta ocupações
            for(long unsigned int i = 0; i < sol.openfacilities.size(); i++)
            {
                if(sol.openfacilities[i] == 1)
                {
                    openflag[i] = 1;
                }
            }

            // Conta os clientes alocados em cada instalação
            for(auto assignment : sol.assignments)
            {
                ocupacao[assignment]++;
            }
        }
        // ===============================


        // ===============================
        // Retorna o grafo de conflitos
        // ===============================
        std::vector<std::vector<std::pair<int, int>>> getConflictGraph()
        {
            return inst.penalidades_grafo;
        }
        // ===============================


        // ===============================
        // Retorna a solução atual
        // ===============================
        Srepresentation getSolution()
        {
            return sol;
        }
        // ===============================


        // ===============================
        // Recalcula o custo para garantir que está correto
        // ===============================
        void recalculateCost()
        {
            sol.totalCost = 0;
            
            // Custo das instalações abertas
            for(int i : sol.openfacilities)
            {
                sol.totalCost += inst.instalacoes[i].custo_abertura;
            }
            
            // Custo das conexões
            for(size_t cliente = 0; cliente < sol.assignments.size(); cliente++)
            {
                int instalacao_atribuida = sol.assignments[cliente];
                sol.totalCost += inst.custo_conexao[instalacao_atribuida][cliente].first;
            }
            
            // Custo das penalidades
            for(const auto& pen : inst.penalidades_vetor)
            {
                int c1 = pen.clientes.first;
                int c2 = pen.clientes.second;
                if(sol.assignments[c1] == sol.assignments[c2])
                {
                    sol.totalCost += pen.custo;
                }
            }
        }
        // ===============================


        // ===============================
        // Executa a busca local para melhorar a solução (Versão para movimentação cliente a cliente)
        // ===============================
        void improveSolution()
        {
            bool improved = true;
            while(improved)
            {
                improved = bestmove();
            }
        }
        // ===============================


        // ===============================
        // Executa a busca local para melhorar a solução (Versão para movimentação em lote)
        // ===============================
        void improveSolution2()
        {
            bool improved = true;
            int seguranca = 90;
            while(improved && seguranca > 0)
            {
                improved = bestmove2();
                seguranca--;
            }

            if(seguranca == 0)
            {
                std::cout << "Limite de segurança atingido, parando busca local." << std::endl;
            }
        }
        // ===============================
};
