#pragma once
#include <utility>

struct penalidade
{
    std::pair<int, int> clientes; // Guarda os clientes conflitantes
    int custo; // Guarda a penalidade do cliente atual 
};