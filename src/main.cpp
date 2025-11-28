#include <iostream>
#include <fstream>
#include "io/inputload.hpp"
#include "structures/instance.hpp"
#include "utils/calculocusto.hpp"
#include "greedy/greedy.hpp"
#include "localsearch/localsearch.hpp"

using namespace std;

void printSolution(const Srepresentation& solucao, const vector<vector<pair<int, int>>>& conflitos)
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
    instance instance_data;
    Srepresentation solucao;

    vector<vector<pair<int, int>>> conflitos;

    for(int in = 0; in < entradas; in++) {
        cout << "\n=== INSTÂNCIA DP BARULHO " << in+1 << " ===\n";

        carregarInstancia(arquivo, qtd_inst, qtd_cli, qtd_pen,
                          instalacoes, custo_conexao, penalidades);

        // Prepara dados da instância
        instance_data = {qtd_inst, qtd_cli, qtd_pen,
                         instalacoes, custo_conexao, penalidades};
                          
        // Gera solução usando algoritmo guloso
        solucao = greedUFL(instalacoes, custo_conexao, penalidades);

        // Calcula o custo total da solução
        solucao.totalCost = calculocusto(instance_data, solucao);

        std::cout << "Solução inicial gerada pelo algoritmo guloso:\n";
        printSolution(solucao, conflitos);

        // Busca local para melhorar a solução
        LocalSearch ls(instance_data, solucao);
        ls.improveSolution();
        solucao = ls.getSolution();

        std::cout << "\nSolução após busca local:\n";
        printSolution(solucao, conflitos);
    }

    return 0;
}
