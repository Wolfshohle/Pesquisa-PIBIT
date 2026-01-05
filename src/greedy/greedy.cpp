#include <vector>
#include "greedy.hpp"
#include "../utils/buscabinaria.hpp"
#include "../utils/calculocusto.hpp"
#include <algorithm>
#include <iostream>
#include <limits>

using namespace std;

// Algoritmo ganancioso para o problema de localização de instalações
bool greedUFL(instance& inst, Srepresentation& solucao)
{
    // variáveis gerais
    int i, j, k, l, m, custos, economia;
    float phi;

    // Conjuntos
    vector<int> X; // Instalações abertas
    vector<int> D; // Clientes atendidos
    vector<int> sigma(inst.qtd_clientes, -1); // Atribuição de clientes a instalação
    vector<int> O(inst.qtd_instalacoes, 1); // Conjunto binário que informa se a fábrica foi aberta ou não



    // Ordena os clientes com base no menor valor pra instalação e armazena para cada instalação
    vector<vector<pair<int, int>>>matriz_ordenacao = inst.custo_conexao;
    for(i = 0; i < matriz_ordenacao.size(); i++)
    {
        sort(matriz_ordenacao[i].begin(), matriz_ordenacao[i].end());
        inst.instalacoes[i].ordem_por_custo = matriz_ordenacao[i];
    }


    // Inicia o conjunto de clientes não atendidos
    for(i  = 0; i < matriz_ordenacao.size(); i++)
    {
        inst.instalacoes[i].Y.resize(matriz_ordenacao[i].size(), 0);
        inst.instalacoes[i].atribuicao.resize(matriz_ordenacao[i].size(), 0);
    }

    // Guarda as atribuições
    for(i = 0 ; i < inst.qtd_instalacoes; i++)
    {
        for(j = 0; j < inst.qtd_clientes; j++)
        {
            inst.instalacoes[i].atribuicao[inst.instalacoes[i].ordem_por_custo[j].second] = j;
        }
    }


    // Escolha local
    while(D.size() != inst.qtd_clientes)
    {
        vector<int> deltinha(inst.qtd_instalacoes, 0); // Economia que obtemos ao mudar o cliente de instalação
        vector<pair<int, int>> bestY; // Melhor conjunto de clientes alocados
        int bestI = -1; // Melhor instalação para "abrir"
        float bestphi = numeric_limits<float>::max(); // Inicia bestphi com o maior valor do tipo float
        int penalidade_cliente_alocando = 0;
        int penalidade_cliente_alocado = 0;
        int penalidade_cliente_queMigra = 0;

        // Looping das instalacoes
        for(i = 0; i < inst.qtd_instalacoes; i++)
        {

            penalidade_cliente_alocado = 0;
            penalidade_cliente_queMigra = 0;
            // δ(D, i) = som_j∈D max(0,(d(σ(j), j) − d(i, j))
            inst.instalacoes[i].Dlinha = {};
            for(auto j : D)
            {
                // não considerar clientes que já estão alocados nesta instalação
                if(sigma[j] == i) 
                {
                    continue;
                }

                // busca se tem penalidade entre os clientes que desejam migrar
                for(auto k : inst.instalacoes[i].Dlinha)
                {
                    penalidade_cliente_queMigra += buscabinariadepar(inst.penalidades_vetor, j, k);
                }

                // busca se tem penalidade entre o cliente que já estão alocados
                for(auto l : D)
                {
                    if(j != l && sigma[j] == i && sigma[l] == i)
                    {
                        penalidade_cliente_alocado += buscabinariadepar(inst.penalidades_vetor, j, l);
                    }
                }

                // adicionar penalidade aqui
                economia = inst.custo_conexao [sigma[j]][j].first - inst.custo_conexao[i][j].first - penalidade_cliente_queMigra + penalidade_cliente_alocado;
                if(economia > 0)
                {
                    inst.instalacoes[i].Dlinha.push_back(j);
                    deltinha[i] += economia;
                }
            }
            // --------------------------------------------------------------------------------


            // Φ(i, Y) := (0_{i∉X} · f_i + ∑_{j∈Y} d(i, j) − δ(D, i)) / |Y|
            custos = 0;
            vector<pair<int, int>> Y = {};
            float bestphilocal = numeric_limits<float>::max();
            penalidade_cliente_alocando = 0;
            penalidade_cliente_alocado = 0;
            penalidade_cliente_queMigra = 0;

            for(j = 0; j < inst.qtd_clientes; j++)
            {
                if(inst.instalacoes[i].Y[j] == 0)
                {
                    Y.push_back(inst.instalacoes[i].ordem_por_custo[j]);
                    // Adicionar um looping aqui para as penalidades


                    // Busca a penalidade entre os clientes que estão sendo alocados
                    for(auto k: Y)
                    {
                        penalidade_cliente_alocando += buscabinariadepar(inst.penalidades_vetor, inst.instalacoes[i].ordem_por_custo[j].second, k.second);
                    }

                    // Busca a penalidade entre os clientes que já estão alocados
                    for(auto l: D)
                    {
                        if(sigma[l] == i)
                        {
                            penalidade_cliente_alocado += buscabinariadepar(inst.penalidades_vetor, inst.instalacoes[i].ordem_por_custo[j].second, l);
                        }
            
                    }

                    // Busca a penalidade entre os clientes que migrarão se a instalação for aberta
                    for(auto m: inst.instalacoes[i].Dlinha)
                    {
                        penalidade_cliente_queMigra += buscabinariadepar(inst.penalidades_vetor, inst.instalacoes[i].ordem_por_custo[j].second, m);
                    }

                    custos += inst.instalacoes[i].ordem_por_custo[j].first;

                    phi = (O[i] * inst.instalacoes[i].custo_abertura + custos - deltinha[i] + penalidade_cliente_alocando + penalidade_cliente_alocado + penalidade_cliente_queMigra) / (float)Y.size();
                    
                    if(phi >= bestphilocal)
                    {
                        break;
                    }

                    bestphilocal = phi;

                    if(bestphilocal < bestphi)
                    {
                        bestphi = bestphilocal;
                        bestY = Y;
                        bestI = i;
                    }
                }
            }
            // --------------------------------------------------------------------------------
        }

        // Atribuicoes finais
        // Verifica se uma nova facilidade foi aberta
        if(O[bestI] == 1)
        {
            X.push_back(bestI);
            for(auto j : inst.instalacoes[bestI].Dlinha)
            {
                sigma[j] = bestI;
            }
            
            O[bestI] = 0;
        }

        for(auto j: bestY)
        {
            D.push_back(j.second);
            sigma[j.second] = bestI;
        }

        for(i = 0; i < inst.instalacoes.size(); i++)
        {
            for(auto j: bestY)
            { 
                inst.instalacoes[i].Y[inst.instalacoes[i].atribuicao[j.second]] = 1;
            }
        }
    }

    solucao.openfacilities = X;
    solucao.assignments = sigma;

    solucao.totalCost = calculocusto(inst, solucao);
    
    return true;
}