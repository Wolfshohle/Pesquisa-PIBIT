#include <iostream>
#include <fstream>
#include "io/inputload.hpp"
#include "greedy/greedy.hpp"
#include "localsearch/localsearch.hpp"

using namespace std;


// Função para printar a solução
//----------------------------------------------------------------------//
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
//----------------------------------------------------------------------//


// Função ILS
//----------------------------------------------------------------------//
void ILS(instance& inst, Srepresentation& solucao, const vector<vector<pair<int, int>>>& conflitos)
{
    
}


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

    carregarInstancia(arquivo, instance_data);
        
    greedUFL(instance_data, solucao);

    cout << "Solução inicial:" << endl;
    printSolution(solucao, conflitos);

    LocalSearch ls(instance_data, solucao);
    ls.improveSolution();
    solucao = ls.getSolution();

    cout << "Solução após Busca Local:" << endl;
    printSolution(solucao, conflitos);
    
    return 0;
}
//----------------------------------------------------------------------//