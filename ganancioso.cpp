#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <fstream>
#include <string>

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
    vector<int> Dlinha; // Conjunto de clientes já atendidos e que podem mudar para nova instalação
    vector<int> atribuicao; // guarda a atribuição dos clientes depois de ordenados
    vector<int> Y; // Clientes que ainda não foram atendidos
};


void greedUFL(vector<instalacao> instalacoes, vector<vector<pair<int, int>>> custo_de_conexao);
int buscabinaria(vector<pair<int, int>>matriz_ordenacao, pair<int, int>key);



int main(int argc, char* argv[])
{
    // Verifica se o arquivo foi passado
    if(argc < 2)
    {
        cerr << "Uso: " << argv[0] << " <arquivo_instancia>" << endl;
        return 1;
    }

    string Nometxt = argv[1];
    ifstream arquivo(Nometxt);

    // Verifica se foi possível abrir o arquivo
    if(!arquivo.is_open())
    {
        cerr << "Erro ao abrir arquivo: " << Nometxt << endl;
        return 1;
    }

    cout << "Arquivo aberto com sucesso" << endl;



    int qtd_instalacoes, qtd_clientes, entradas, i, j, x, in = 0;
    arquivo >> entradas;

    while(in != entradas)
    {
        cout<< "\n" << "Instância :): " << in + 1 << endl;

        arquivo >> qtd_instalacoes;
        arquivo >> qtd_clientes;
        vector<instalacao> instalacoes;
        vector<vector<pair<int, int>>> custo_de_conexao;
        
        // monta a instancia de instalacoes
        for(i = 0; i < qtd_instalacoes; i++)
        {
            arquivo >> x;
            instalacoes.push_back({x, {}, {}, {}, {}});
        }

        // monta a instancia de clientes
        custo_de_conexao.resize(qtd_instalacoes);
        for(i = 0; i < qtd_instalacoes; i++)
        {
            for(j = 0; j < qtd_clientes; j++)
            {
                arquivo >> x;
                custo_de_conexao[i].push_back({x, j});
            }
        }

        greedUFL(instalacoes, custo_de_conexao);

        in++;
    }
    
    return 0;
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
        int bestI; // Melhor instalação para "abrir"
        float bestphi = numeric_limits<float>::max(); // Inicia bestphi com o maior valor do tipo float

        // Looping das instalacoes
        for(i = 0; i < instalacoes.size(); i++)
        {

            // δ(D, i) = som_j∈D max(0,(d(σ(j), j) − d(i, j))
            instalacoes[i].Dlinha = {};
            for(auto j : D)
            {
                economia = custo_de_conexao [sigma[j]][j].first - custo_de_conexao[i][j].first;
                if(economia > 0)
                {
                    instalacoes[i].Dlinha.push_back(j);
                    deltinha[i] += economia;
                }
            }
            // --------------------------------------------------------------------------------


            // Φ(i, Y) := (0_{i∉X} · f_i + ∑_{j∈Y} d(i, j) − δ(D, i)) / |Y|
            custos = 0;
            vector<pair<int, int>> Y = {};
            float bestphilocal = numeric_limits<float>::max();

            for(j = 0; j < instalacoes[i].ordem_por_custo.size(); j++)
            {
                if(instalacoes[i].Y[j] == 0)
                {
                    Y.push_back(instalacoes[i].ordem_por_custo[j]);
                    custos += instalacoes[i].ordem_por_custo[j].first;

                    phi = (O[i] * instalacoes[i].custo_abertura + custos - deltinha[i]) / (float)Y.size();

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

        //Verificar melhor esse final!!!
        // O erro deste algoritmo é esse final
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