#include <vector>
#include "greedy.hpp"
#include "buscabinaria.hpp"
#include <algorithm>
#include <iostream>
#include <limits>

using namespace std;

// Algoritmo ganancioso para o problema de localização de instalações
void greedUFL(vector<instalacao>& instalacoes, vector<vector<pair<int, int>>>& custo_de_conexao, vector<penalidade>& penalidades)
{
    // variáveis gerais
    int i, j, k, l, m, custos, economia;
    float phi;

    // Conjuntos
    vector<int> X; // Instalações abertas
    vector<int> D; // Clientes atendidos
    vector<int> sigma(custo_de_conexao[0].size(), -1); // Atribuição de clientes a instalação
    vector<int> O(instalacoes.size(), 1); // Conjunto binário que informa se a fábrica foi aberta ou não



    // Ordena os clientes com base no menor valor pra instalação e armazena para cada instalação
    vector<vector<pair<int, int>>>matriz_ordenacao = custo_de_conexao;
    for(i = 0; i < matriz_ordenacao.size(); i++)
    {
        sort(matriz_ordenacao[i].begin(), matriz_ordenacao[i].end());
        instalacoes[i].ordem_por_custo = matriz_ordenacao[i];
    }


    // Ordenar as penalidades em ordem decrescente de clientes, para usar um algoritmo de busca binária depois
    sort(penalidades.begin(), penalidades.end(), 
    [](const penalidade& a, const penalidade& b){if(a.clientes.first != b.clientes.first)
        {
            return a.clientes.first < b.clientes.first;
        }
        return a.clientes.second < b.clientes.second;
    });


    // Inicia o conjunto de clientes não atendidos
    for(i  = 0; i < matriz_ordenacao.size(); i++)
    {
        instalacoes[i].Y.resize(matriz_ordenacao[i].size(), 0);
        instalacoes[i].atribuicao.resize(matriz_ordenacao[i].size(), 0);
    }
    // Guarda as atribuições
    for(i = 0 ; i < instalacoes.size(); i++)
    {
        for(j = 0; j < instalacoes[i].ordem_por_custo.size(); j++)
        {
            instalacoes[i].atribuicao[instalacoes[i].ordem_por_custo[j].second] = j;
        }
    }


    // Escolha local
    while(D.size() != custo_de_conexao[0].size())
    {
        vector<int> deltinha(instalacoes.size(), 0); // Economia que obtemos ao mudar o cliente de instalação
        vector<pair<int, int>> bestY; // Melhor conjunto de clientes alocados
        int bestI = -1; // Melhor instalação para "abrir"
        float bestphi = numeric_limits<float>::max(); // Inicia bestphi com o maior valor do tipo float
        int penalidade_cliente_queMigra = 0;
        int penalidade_cliente_alocado = 0;

        // Looping das instalacoes
        for(i = 0; i < instalacoes.size(); i++)
        {

            // δ(D, i) = som_j∈D max(0,(d(σ(j), j) − d(i, j))
            instalacoes[i].Dlinha = {};
            for(auto j : D)
            {
                // busca se tem penalidade entre os clientes que desejam migrar
                for(auto k : instalacoes[i].Dlinha)
                {
                    penalidade_cliente_queMigra += buscabinariadepar(penalidades, j, k);
                }

                // busca se tem penalidade entre o cliente que já estão alocados
                for(auto l : D)
                {
                    if(j != l && sigma[j] == i && sigma[l] == i)
                    {
                        penalidade_cliente_alocado += buscabinariadepar(penalidades, j, l);
                    }
                }

                // adicionar penalidade aqui
                economia = custo_de_conexao [sigma[j]][j].first - custo_de_conexao[i][j].first - penalidade_cliente_queMigra + penalidade_cliente_alocado;
                if(economia > 0)
                {
                    instalacoes[i].Dlinha.push_back(j);
                    deltinha[i] += economia;
                }
            }
            // --------------------------------------------------------------------------------


            // Φ(i, Y) := (0_{i∉X} · f_i + ∑_{j∈Y} d(i, j) − δ(D, i)) / |Y|
            custos = 0;
            int penalidade_cliente_alocando = 0;
            int penalidade_cliente_alocado = 0;
            int penalidade_cliente_queMigra = 0;
            vector<pair<int, int>> Y = {};
            float bestphilocal = numeric_limits<float>::max();

            for(j = 0; j < instalacoes[i].ordem_por_custo.size(); j++)
            {
                if(instalacoes[i].Y[j] == 0)
                {
                    Y.push_back(instalacoes[i].ordem_por_custo[j]);
                    // Adicionar um looping aqui para as penalidades


                    // Busca a penalidade entre os clientes que estão sendo alocados
                    for(auto k: Y)
                    {
                        penalidade_cliente_alocando += buscabinariadepar(penalidades, instalacoes[i].ordem_por_custo[j].second, k.second);
                    }

                    // Busca a penalidade entre os clientes que já estão alocados
                    for(auto l: D)
                    {
                        if(sigma[l] == i)
                        {
                            penalidade_cliente_alocado += buscabinariadepar(penalidades, instalacoes[i].ordem_por_custo[j].second, l);
                        }
                    }

                    // Busca a penalidade entre os clientes que migrarão se a instalação for aberta
                    for(auto m: instalacoes[i].Dlinha)
                    {
                        penalidade_cliente_queMigra += buscabinariadepar(penalidades, instalacoes[i].ordem_por_custo[j].second, m);
                    }

                    custos += instalacoes[i].ordem_por_custo[j].first;

                    phi = (O[i] * instalacoes[i].custo_abertura + custos - deltinha[i] + penalidade_cliente_alocando + penalidade_cliente_alocado + penalidade_cliente_queMigra) / (float)Y.size();

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
            for(auto j : instalacoes[bestI].Dlinha)
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

        for(i = 0; i < instalacoes.size(); i++)
        {
            for(auto j: bestY)
            { 
                instalacoes[i].Y[instalacoes[i].atribuicao[j.second]] = 1;
            }
        }
    }

    //printa atribuições
    cout << "Instalacoes:" << endl;
    for(auto i: X)
    {
        cout << i << " ";
    }
    cout << endl;

    cout << "Atribuicoes:" << endl;
    for(auto i: sigma)
    {
        cout << i << " ";
    }
    cout << endl;

    return;
}