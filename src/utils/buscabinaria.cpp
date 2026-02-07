#include "buscabinaria.hpp"

using namespace std;

int buscabinariadepar(vector<penalidade>& penalidades, int c1, int c2)
{
    int left = 0;
    int right = penalidades.size() - 1;

    while(left <= right)
    {
        int mid = left + (right - left) / 2;

        if((penalidades[mid].clientes.first == c1 && penalidades[mid].clientes.second == c2) || (penalidades[mid].clientes.first == c2 && penalidades[mid].clientes.second == c1))
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