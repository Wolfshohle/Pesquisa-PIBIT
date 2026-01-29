#pragma once
#include <vector>
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

        // Calcula a diferença de custo ao mover um cliente para uma nova instalação
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

        // Aplica a movimentação na solução
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
                // Remove a instalação fechada da lista de instalações abertas
                for(int i = 0; i < sol.openfacilities.size(); i++)
                {
                    if(sol.openfacilities[i] == current_facility)
                    {
                        sol.openfacilities.erase(sol.openfacilities.begin() + i);
                        break;
                    }
                }

                openflag[current_facility] = 0;
            }

            // Verifica se a instalação nova precisa ser marcada como aberta
            if(openflag[best_facility] == 0)
            {
                openflag[best_facility] = 1;
                sol.openfacilities.push_back(best_facility);
            }
        }

        // Encontra a melhor movimentação possível
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

    public:
        // Construtor
        LocalSearch(instance &instancia, Srepresentation solucao):
            inst(instancia), sol(solucao)
            {
                initializeAuxStructures();
            }

        // Inicializa estruturas auxiliares
        void initializeAuxStructures()
        {
            int f = inst.qtd_instalacoes;
            ocupacao.assign(f, 0);
            openflag.assign(f, 0);

            // Marca instalações abertas e conta ocupações
            for(auto facility : sol.openfacilities)
            {
                openflag[facility] = 1;
            }

            // Conta os clientes alocados em cada instalação
            for(auto assignment : sol.assignments)
            {
                ocupacao[assignment]++;
            }
        }

        // Retorna o grafo de conflitos
        std::vector<std::vector<std::pair<int, int>>> getConflictGraph()
        {
            return inst.penalidades_grafo;
        }

        // Retorna a solução atual
        Srepresentation getSolution()
        {
            return sol;
        }

        // Recalcula o custo para garantir que está correto
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

        // Executa a busca local para melhorar a solução
        void improveSolution()
        {
            bool improved = true;
            while(improved)
            {
                improved = bestmove();
            }
        }
};
