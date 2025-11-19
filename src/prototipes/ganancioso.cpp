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
    pair<int, int> clientes; // Guarda os clientes conflitantes
    int custo; // Guarda a penalidade do cliente atual 
};

struct instalacao
{
    int custo_abertura; // Custo de abrir a instalação
    vector<pair<int, int>> ordem_por_custo; // Custo da instalação i atender o cliente j em ordem crescente pelo custo
    vector<int> Dlinha; // Conjunto de clientes já atendidos e que podem mudar para nova instalação
    vector<int> atribuicao; // guarda a atribuição dos clientes depois de ordenados
    vector<int> Y; // Clientes que ainda não foram atendidos
};


void greedUFL(vector<instalacao>& instalacoes, vector<vector<pair<int, int>>>& custo_de_conexao, vector<penalidade>& penalidades);
int buscabinariadepar(vector<penalidade>& penalidades, int c1, int c2);


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



    int qtd_instalacoes, qtd_clientes, qtd_penalidades, entradas, i, j, x, in = 0, c1, c2, custo;
    arquivo >> entradas;

    while(in != entradas)
    {
        cout<< "\n" << "Instância: " << in + 1 << endl;

        arquivo >> qtd_instalacoes;
        arquivo >> qtd_clientes;
        arquivo >> qtd_penalidades;

        vector<instalacao> instalacoes;
        vector<vector<pair<int, int>>> custo_de_conexao;
        vector<penalidade> penalidades;
        

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

        
        // monta as penalidades
        for(i = 0; i < qtd_penalidades; i++)
        {
            arquivo >> c1 >> c2 >> custo;
            if(c1 < c2)
            {
                penalidades.push_back({{c1, c2}, {custo}});
            }
            else
            {
                penalidades.push_back({{c2, c1}, {custo}});
            }
        }

        greedUFL(instalacoes, custo_de_conexao, penalidades);

        in++;
    }
    
    return 0;
}

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

    for(i = 0; i < penalidades.size(); i++)
    {
        cout << "Clientes: (" << penalidades[i].clientes.first << ", " << penalidades[i].clientes.second << ") - Custo: " << penalidades[i].custo << endl;
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
        cout << "Melhor instalação a abrir: " << bestI << " com φ = " << bestphi << endl;
        cout << "Clientes alocados: ";
        for(auto c: bestY)
        {
            cout << c.second << " ";
        }
        cout << endl;
        cout << "clientes que migraram: ";
        for(auto c: instalacoes[bestI].Dlinha)
        {
            cout << c << " ";
        }
        cout << endl;
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

int calculodepenalidade(int c1, int c2, vector<penalidade>& penalidades)
{
    for(auto p: penalidades)
    {
        if((p.clientes.first == c1 && p.clientes.second == c2) || (p.clientes.first == c2 && p.clientes.second == c1))
        {
            return p.custo;
        }
    }
    
    return 0;
}

int buscabinariadepar(vector<penalidade>& penalidades, int c1, int c2)
{
    int left = 0;
    int right = penalidades.size() - 1;

    while(left <= right)
    {
        int mid = left + (right - left) / 2;

        if(penalidades[mid].clientes.first == c1 && penalidades[mid].clientes.second == c2 || penalidades[mid].clientes.first == c2 && penalidades[mid].clientes.second == c1)
        {
            return penalidades[mid].custo;
        }
        else if(penalidades[mid].clientes < make_pair(c1, c2))
        {
            left = mid + 1;
        }
        else
        {
            right = mid - 1;
        }
    }

    return 0;
}