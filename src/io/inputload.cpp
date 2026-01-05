#include "inputload.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;

bool carregarInstancia(ifstream &arquivo, instance& inst)
{
    if(!(arquivo >> inst.qtd_instalacoes)) 
    {
        return false;
    }

    arquivo >> inst.qtd_clientes;
    arquivo >> inst.qtd_penalidades;

    inst.instalacoes.clear();
    inst.custo_conexao.clear();
    inst.penalidades_vetor.clear();
    inst.penalidades_grafo.clear();

    // custos de abertura
    for(int i = 0; i < inst.qtd_instalacoes; i++) {
        int x;
        arquivo >> x;
        inst.instalacoes.push_back({x, {}, {}, {}, {}});
    }

    // custos de conexão
    inst.custo_conexao.resize(inst.qtd_instalacoes);
    for(int i = 0; i < inst.qtd_instalacoes; i++)
        for(int j = 0; j < inst.qtd_clientes; j++) {
            int x;
            arquivo >> x;
            inst.custo_conexao[i].push_back({x, j});
        }

    // penalidades
    inst.penalidades_grafo.assign(inst.qtd_clientes, {});
    inst.penalidades_vetor.resize(inst.qtd_penalidades);
    pair<int, int>  penalidade;

    // Monta o grafo de penalidades
    for(int i = 0; i < inst.qtd_penalidades; i++) 
    {
        int c1, c2, custo;
        arquivo >> c1 >> c2 >> custo;
        inst.penalidades_grafo[c1].push_back({c2, custo});
        inst.penalidades_grafo[c2].push_back({c1, custo});
        if(c1 < c2)
            inst.penalidades_vetor[i] = {{c1, c2}, custo};
        else
        {
            inst.penalidades_vetor[i] = {{c2, c1}, custo};
        }
    }

    sort(inst.penalidades_vetor.begin(), inst.penalidades_vetor.end(), [](const auto &a, const auto &b) {
        return a.clientes < b.clientes; 
    });


    return true;
}