#include <iostream>
#include <fstream>
#include "io/input.hpp"
#include "greedy/greedy.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    if(argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <instancia>\n";
        return 1;
    }

    ifstream arquivo(argv[1]);
    if(!arquivo.is_open()) {
        std::cerr << "Erro ao abrir arquivo.\n";
        return 1;
    }

    int entradas;
    arquivo >> entradas;

    int qtd_inst, qtd_cli, qtd_pen;
    vector<instalacao> instalacoes;
    vector<vector<pair<int,int>>> custo_conexao;
    vector<penalidade> penalidades;

    for(int in = 0; in < entradas; in++) {
        cout << "\n=== INSTÂNCIA " << in+1 << " ===\n";

        carregarInstancia(arquivo, qtd_inst, qtd_cli, qtd_pen,
                          instalacoes, custo_conexao, penalidades);

        greedUFL(instalacoes, custo_conexao, penalidades);
    }

    return 0;
}
