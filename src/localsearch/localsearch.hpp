#pragma once
#include <vector>
#include <queue>
#include <algorithm>
#include <iostream>
#include "../structures/instance.hpp"
#include "../structures/solution.hpp"
#include "../Asserts/automatizedtests.hpp"


class LocalSearch 
{
    private:
        instance &inst; // Dados da instância
        Srepresentation &sol; // Solução atual


        // ===============================
        // Calcula a diferença de custo ao mover um cliente para uma nova instalação (Versão para movimentação cliente a cliente)
        // ===============================
        int calculateSingleClientRelocationDelta(int client, int new_facility)
        {
            int current_facility = sol.assignments[client];
            int delta = 0;

            // Calcula a diferença de custo da movimentação
            delta += inst.custo_conexao[new_facility][client].first - 
            inst.custo_conexao[current_facility][client].first;

            // Se vai abrir uma instalação nova
            if(sol.openfacilities[new_facility] == 0)
                delta += inst.instalacoes[new_facility].custo_abertura;

            // Se vai fechar a instalação antiga

            if(sol.info.ClientsPerFacility[current_facility] == 1)
            {
                delta -= inst.instalacoes[current_facility].custo_abertura;
            }

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
        // TODO: Devido ao fato de ser uma movimentação em lote, o uso da estrutura solutioninfo não é possível
        // ===============================
        int calculateBatchRelocationDelta(int client, int new_facility, std::vector<int>& assignments_backup)
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
                if(assignments_backup[i] == current_facility && i != client)
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
        void applySingleClientRelocation(int best_client, int best_facility, int best_delta)
        {
            int current_facility = sol.assignments[best_client];

            // Atualiza a atribuição do cliente
            sol.assignments[best_client] = best_facility;

            // Atualiza o custo total da solução
            sol.totalCost += best_delta;

            // Atualiza a ocupação das instalações
            sol.info.ClientsPerFacility[current_facility]--;
            sol.info.ClientsPerFacility[best_facility]++;

            // Fecha a instalação antiga se necessário
            if(sol.info.ClientsPerFacility[current_facility] == 0)
            {
                sol.info.decrementFacilitiesCount();
                sol.info.atualizeOnePositionOpenClose(current_facility);

                sol.openfacilities[current_facility] = 0;
            }

            // Abre a instalação antiga se necessáro
            if(sol.openfacilities[best_facility] == 0)
            {
                sol.info.incrementFacilitiesCount();
                sol.info.atualizeOnePositionOpenClose(best_facility);

                sol.openfacilities[best_facility] = 1;
            }

            // Atualiza as penalidades para a instalação antiga e nova
            for(auto conflito : inst.penalidades_grafo[best_client])
            {
                int conflicted_client = conflito.first;
                int penality_cost = conflito.second;

                // Atualiza a penalidade da instalação antiga
                if(sol.assignments[conflicted_client] == current_facility)
                {
                    sol.info.PenalityPerFacility[current_facility] -= penality_cost;
                }
                // Atualiza a penalidade da nova instalação
                if(sol.assignments[conflicted_client] == best_facility)
                {
                    sol.info.PenalityPerFacility[best_facility] += penality_cost;
                }
            }
        }
        // ===============================


        // ===============================
        // Aplica a movimentação (Versão para movimentação em lote)
        // TODO: Preciso urgentemente criar mais um dado na struct de solutioninfo para otimizar essa busca em batch. Atualmente a otimização é quase 0
        // ===============================
        void applyBatchRelocation(const std::vector<int>& clients, int facility, int delta_total)
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
            

            // Reseta a contagem de instalações aberdas
            sol.info.resetFacilitiesCount();
            // Limpa o vetor que armazena as instalações abertas
            sol.info.OpenFacilities.clear();
            // Limpa o vetor que armazena as instalações fechadas
            sol.info.ClosedFacilities.clear();
            // Zera a quantidade de clientes nas instalações
            fill(sol.info.ClientsPerFacility.begin(), sol.info.ClientsPerFacility.end(), 0);
            // Zera o valor total de penalidades em cada instalação
            fill(sol.info.PenalityPerFacility.begin(), sol.info.PenalityPerFacility.end(), 0);

            // Verifica se é necessário fechar ou abrir instalações
            for(i = 0; i < inst.qtd_instalacoes; i++)
            {
                if(facilitys_to_close[i] > 0)
                {
                    sol.openfacilities[i] = 1;

                    sol.info.incrementFacilitiesCount();
                    sol.info.ClientsPerFacility[i] = facilitys_to_close[i];
                    sol.info.OpenFacilities.push_back(i);
                }
                else
                {
                    sol.openfacilities[i] = 0;

                    sol.info.ClosedFacilities.push_back(i);
                }
            }

            // Atualiza as penalidades por instalações
            for(const auto& pen: inst.penalidades_vetor)
            {
                int c1 = pen.clientes.first;
                int c2 = pen.clientes.second;
                
                if(sol.assignments[c1] == sol.assignments[c2])
                {
                    sol.info.PenalityPerFacility[sol.assignments[c1]] += pen.custo;
                }
            }

            // Atualiza a estrutura solutioninfo internamente
            sol.info.atualizeAllPositionOpenClose();
        }
        // ===============================


