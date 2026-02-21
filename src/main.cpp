#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstdlib>
#include "ils/ils.hpp"

using namespace std;

//----------------------------------------------------------------------//
// Função para printar a solução
//----------------------------------------------------------------------//
void printSolution(const Srepresentation& solucao)
{
    //printa atribuições
    cout << "Instalacoes:" << endl;
    for(auto i: solucao.openfacilities)
    {
        cout << i << " ";
    }
    cout << endl;

    cout << "Atribuicoes:" << endl;
    for(auto i: solucao.assignments)
    {
        cout << i << " ";
    }
    cout << endl;

    cout << "Custo Total: " << solucao.totalCost << endl;

}
//----------------------------------------------------------------------//


//----------------------------------------------------------------------//
// Main function
//----------------------------------------------------------------------//
int main(int argc, char *argv[])
{
    // Parametros de tempo e seed padrão
    int tempo = 60;
    int seed = -1;
    int opt;
    // !string instancia -> Verificar depois se é necessário ou não;

    // Processamento dos argumentos de linha de comando
    while((opt = getopt(argc, argv, "T:s:")) != -1) 
    {
        switch(opt) 
        {
            // Processa o tempo limite
            case 'T':
                tempo = atoi(optarg);
                break;
            // Processa a seed para geração de números aleatórios
            case 's':
                seed = atoi(optarg);
                break;
            // Caso de uso inválido
            default:
                cerr << "Uso: " << argv[0] << " [-t tempo] [-s seed] <instancia>\n";
                return 1;
        }
    }

    // Verifica se o arquivo de instância foi fornecido
    if(optind >= argc)
    {
        cerr << "Uso: " << argv[0] << " [-t tempo] [-s seed] <instancia>\n";
        return 1;
    }


    // Abre o arquivo de instância
    ifstream arquivo(argv[optind]);

    // Verifica se o arquivo foi aberto com sucesso
    if(!arquivo.is_open())
    {
        cerr << "Erro ao abrir o arquivo de instancia.\n";
        return 1;
    }


    // Variáveis para armazenar os dados da instância e a solução
    instance instance_data;
    Srepresentation solucao;

    // Conflitos entre clientes (para penalidades)
    vector<vector<pair<int, int>>> conflitos;

    if(!carregarInstancia(arquivo, instance_data)) {
        std::cerr << "Erro ao carregar instancia.\n";
        return 1;
    }

    ILS ils_solver(instance_data, tempo, 100, seed);
    ils_solver.run();

    solucao = ils_solver.getBestSolution();

    printSolution(solucao);
    
    return 0;
}
//----------------------------------------------------------------------//