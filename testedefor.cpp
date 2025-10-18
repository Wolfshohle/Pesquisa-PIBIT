#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

using namespace std;

struct instalacao
{
    int custo_abertura; // Custo de abrir a instalação
    vector<pair<int, int>> ordem_por_custo; // Custo da instalação i atender o cliente j em ordem crescente pelo custo
    vector<int> Y; // Clientes que ainda não foram atendidos
};

int main()
{
    // variáveis gerais
    int i, j, custos, economia, bestI;
    float phi;
    float bestphi = numeric_limits<float>::max(); // Inicia bestphi com o maior valor do tipo float

    vector<vector<pair<int, int>>>custo_de_conexao = 
        //(valor, cliente)
        {{{3, 0}, {2, 1}, {7, 2}, {6, 3}}, //F1
        {{4, 0}, {1, 1}, {5, 2}, {8, 3}},  //F2
        {{6, 0}, {3, 1}, {2, 2}, {4, 3}}}; //F3

    vector<instalacao> instalacoes = {
        {10, {}, {}},
        {8, {}, {}},
        {12, {}, {}}
    };
   
    vector<int> D = {1, 2, 0}; // Clientes atendidos
    vector<int> sigma = {1, 1, 1, -1}; // Atribuição de clientes a instalação
    vector<int> deltinha = {0, 0, 0}; // Economia que obtemos ao mudar o cliente de instalação
    vector<int> Dlinha; // Conjunto de clientes já atendidos e que podem mudar para nova instalação
    vector<pair<int, int>> bestY;
    vector<int> O(instalacoes.size(), 1); // Conjunto binário que informa se a fábrica foi aberta ou não
    
    // Ordena os clientes por custo
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
    

    // teste do deltinha
    for(i = 0; i < 3; i++)
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
    }

    cout << "Deltinha[0] = " << deltinha[0] << " " << "Deltinha[1] = " << deltinha[1] << " " << "Deltinha[2] = " << deltinha[2] << endl;
    cout << "Dlinha: "; 
    for(auto j : Dlinha)
    {
        cout << j << " ";
    }
    cout << endl;
    // -------------------------------------


    /*Teste o phi
    Algumas considerações:
        BestPhi, BestY e BestI tem que ser iniciados no while
        custos e Y tem que ser iniciado antes do for do phi
        Tentar melhorare essa checagem dos elementos de instalacoes.Y (Linear é foda...0)*/
    /*for(i = 0; i < 3; i++)
    {
        custos = 0;
        vector<pair<int, int>> Y = {};
        // Φ(i, Y) := (0_{i∉X} · f_i + ∑_{j∈Y} d(i, j) − δ(D, i)) / |Y|
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
    }

    cout << "BestI: " << bestI << " " << "Bestphi: " << " " << bestphi << endl;
    
    for(auto j: bestY)
    {
        cout << j.first << " " << j.second << endl;
    }*/
    
    // --------------------------------------

}