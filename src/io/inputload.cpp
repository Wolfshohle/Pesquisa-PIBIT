#include "inputload.hpp"
#include <fstream>
#include <iostream>

using namespace std;

bool carregarInstancia(ifstream &arquivo,
                       int &qtd_inst,
                       int &qtd_cli,
                       int &qtd_pen,
                       vector<instalacao> &instalacoes,
                       vector<vector<pair<int,int>>> &custo_conexao,
                       vector<penalidade> &penalidades)
{
    if(!(arquivo >> qtd_inst)) return false;  // acabou o arquivo

    arquivo >> qtd_cli;
    arquivo >> qtd_pen;

    instalacoes.clear();
    custo_conexao.clear();
    penalidades.clear();

    // custos de abertura
    for(int i = 0; i < qtd_inst; i++) {
        int x;
        arquivo >> x;
        instalacoes.push_back({x, {}, {}, {}, {}});
    }

    // custos de conexão
    custo_conexao.resize(qtd_inst);
    for(int i = 0; i < qtd_inst; i++)
        for(int j = 0; j < qtd_cli; j++) {
            int x;
            arquivo >> x;
            custo_conexao[i].push_back({x, j});
        }

    // penalidades
    for(int i = 0; i < qtd_pen; i++) {
        int c1, c2, custo;
        arquivo >> c1 >> c2 >> custo;
        if(c1 > c2) std::swap(c1, c2);
        penalidades.push_back({{c1, c2}, custo});
    }

    return true;
}