        // ===============================
        // Encontra a melhor movimentação possível (Versão para movimentação cliente a cliente)
        bool findBestSingleClientRelocation()
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
                    delta = calculateSingleClientRelocationDelta(client, newfacility);

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
                applySingleClientRelocation(best_client, best_new_facility, best_delta);
                return true;
            }

            return false;
        }
        // ===============================


        // ===============================
        // Encontra a melhor movimentação possível (Versão para movimentação em lote)
        // Verificar depois se é possível mover clientes que estão em instalações diferentes, mas que ainda assim geram uma redução de custo
        // ===============================
        bool findBestBatchRelocation()
        {
            // ----------------------------------------------------------
            // Variaveis para armazenar a melhor movimentação encontrada
            // ----------------------------------------------------------
            int best_delta = INT_MAX; // Melhor delta encontrado
            int best_facility = -1; // Melhor instalação para mover
            std::vector<int> best_move; // Melhor conjunto de clientes a mover
            // ----------------------------------------------------------


            //! Pode ser removido com a implementação da nova estrutura de dados
            // -----------------------------------------------------------
            // Escolhe as instalações candidatas para mover os clientes
            // -----------------------------------------------------------
            std::vector<int> facilitys_to_open;
            for(int f = 0; f < inst.qtd_instalacoes; f++)
            {
                if(sol.openfacilities[f] == 0)
                {
                    facilitys_to_open.push_back(f);
                }
            }
            // -----------------------------------------------------------


            //! Aqui eu posso inserir apenas o sol.info.ClosedFacilities
            // -----------------------------------------------------------
            // Ordena as instalações candidatas por custo de abertura
            // -----------------------------------------------------------
            std::sort(facilitys_to_open.begin(), facilitys_to_open.end(),
                [this](int a, int b) {
                    return inst.instalacoes[a].custo_abertura <
                        inst.instalacoes[b].custo_abertura;
                });
            // -----------------------------------------------------------


            //! Aqui deve ser feito a mesma alteração em facilitys_to_open
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
                    int delta = calculateBatchRelocationDelta(client, facility, sol.assignments);

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
                std::vector<int> C;
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
                while(!next_clients.empty())
                {
                    auto top = next_clients.top();
                    int client = top.second;
                    next_clients.pop();

                    int delta = calculateBatchRelocationDelta(client, facility, assignments_backup);
                    if(delta + delta_atual < delta_atual)
                    {
                        C.push_back(client);
                        delta_atual += delta;

                        // Atualiza o backup das atribuições para refletir a movimentação
                        assignments_backup[client] = facility;

                        for(auto conflito : inst.penalidades_grafo[client])
                        {
                            int conflicted_client = conflito.first;

                            if(!in_queue[conflicted_client])
                            {
                                int new_delta = calculateBatchRelocationDelta(conflicted_client, facility, assignments_backup);
                                next_clients.push({new_delta, conflicted_client});
                                in_queue[conflicted_client] = true;
                            }
                        }
                    }
                }
                // ----------------------------

                int delta_total = delta_atual + open_cost;
                if(delta_total < best_delta)
                {
                    best_delta = delta_total;
                    best_facility = facility;
                    best_move = C;
                }
            }
            // -----------------------------------------------------------

            // -----------------------------------------------------------
            // Aplica a melhor movimentação encontrada
            // -----------------------------------------------------------
            if(best_facility != -1 && !best_move.empty() && best_delta < 0)
            {
                applyBatchRelocation(best_move, best_facility, best_delta);
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
        LocalSearch(instance &instancia, Srepresentation &solucao):
            inst(instancia), sol(solucao){}
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
        // Executa a busca local para melhorar a solução (Versão para movimentação cliente a cliente)
        // ===============================
        void improveBySingleClientRelocation()
        {
            bool improved = true;
            while(improved)
            {
                improved = findBestSingleClientRelocation();
            }
        }
        // ===============================


        // ===============================
        // Executa a busca local para melhorar a solução (Versão para movimentação em lote)
        // ===============================
        void improveByBatchRelocation()
        {
            bool improved = true;
            int seguranca = 90;
            while(improved && seguranca > 0)
            {
                improved = findBestBatchRelocation();
                seguranca--;
            }

            if(seguranca == 0)
            {
                std::cout << "Limite de segurança atingido, parando busca local." << std::endl;
            }
        }
        // ===============================
};
