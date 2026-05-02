// Penality-Density instance generator
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include "../../src/io/inputload.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    int opt;
    unsigned int seed = static_cast<unsigned int>(time(0));
    double p = 0.1;

    // Processamento dos argumentos de linha de comando
    while((opt = getopt(argc, argv, "p:")) != -1) 
    {
        switch(opt) 
        {
            case 'p':
                p = atof(optarg);
                break;
            default:
                cerr << "Uso: " << argv[0] << " [-p probabilidade]\n";
                return 1;
        }
    }

    srand(seed); // Define a seed para geração de números aleatórios

    // Carrega a instância base
    ifstream instancia("Data/examples/InstanceBase.txt");

    if(!instancia.is_open())
    {
        cerr << "Erro ao abrir arquivo\n";
        return 1;
        
    }

    instance inst;

    // p é a probabilidade de gerar uma penalidade entre dois clientes
    double rand_val;
    int i, j;

    // Carrega a instância do arquivo
    carregarInstancia(instancia, inst);


    // Gera as penalidades entre os clientes com base na probabilidade p
    for(i = 0; i < inst.qtd_clientes; i++)
    {
        for(j = i + 1; j < inst.qtd_clientes; j++)
        {
            rand_val = rand() / (double)RAND_MAX; // Gera um número aleatório entre 0 e 1
            
            if(rand_val < p)
            {
                penalidade add_penalidade;
                add_penalidade.clientes = make_pair(i, j);
                add_penalidade.custo = rand() % 7500 + 1; // Gera um custo aleatório entre 1 e 7500
                inst.penalidades_vetor.push_back(add_penalidade);
            }
        }
    }

    inst.qtd_penalidades = inst.penalidades_vetor.size();

    // Salva a nova instância com as penalidades geradas em um novo arquivo
    int percent = static_cast<int>(p * 100);
    int k = 1;
    string base = "Data/raw/" + to_string(inst.qtd_clientes) + "_" + "P" + to_string(percent);
    string nome;

    // Gera um nome de arquivo único para evitar sobrescrever arquivos existentes
    do
    {
        nome = base + "_" + to_string(k) + ".txt";
        k++;
    }
    while(filesystem::exists(nome)); // Verifica se o arquivo já existe para evitar sobrescrever
    
    ofstream out(nome);

    out << inst.qtd_instalacoes << " " << inst.qtd_clientes << " " << inst.qtd_penalidades << "\n";

    // Escreve as instalações
    for(i = 0; i < inst.qtd_instalacoes; i++)
    {
        if(i+1 < inst.qtd_instalacoes)
            out << inst.instalacoes[i].custo_abertura << " ";
        else
        {
            out << inst.instalacoes[i].custo_abertura << "\n";
        }
    }

    // Escreve os custos de conexão
    for(i = 0; i < inst.qtd_instalacoes; i++)
    {
        for(j = 0; j < inst.qtd_clientes; j++)
        {
            if(j+1 < inst.qtd_clientes)
                out << inst.custo_conexao[i][j].first << " ";
            else
            {
                out << inst.custo_conexao[i][j].first << "\n";
            }
        }
    }

    // Escreve as penalidades
    for(const auto& penalidade : inst.penalidades_vetor)
    {
        out << penalidade.clientes.first << " " << penalidade.clientes.second << " " << penalidade.custo << "\n";
    }

    ofstream seedfile("Data/raw/seeds/seed.txt", ios::app);
    if(!seedfile.is_open())
    {
        cout << "Erro ao abrir o arquivo de seed!" << endl;
    }

    k= k - 1;
    seedfile << inst.qtd_clientes << "_" << to_string(percent) << "_" << k << ": " << seed << endl;
    seedfile.close();

    out.close();

    cout << "Instância gerada com sucesso: " << inst.qtd_clientes << "_" <<percent << k << ".txt" << endl;

    return 0;
}