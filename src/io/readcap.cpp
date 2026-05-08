#include "readcap.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;

bool readcap(std::ifstream &arquivo, instance &inst)
{
    int lixo;
    double cliente, instalacao;


    arquivo >> inst.qtd_instalacoes;
    arquivo >> inst.qtd_clientes;



    // Lendo as instalações
    for(int i = 0; i < inst.qtd_instalacoes; i++)
    {
        arquivo >> lixo >> instalacao;

        inst.instalacoes.push_back({int(instalacao), {}, {}, {}, {}});
    }


    inst.custo_conexao.resize(inst.qtd_instalacoes);
    for(int i = 0; i < inst.qtd_instalacoes; i++)
    {
        arquivo >> lixo;
        for(int j = 0; j < inst.qtd_clientes; j++)
        {
            arquivo >> cliente;
            inst.custo_conexao[i].push_back({int(cliente), j});
        }
    }

    return true;
}