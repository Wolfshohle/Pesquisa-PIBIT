#include <iostream>
#include <fstream>
#include "../io/inputload.hpp"
#include "../greedy/greedy.hpp"
#include "../localsearch/localsearch.hpp"

using namespace std;


//----------------------------------------------------------------------//
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



//----------------------------------------------------------------------//
// Função ILS
//----------------------------------------------------------------------//
class ILS
{
    private:
        instance& inst;

        Srepresentation atualSolution;
        Srepresentation bestSolution;

        int qtdvezesmelhorResultado = 0;

        int maxIter;
        int pertubsize;


        // ===============================
        // Solução inicial (Greedy)
        // ===============================
        void initialSolution()
        {
            greedUFL(inst, atualSolution);
        }
        // ===============================


        // ===============================
        // Busca local na solução atual
        // ===============================
        void localSearch(Srepresentation& solucao)
        {
            LocalSearch ls(inst, solucao);
            ls.improveSolution();
            solucao = ls.getSolution();
        };


        // ===============================
        // Perturbação da solução atual
        // ===============================
        void perturbation(Srepresentation& solucao)
        {
            int n = inst.qtd_clientes;
            int m = inst.qtd_instalacoes;

            for(int i = 0; i < pertubsize; i++)
            {
                // Seleciona um cliente aleatório
                int client = rand() % n;
                // Seleciona uma nova instalação aleatória
                int new_facility = rand() % m;

                // Garante que a nova instalação seja diferente da atual
                while(solucao.assignments[client] == new_facility)
                {
                    new_facility = rand() % m;
                }

                solucao.assignments[client] = new_facility;
            }
        };
        // ===============================


        // ===============================
        // Reconstrução da solução
        // ===============================
        void reconstruction(Srepresentation& atualSolution)
        {
            int n = inst.qtd_clientes;
            int m = inst.qtd_instalacoes;
            int i;

            vector<int> ocupacao(m, 0);
            atualSolution.openfacilities.clear();

            for(i = 0; i < n; i++)
            {
                ocupacao[atualSolution.assignments[i]]++;
            }

            for(i = 0; i < m; i++)
            {
                if(ocupacao[i] > 0)
                {
                    atualSolution.openfacilities.push_back(i);
                }
            }

            atualSolution.totalCost = calculocusto(inst, atualSolution);
        };
        // ===============================


        // ===============================
        // Critério de aceitação
        // ===============================
        bool acceptanceCriterion(const Srepresentation& candidate)
        {
            return candidate.totalCost < atualSolution.totalCost;
        };
        // ===============================


    public:
        // ==============================
        // Construtor
        // ==============================
        ILS(instance& instancia, int max_iterations, int perturbation_size):
            inst(instancia), maxIter(max_iterations), pertubsize(perturbation_size)
            {
                srand(time(NULL));
            }
        // ==============================


        // ==============================
        // Loop principal
        // ==============================
        void run()
        {
            int runtime = 0;

            initialSolution();

            printSolution(atualSolution, vector<vector<pair<int, int>>>());

            localSearch(atualSolution);

            bestSolution = atualSolution;

            for(int i = 0; i < maxIter; i++)
            {
                Srepresentation candidateSolution = atualSolution;

                // Aplica perturbação jogando um cliente aleatório para outra instalação
                perturbation(candidateSolution);

                // Reconstrói a solução para ajustar instalações abertas e fechadas
                reconstruction(candidateSolution);

                // Aplica busca local na solução perturbada
                localSearch(candidateSolution);

                cout << "Iteração " << i+1 << ": Custo da solução candidata = " << candidateSolution.totalCost << endl;

                
                // Critério de aceitação
                if(acceptanceCriterion(candidateSolution))
                {
                    atualSolution = candidateSolution;

                    if(atualSolution.totalCost < bestSolution.totalCost)
                    {
                        bestSolution = atualSolution;
                    }
                }

                if(candidateSolution.totalCost == bestSolution.totalCost)
                {
                    qtdvezesmelhorResultado++;
                }

                runtime++;
            }

            atualSolution = bestSolution;
            cout << "Melhor resultado encontrado " << qtdvezesmelhorResultado << " vezes." << endl;
            cout << "Número de iterações: " << runtime << endl;
        }
        // ==============================


        // ==============================
        // Obtém a melhor solução encontrada
        // ==============================
        Srepresentation getBestSolution()
        {
            return bestSolution;
        }
        // ==============================
};


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

    ILS ils_solver(instance_data, 500, 100);
    ils_solver.run();

    solucao = ils_solver.getBestSolution();

    printSolution(solucao, conflitos);
    
    return 0;
}
//----------------------------------------------------------------------//