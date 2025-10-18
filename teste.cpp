#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int buscabinária(vector<pair<int, int>> matriz_ordenacao, pair<int, int>key);

int main()
{
    int i;
    vector<vector<pair<int, int>>>custo_de_conexao = 
        //(valor, cliente)
        {{{3, 1}, {2, 2}, {7, 3}, {6, 4}}, //F1
        {{4, 1}, {1, 2}, {5, 3}, {8, 4}},  //F2
        {{6, 1}, {3, 2}, {2, 3}, {4, 4}}}; //F3


    vector<vector<pair<int, int>>>matriz_ordenacao = custo_de_conexao;
    for(i = 0; i < matriz_ordenacao.size(); i++)
    {
        sort(matriz_ordenacao[i].begin(), matriz_ordenacao[i].end());
    }

    int busca;

    busca = buscabinária(matriz_ordenacao[0], {7, 3});  
    
    cout << matriz_ordenacao[0][busca].first << " " << matriz_ordenacao[0][busca].second << endl;

}


// Busca binária de pares
int buscabinária(vector<pair<int, int>>matriz_ordenacao, pair<int, int>key)
{
    int esquerda = 0;
    int direita = matriz_ordenacao.size() - 1;
    int meio;

    while(esquerda <= direita)
    {
        meio = (direita + esquerda) / 2;
        if(matriz_ordenacao[meio].first == key.first)
        {
            return meio;
        }
        else if(matriz_ordenacao[meio].first > key.first)
        {
            direita = meio - 1;
        }
        else
        {
            esquerda = meio + 1;
        }
    }

    return -1;
}