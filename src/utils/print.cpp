#include "print.hpp"
#include <iostream>


//----------------------------------------------------------------------//
// Função para printar a solução
//----------------------------------------------------------------------//
void printSolution(const Srepresentation& solucao)
{
    //printa atribuições
    std::cout << "Instalacoes:" << std::endl;
    for(auto i: solucao.openfacilities)
    {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    std::cout << "Atribuicoes:" << std::endl;
    for(auto i: solucao.assignments)
    {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    std::cout << "Custo Total: " << solucao.totalCost << std::endl;

}
//----------------------------------------------------------------------//