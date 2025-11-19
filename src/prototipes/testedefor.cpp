#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

/*Foi mudado o Dlinha, foi adicionado a estrutura de dados de instalação e foi modificado o looping de calculo do deltinha*/

using namespace std;

struct instalacao
{
    int custo_abertura; // Custo de abrir a instalação
    vector<pair<int, int>> ordem_por_custo; // Custo da instalação i atender o cliente j em ordem crescente pelo custo
    vector<int> Dlinha; // Conjunto de clientes já atendidos e que podem mudar para nova instalação
    vector<int> atribuicao; // guarda a atribuição dos clientes depois de ordenados
    vector<int> Y; // Clientes que ainda não foram atendidos
};

// Busca binária de pares
int buscabinária(vector<pair<int, int>>matriz_ordenacao, pair<int, int>key)
{
    int esquerda = 0;
    int direita = matriz_ordenacao.size() - 1;
    int meio;

    while(esquerda <= direita)
    {
        meio = (direita + esquerda) / 2;
        if(matriz_ordenacao[meio] == key)
        {
            return meio;
        }
        else if(matriz_ordenacao[meio] > key)
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

int main()
{
    // variáveis gerais
    int i, j, custos, economia, bestI;
    float phi;
    float bestphi = numeric_limits<float>::max(); // Inicia bestphi com o maior valor do tipo float

    vector<vector<pair<int, int>>> custo_de_conexao =
    {
        // (custo, cliente)
        {{3,0}, {2,1}, {7,2}, {6,3}},   // F1 → barata, mas ruim pros clientes mais distantes
        {{4,0}, {1,1}, {5,2}, {8,3}},    // F2 → boa pra clientes centrais (1–3)
        {{6,0}, {3,1}, {2,2}, {4,3}} // F5 → alternativa pra primeiros clientes
    };

    vector<instalacao> instalacoes = {
        {10, {}, {}, {}, {}},   // F1 → barata
        {8, {}, {}, {}, {}},   // F2 → custo moderado
        {12, {}, {}, {}, {}},   // F3 → custo médio
    };



    vector<int> X;
    vector<int> D; // Clientes atendidos
    vector<int> sigma = {-1, -1, -1, -1,}; // Atribuição de clientes a instalação
    vector<int> deltinha = {0, 0, 0, 0}; // Economia que obtemos ao mudar o cliente de instalação
    vector<pair<int, int>> bestY;
    vector<int> O(instalacoes.size(), 1); // Conjunto binário que informa se a fábrica foi aberta ou não
    /*
    O[2] = 0;
    D = {3, 2, 4, 5};
    X ={2};*/
    
    // Ordena os clientes por custo
    vector<vector<pair<int, int>>>matriz_ordenacao = custo_de_conexao;
    for(i = 0; i < matriz_ordenacao.size(); i++)
    {
        sort(matriz_ordenacao[i].begin(), matriz_ordenacao[i].end());
        instalacoes[i].ordem_por_custo = matriz_ordenacao[i];
    }

    cout << "Matriz ordenada:" << endl;
    for(auto i : instalacoes)
    {
        for(auto j : i.ordem_por_custo)
        {
            cout << "(" << j.first << " " << j.second << ")" << " ";
        }
        cout << endl;
    }

    // Inicia o conjunto de clientes não atendidos
    for(i  = 0; i < matriz_ordenacao.size(); i++)
    {
        instalacoes[i].Y.resize(matriz_ordenacao[i].size(), 0);
        instalacoes[i].atribuicao.resize(matriz_ordenacao[i].size(), 0);
    }

    for(i = 0 ; i < instalacoes.size(); i++)
    {
        for(j = 0; j < instalacoes[i].ordem_por_custo.size(); j++)
        {
            instalacoes[i].atribuicao[instalacoes[i].ordem_por_custo[j].second] = j;
        }
    }

    cout << "Atribuicoes:" << endl;
    for(auto i : instalacoes)
    {
        for(auto j : i.atribuicao)
        {
            cout << j << " ";
        }
        cout << endl;
    }
    cout << endl;
    /*
    instalacoes[0].Y = {0, 0, 1, 1, 1, 1};
    instalacoes[1].Y = {1, 0, 1, 1, 0, 1};
    instalacoes[2].Y = {1, 1, 1, 1, 0, 0};
    instalacoes[3].Y = {1, 1, 1, 1, 0, 0};
    instalacoes[4].Y = {0, 0, 1, 1, 1, 1};*/
    
    // teste do deltinha
    for(i = 0; i < 3; i++)
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
    }

    for(i = 0; i < instalacoes.size(); i++)
    {
        cout << "Deltinha[" << i << "]: " << deltinha[i] << " ";
    }    
    cout << endl;

    cout << "Dlinha: "; 
    for(auto i : instalacoes)
    {
        for(auto j : i.Dlinha)
        {
            cout << j << " ";
        }
    }
    cout << endl;
    // -------------------------------------


    /*Teste o phi
    Algumas considerações:
        BestPhi, BestY e BestI tem que ser iniciados no while
        custos e Y tem que ser iniciado antes do for do phi
        Tentar melhorare essa checagem dos elementos de instalacoes.Y (Linear é foda...0)*/
    for(i = 0; i < 3; i++)
    {
        custos = 0;
        vector<pair<int, int>> Y = {};
        float bestphilocal = numeric_limits<float>::max();
        // Φ(i, Y) := (0_{i∉X} · f_i + ∑_{j∈Y} d(i, j) − δ(D, i)) / |Y|
        for(j = 0; j < instalacoes[i].ordem_por_custo.size(); j++)
        {
            if(instalacoes[i].Y[j] == 0)
            {
                Y.push_back(instalacoes[i].ordem_por_custo[j]);
                custos += instalacoes[i].ordem_por_custo[j].first;

                phi = (O[i] * instalacoes[i].custo_abertura + custos - deltinha[i]) / (float)Y.size();
                cout << "Bestphilocal: " << bestphilocal << endl;

                if(phi >= bestphilocal)
                {
                    cout << phi << " é maior ou igual a " << bestphilocal << endl;
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
    }

    cout << "BestI: " << bestI << " " << "Bestphi: " << " " << bestphi << endl;
    
    for(auto j: bestY)
    {
        cout << j.first << " " << j.second << endl;
    }
    
    // --------------------------------------


    if(O[bestI] == 1)
    {
        X.push_back(bestI);
        for(auto j : bestY)
        {
            sigma[j.second] = bestI;
        }
        for(auto j : instalacoes[bestI].Dlinha)
        {
            cout << "O cliente " << j << " trocou" << endl;
            sigma[j] = bestI;
        }
        
        O[bestI] = 0;
    }

    for(auto j: bestY)
    {
        D.push_back(j.second);
    }

    //Verificar melhor esse final!!!
    for(i = 0; i < instalacoes.size(); i++)
    {
        for(auto j: bestY)
        { 
            instalacoes[i].Y[instalacoes[i].atribuicao[j.second]] = 1;
            //instalacoes[i].Y[indice] = 1;
        }
    }

    cout << "Instalacoes:" << endl;;
    for(auto i: X)
    {
        cout << i << " ";
    }
    cout << endl;
    
    cout << "Sigma:" << endl;
    for(auto i: sigma)
    {
        cout << i << " ";
    }
    cout << endl;

    cout << "Clientes:" << endl;
    for(auto i: D)
    {
        cout << i << " ";
    }
    cout << endl;

    cout << "Instalacoes:" << endl;
    for(auto i: instalacoes)
    {
        cout << "Y: ";
        for(auto j : i.Y)
        {
            cout << j << " ";
        }
        cout << endl;
    }
}