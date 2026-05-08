#include <iostream>
#include <unistd.h>
#include <cstdlib> 
#include "ils/ils.hpp"
#include "io/readcap.hpp"

using namespace std;

int main()
{
    ifstream arquivo("Data/examples/cap134.txt");

    cout << "Carregando instância..." << endl;
    if(!arquivo.is_open())
    {
        cerr << "Arquivo não encontrado :P\n";
        return 1;
    }

    instance instance_dat;

    readcap(arquivo, instance_dat);

    return 0;
}