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
#include <cmath>
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
//! Terminar essa parte
void generate_penality_mode1(instance &inst, instance &inst2, double p, unsigned int seed)
{
    int n = inst.qtd_clientes;

    // Calcula o máximo de arestas do grafo
    int max_penalitys = (n*(n - 1)) / 2;
    
    
    vector<bool> inHub(n, false);

    // Seleciona 20% dos clientes para serem hubs de conflitos
    int num_hubs = (int)(0.2 * n);

    if(num_hubs < 1)
    {
        num_hubs = 1;
    }

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

    // cria uma pseudo estrututra de dados pra selecionar os pares, vulgo, preguiça de criar mais uma estrutura
    vector<pair<int, int>> pares_hub;
    for(int i = 0; i < n; i++)
    {
        for(int j = i + 1; j < n; j++)
        {
            pares_hub.push_back(make_pair(i, j));
        }
    }

    int qtd_alvo = (int)min(max_penalitys, int(round(max_penalitys * p)));

    // Processo de sort através de uma urna
    for(int k = 0; k < qtd_alvo; k++)
    {
        vector<int> urna;
        for(int i = 0; i < pares_hub.size(); i++)
        {
            int qtd_biletes;
            // Adiciona o cliente hub 3 vezes na urna
            if(inHub[pares_hub[i].first] || inHub[pares_hub[i].second])
            {
                urna.push_back(i);
                urna.push_back(i);
                urna.push_back(i);
            }
            // Adiciona o cliente sem hub2 vezes na urna
            else
            {
                urna.push_back(i);
            }
        }

        // Sorteia um valor da urna
        int posicao_sorteada = rand() % urna.size();
        // Pega o indicie dos pares sorteados
        int indice_escolhido = urna[posicao_sorteada];
        // Gera um custo
        int custo = rand() % 60001 + 10000;
        // Cria uma variável de penalidade
        penalidade add_penalidade;
        add_penalidade.clientes = pares_hub[indice_escolhido];
        add_penalidade.custo = custo;
        // Adiciona a penalidade e remove o par
        inst.penalidades_vetor.push_back(add_penalidade);
        pares_hub.erase(pares_hub.begin() + indice_escolhido);
    }


    inst.qtd_penalidades = inst.penalidades_vetor.size();


    vector<pair<int, int>> pares_continuos;

    for(int i = 0; i < n; i++)
    {
        for(int j = i + 1; j < n; j++)
        {
            pares_continuos.push_back(make_pair(i, j));
        }
    }

    shuffle(pares_continuos.begin(), pares_continuos.end(), rng);

    for(int i = 0; i < qtd_alvo; i++)
    {
        penalidade add_penalidae2;

        add_penalidae2.clientes = pares_continuos[i];
        add_penalidae2.custo = rand() % 60001 + 10000;

        inst2.penalidades_vetor.push_back(add_penalidae2);
    }

    inst2.qtd_penalidades = inst2.penalidades_vetor.size();

    return;
}

void to_write(instance &inst, int seed, int mode, double p, string descricao)
{
    int percent = static_cast<int>(p * 100);
    int k = 1;
    string base = "Data/raw/" + to_string(inst.qtd_clientes) + "_" + "P" + to_string(percent) + "_" + "M" + to_string(mode) + "_" + descricao;
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
    seedfile << inst.qtd_clientes << "_" << "P" << to_string(percent) << "_" << "M" << to_string(mode) << "_" << descricao << "_" << k << ": " << seed << endl;
    seedfile.close();

    out.close();
    // --------------------------------------------------

    cout << "Instância gerada com sucesso: " << inst.qtd_clientes << "_" << "P" << to_string(percent) << "_" << "M" << to_string(mode) << "_" << descricao << "_" << k << ": " << seed << endl;
    
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
    while((opt = getopt(argc, argv, "p:m:s:")) != -1) 
    {
        switch(opt) 
        {
            case 'p':
                p = atof(optarg);
                break;
            case 'm':
                mode = atoi(optarg);
                break;
            case 's':
                seed = atoi(optarg);
                break;
            default:
                cerr << "Uso: " << argv[0] << " [-p probabilidade/porcentagem] [-m Modo de geração] [-s seed]\n";
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
    instance inst2 = inst;
    // --------------------------------------------------


    //! MODIFICAR AQUI MAIS TARDE
    // --------------------------------------------------
    // Gera as penalidades entre os clientes com base na probabilidade p
    // --------------------------------------------------
    if(mode == 0)
    {
        generate_penality_mode0(inst, p);
        to_write(inst, seed, mode, p, "Continuo");
    }
    else
    {
        generate_penality_mode1(inst, inst2, p, seed);
        to_write(inst, seed, mode, p, "HUB");
        to_write(inst2, seed, mode, p, "Continuo");
    }
    // --------------------------------------------------
    return 0;
}