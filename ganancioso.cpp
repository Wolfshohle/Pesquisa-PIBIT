#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

// Usando uma variável global
using namespace std;

struct penalidade
{
    int i, j; // Par ordenado
    int d; // Custo por penalidade
};

struct instalacao
{
    int custo_abertura; // Custo de abrir a instalação
    vector<pair<int, int>> ordem_por_custo; // Custo da instalação i atender o cliente j em ordem crescente pelo custo

    vector<int> Y; // Clientes que ainda não foram atendidos
};


void greedUFL(vector<instalacao> instalacoes, vector<vector<pair<int, int>>> custo_de_conexao);


int main()
{
    int i;
    
    vector<vector<pair<int, int>>>custo_de_conexao = 
        //(valor, cliente)
        {{{3, 1}, {2, 2}, {7, 3}, {6, 4}}, //F1
        {{4, 1}, {1, 2}, {5, 3}, {8, 4}},  //F2
        {{6, 1}, {3, 2}, {2, 3}, {4, 4}}}; //F3

    vector<instalacao> instalacoes = {
        {10, {}, {}},
        {8, {}, {}},
        {12, {}, {}}
    };

    greedUFL(instalacoes, custo_de_conexao);
}

// Algoritmo ganancioso para o problema de localização de instalações
void greedUFL(vector<instalacao> instalacoes, vector<vector<pair<int, int>>> custo_de_conexao)
{
    // variáveis gerais
    int i, j, custos, economia;
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


    // Inicia o conjunto de clientes não atendidos
    for(i  = 0; i < matriz_ordenacao.size(); i++)
    {
        instalacoes[i].Y.resize(matriz_ordenacao[i].size(), 0);
    }


    // Escolha local
    while(D.size() != custo_de_conexao.size())
    {
        vector<int> deltinha(instalacoes.size(), 0); // Economia que obtemos ao mudar o cliente de instalação
        vector<pair<int, int>> bestY; // Melhor conjunto de clientes alocados
        vector<int> Dlinha; // Conjunto de clientes já atendidos e que podem mudar para nova instalação
        int bestI; // Melhor instalação para "abrir"
        float bestphi = numeric_limits<float>::max(); // Inicia bestphi com o maior valor do tipo float

        // Looping das instalacoes
        for(i = 0; i < instalacoes.size(); i++)
        {

            // δ(D, i) = som_j∈D max(0,(d(σ(j), j) − d(i, j))
            for(auto j : D)
            {
                economia = custo_de_conexao [sigma[j]][j].first - custo_de_conexao[i][j].first;
                if(economia > 0)
                {
                    Dlinha.push_back(j);
                    deltinha[i] += economia;
                }
            }
            // --------------------------------------------------------------------------------


            // Φ(i, Y) := (0_{i∉X} · f_i + ∑_{j∈Y} d(i, j) − δ(D, i)) / |Y|
            custos = 0;
            vector<pair<int, int>> Y = {};

            for(j = 0; j < instalacoes[i].ordem_por_custo.size(); j++)
            {
                if(instalacoes[i].Y[j] == 0)
                {
                    Y.push_back(instalacoes[i].ordem_por_custo[j]);
                    custos += instalacoes[i].ordem_por_custo[j].first;

                    phi = (O[i] * instalacoes[i].custo_abertura + custos - deltinha[i]) / (float)Y.size();

                    if(phi < bestphi)
                    {
                        bestphi = phi;
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
            for(auto j : Dlinha)
            {
                sigma[j] = bestI;
            }
            O[bestI] = 0;
        }

        //Verificar melhor esse final!!!
        for(auto j: bestY)
        {

        }
        
    }

    return;
}

// Busca binária de pares
int buscabinária(vector<pair<int, int>>matriz_ordenacao, pair<int, int>key)
{
    int esquerda = 0;
    int direita = matriz_ordenacao.size() - 1;
    int meio;

    while(esquerda <= direita)
    {
        meio = (direita + esquerda) / 2;
        if(matriz_ordenacao[meio].first == key.first)
        {
            return meio;
        }
        else if(matriz_ordenacao[meio].first > key.first)
        {
            direita = meio - 1;
        }
        else
        {
            esquerda = meio + 1;
        }
    }

    return -1;
}