#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

struct instalacao
{
    int custo_abertura; // Custo de abrir a instalação
    vector<pair<int, int>> ordem_por_custo; // Custo da instalação i atender o cliente j em ordem crescente pelo custo
    vector<int> Dlinha; // Conjunto de clientes já atendidos e que podem mudar para nova instalação
    vector<int> atribuicao; // guarda a atribuição dos clientes depois de ordenados
    vector<int> Y; // Clientes que ainda não foram atendidos
};


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

    vector<instalacao> instalacoes;
    vector<vector<pair<int, int>>> custo_de_conexao;



    int qtd_instalacoes, qtd_clientes, entradas, i, j, x, in = 0;
    arquivo >> entradas;

    while(in != entradas)
    {
        arquivo >> qtd_instalacoes;
        arquivo >> qtd_clientes;
        
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

        // gulosinho

        in++;
    }
    
    return 0;
}