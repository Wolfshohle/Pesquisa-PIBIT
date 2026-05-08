// Penality-Density instance generator
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <algorithm>
#include "../../src/io/readcap.hpp"

using namespace std;

// Modo de geraçao normal
void generate_penality_mode0(instance &inst, double p)
{
    for(int i = 0; i < inst.qtd_clientes; i++)
    {
        for(int j = i + 1; j < inst.qtd_clientes; j++)
        {
            double rand_val = rand() / (double)RAND_MAX; // Gera um número aleatório entre 0 e 1
            
            if(rand_val < p)
            {
                penalidade add_penalidade;
                add_penalidade.clientes = make_pair(i, j);
                add_penalidade.custo = rand() % 60000 + 10000;
                inst.penalidades_vetor.push_back(add_penalidade);
            }
        }
    }

    inst.qtd_penalidades = inst.penalidades_vetor.size();

    return;
}

// Modo de geração com HUBS
void generate_penality_mode1(instance &inst, double p, unsigned int seed)
{
    int n = inst.qtd_clientes;
    int max_edgers = (n*(n - 1)) / 2;
    int max_of_penalitys = max_edgers * p;
    vector<bool> inHub(n, false);

    // Seleciona 20% dos clientes para serem hubs de conflitos
    int num_hubs = (int)(0.2 * n);

    vector<int>indice(n);

    for(int i = 0; i < n; i++)
    {
        indice[i] = i;
    }

    // Seleciona aleatoriamente os clientes que serão HUBS de conflitos
    mt19937 rng(seed);
    shuffle(indice.begin(), indice.end(), rng);

    for(int i = 0; i < num_hubs; i++)
    {
        inHub[indice[i]] = true;
    }

    // Probabilidade ajustada para o caso de clientes como HUBS
    double p_low = max(0.0, p * 0.7);
    double p_hig = min(1.0, p * 1.5);

    // Variáveis auxiliares
    double prob_edge;
    double rand_val;

    for(int i = 0; i < n; i++)
    {
        for(int j = i + 1; j < n; j++)
        {

            if(inHub[i] || inHub[j])
            {
                prob_edge = p_hig;
            }
            else
            {
                prob_edge = p_low;
            }

            rand_val = rand() / double(RAND_MAX);

            if(rand_val < prob_edge)
            {
                penalidade add_penalidade;
                add_penalidade.clientes = make_pair(i, j);
                add_penalidade.custo = rand() % 60000 + 10000;
                inst.penalidades_vetor.push_back(add_penalidade);
            }
        }
    }

    inst.qtd_penalidades = inst.penalidades_vetor.size();

    return;
}

int main(int argc, char *argv[])
{
    int opt;
    unsigned int seed = static_cast<unsigned int>(time(0));
    double p = 0.1;
    int mode = 0;

    // --------------------------------------------------
    // Processamento dos argumentos de linha de comando
    // --------------------------------------------------
    while((opt = getopt(argc, argv, "p:m:")) != -1) 
    {
        switch(opt) 
        {
            case 'p':
                p = atof(optarg);
                break;
            case 'm':
                mode = atoi(optarg);
                break;
            default:
                cerr << "Uso: " << argv[0] << " [-p probabilidade] [-m Modo de geração]\n";
                return 1;
        }
    }
    // --------------------------------------------------


    // --------------------------------------------------
    // Define a seed para geração de números aleatórios
    // --------------------------------------------------
    srand(seed);
    // --------------------------------------------------


    // --------------------------------------------------
    // Carrega a instância base
    // --------------------------------------------------
    ifstream instancia("Data/examples/cap134.txt");

    if(!instancia.is_open())
    {
        cerr << "Erro ao abrir arquivo\n";
        return 1;
        
    }

    // Carrega a instância do arquivo
    instance inst;
    readcap(instancia, inst);
    // --------------------------------------------------


    //! MODIFICAR AQUI MAIS TARDE
    // --------------------------------------------------
    // Gera as penalidades entre os clientes com base na probabilidade p
    // --------------------------------------------------
    if(mode == 0)
    {
        generate_penality_mode0(inst, p);
    }
    else
    {
        generate_penality_mode1(inst, p, seed);
    }
    // --------------------------------------------------

    //! MODIFICAR A GERAÇÃO DO NOME DOS ARQUIVOS DE INSTÂNCIAS
    // --------------------------------------------------
    // Salva a nova instância com as penalidades geradas em um novo arquivo
    // --------------------------------------------------
    /*int percent = static_cast<int>(p * 100);
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
    for(int i = 0; i < inst.qtd_instalacoes; i++)
    {
        if(i+1 < inst.qtd_instalacoes)
            out << inst.instalacoes[i].custo_abertura << " ";
        else
        {
            out << inst.instalacoes[i].custo_abertura << "\n";
        }
    }

    // Escreve os custos de conexão
    for(int i = 0; i < inst.qtd_instalacoes; i++)
    {
        for(int j = 0; j < inst.qtd_clientes; j++)
        {
            if(j+1 < inst.qtd_clientes)
                out << inst.custo_conexao[i][j].first << " ";
            else
            {
                out << inst.custo_conexao[i][j].first << "\n\n";
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
    // --------------------------------------------------

    cout << "Instância gerada com sucesso: " << inst.qtd_clientes << "_" <<percent << k << ".txt" << endl;
    */
    return 0;
}