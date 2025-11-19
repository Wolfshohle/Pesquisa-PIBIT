#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <fstream>
#include <string>

// Usando uma variável global
using namespace std;


struct penalidade
{
    pair<int, int> clientes; // Guarda os clientes conflitantes
    int custo; // Guarda a penalidade do cliente atual 
};

struct instalacao
{
    int custo_abertura; // Custo de abrir a instalação
    vector<pair<int, int>> ordem_por_custo; // Custo da instalação i atender o cliente j em ordem crescente pelo custo
    vector<int> Dlinha; // Conjunto de clientes já atendidos e que podem mudar para nova instalação
    vector<int> atribuicao; // guarda a atribuição dos clientes depois de ordenados
    vector<int> Y; // Clientes que ainda não foram atendidos
};

int buscabinariadepar(vector<penalidade>& penalidades, int c1, int c2)
{
    int left = 0;
    int right = penalidades.size() - 1;

    while(left <= right)
    {
        int mid = left + (right - left) / 2;

        if(penalidades[mid].clientes.first == c1 && penalidades[mid].clientes.second == c2 || penalidades[mid].clientes.first == c2 && penalidades[mid].clientes.second == c1)
        {
            return penalidades[mid].custo;
        }
        else if(penalidades[mid].clientes < make_pair(c1, c2))
        {
            left = mid + 1;
        }
        else
        {
            right = mid - 1;
        }
    }

    return 0;
}

int main()
{
    vector<penalidade>vetor = {{{1,2}, 10}, {{2,3}, 20}, {{3,4}, 30}};
    cout << buscabinariadepar(vetor, 3, 2) << endl;
}