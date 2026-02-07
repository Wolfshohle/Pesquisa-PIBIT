#include <iostream>
#include <fstream>
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
    if(argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <instancia>\n";
        return 1;
    }

    ifstream arquivo(argv[1]);
    if(!arquivo.is_open()) {
        std::cerr << "Erro ao abrir arquivo.\n";
        return 1;
    }

    instance instance_data;
    Srepresentation solucao;

    vector<vector<pair<int, int>>> conflitos;

    if(!carregarInstancia(arquivo, instance_data)) {
        std::cerr << "Erro ao carregar instancia.\n";
        return 1;
    }

    ILS ils_solver(instance_data, 500, 100, 42);
    ils_solver.run();

    solucao = ils_solver.getBestSolution();

    printSolution(solucao);
    
    return 0;
}
//----------------------------------------------------------------------//