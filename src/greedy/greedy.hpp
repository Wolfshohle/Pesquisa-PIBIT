#pragma once
#include <vector>
#include "../structures/instance.hpp"
#include "../structures/solution.hpp"
#include "../utils/buscabinaria.hpp"
#include "../utils/calculocusto.hpp"

// Algoritmo ganancioso para o problema de localização de instalações
bool greedUFL(instance& inst, Srepresentation& solucao);