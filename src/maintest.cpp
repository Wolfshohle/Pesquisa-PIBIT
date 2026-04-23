#include <iostream>
#include <unistd.h>
#include <cstdlib> 
#include "ils/ils.hpp"

using namespace std;

int main()
{
    ifstream arquivo("Data/examples/in.txt");

    cout << "Carregando instância..." << endl;
    if(!arquivo.is_open())
    {
        cerr << "Arquivo não encontrado :P\n";
        return 1;
    }

    instance instance_dat;
    Srepresentation solution;

    //! Pega a entrada do problema
    carregarInstancia(arquivo, instance_dat);
    
    //? Pega a solução do ganancioso
    greedUFL(instance_dat, solution);

    cout << "Solução inicial:" << endl;
    printSolution(solution);

    //? Aplica busca local
    LocalSearch ls(instance_dat, solution);
    ls.improveSolution2();
    solution = ls.getSolution();

    cout << "Solução após busca local:" << endl;
    printSolution(solution);


    return 0;
}