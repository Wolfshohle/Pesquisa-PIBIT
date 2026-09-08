#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstdlib>
#include "ils/ils.hpp"
#include "Asserts/automatizedtests.hpp"

using namespace std;


//----------------------------------------------------------------------//
// Main function
//----------------------------------------------------------------------//
int main(int argc, char *argv[])
{
    // Parametros de tempo e seed padrão
    int tempo = 60;
    int seed = 1;
    int pertubacao = 5;
    int opt;
    // Constantes de calibração
    int rclf = 2;
    int rcls = 10;
    int c2 = 5;
    int c3 = 1;
    int c4 = 3;
    // !string instancia -> Verificar depois se é necessário ou não;

    // Processamento dos argumentos de linha de comando
    while((opt = getopt(argc, argv, "T:s:P:f:z:c:p:d:")) != -1) 
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
            // Processa o tamanho da pertubação
            case 'P':
                pertubacao = atoi(optarg);
                break;
            // Processa a constante rclf
            case 'f':
                rclf = atoi(optarg);
                break;
            // Processa a constante rcls
            case 'z':
                rcls = atoi(optarg);
                break;
            // Processa a constante c2
            case 'c':
                c2 = atoi(optarg);
                break;
            // Processa a constante c3
            case 'p':
                c3 = atoi(optarg);
                break;
            // Processa a constante c4
            case 'd':
                c4 = atoi(optarg);
                break;
            // Caso de uso inválido
            default:
                cerr << "Uso: " << argv[0] << " [-T tempo] [-s seed] [-P pertubacao] [-f restricted candidate list factor] [-z restricted candidate list size] [-c accept const 1] [-p accept const 2] [-d accept const 3] <instancia>\n";
                return 1;
        }
    }

    // Verifica se o arquivo de instância foi fornecido
    if(optind >= argc)
    {
        cerr << "Uso: " << argv[0] << " [-T tempo] [-s seed] [-P pertubacao] [-f restricted candidate list factor] [-z restricted candidate list size] [-c accept const 1] [-p accept const 2] [-d accept const 3] <instancia>\n";
                return 1;
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

    cout << "Instância: " << argv[optind] << endl;
    cout << "Tempo limite: " << tempo << " segundos" << endl;
    cout << "Seed: " << seed << endl;
    cout << "Tamanho da pertubação: " << pertubacao << endl;

    // Variáveis para armazenar os dados da instância e a solução
    instance instance_data;
    Srepresentation solucao;

    // Conflitos entre clientes (para penalidades)
    vector<vector<pair<int, int>>> conflitos;

    if(!carregarInstancia(arquivo, instance_data)) {
        std::cerr << "Erro ao carregar instancia.\n";
        return 1;
    }

    ILS ils_solver(instance_data, tempo, pertubacao, seed, rclf, rcls, c2, c3, c4);
    ils_solver.run();

    solucao = ils_solver.getglobal_solution();
    
    asserts assert_checker;
    assert_checker.checkresult(solucao, instance_data);

    return 0;
}
//----------------------------------------------------------------------//