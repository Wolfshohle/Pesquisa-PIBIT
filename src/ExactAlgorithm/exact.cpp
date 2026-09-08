#include <ilcplex/ilocplex.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>

#include "../io/inputload.hpp"

using namespace std;

#define EPS 1e-6

ILOSTLBEGIN

class XbranchCallBack : public IloCplex::Callback::Function
{
    private:
        IloBoolVarArray x; // Instalações
        IloArray<IloBoolVarArray> y;    // clientes
        int n, m;

    public:
        XbranchCallBack(IloBoolVarArray x_main, IloArray<IloBoolVarArray> y_main, int n_main, int m_main)
        : x(x_main), y(y_main), n(n_main), m(m_main){}

        // Função invocada durante o processo de branching
        // Variável context contém informações sobre o estado atual do processo de branching
        void invoke(const IloCplex::Callback::Context& context) override
        {
            // Verifica se o callback foi invocado durante o processo de branching
            if(context.getId() != IloCplex::Callback::Context::Id::Branching)
            {
                return;
            }

            //  =====================================================
            // | Etapa 1: Procurar a instalação (X) Mais fracionária |
            //  =====================================================

            int mostFractionalIndex = -1;
            double mostFractionalValueX = -1.0;

            for(int i = 0; i < n; i++)
            {
                // Obtém o valor da variável de decisão x[i] no ponto de relaxação
                double value = context.getRelaxationPoint(x[i]);
                double fractionalPart = min(value, 1.0 - value);

                // Verifica se a variável é maior que 0 e menor que 1, e se é mais fracionária do que a atual mais fracionária
                if(value > EPS && value < 1.0 - EPS && fractionalPart > mostFractionalValueX)
                {
                    mostFractionalValueX = fractionalPart;
                    mostFractionalIndex = i;
                }
            }

            // Se achou uma instalação mais fracionária, cria os nós filhos para o branching
            if(mostFractionalIndex != -1)
            {
                branching(context, x[mostFractionalIndex]);
                return;
            }

            //  ==================================================
            // | Etapa 2: procurar o cliente (y) mais fracionário |
            //  ==================================================
            int best_i = -1;
            int best_j = -1;
            double mostFractionalValueY = -1.0;

            for(int i = 0; i < n; i++)
            {
                for(int j = 0; j < m; j++)
                {
                    // Obtém o valor da variável de decisão y[i][j] no ponto de relaxação
                    double value = context.getRelaxationPoint(y[i][j]);
                    double fractionalPart = min(value, 1.0 - value);

                    // Verifica se a variável é maior que 0 e menor que 1, e se é mais fracionária do que a atual mais fracionária
                    if(value > EPS && value < 1.0 - EPS && fractionalPart > mostFractionalValueY)
                    {
                        mostFractionalValueY = fractionalPart;
                        best_i = i;
                        best_j = j;
                    }
                }
            }

            // Se achou um cliente mais fracionário, cria os nós filhos para o branching
            if(best_i != -1 && best_j != -1)
            {
                branching(context, y[best_i][best_j]);
            }

            // Se não achou nenhuma variável fracionária, não faz nada
        }

        private:
            void branching(const IloCplex::Callback::Context& context, IloBoolVar var)
            {
                // Obtém o valor da função objetivo no ponto de relaxação
                IloNum estimate = context.getRelaxationObjective();

                // Cria os nós filhos para o branching
                // Branching down: fixa a variável var para 0
                context.makeBranch(var, 0, IloCplex::BranchDown, estimate);
                // Branching up: fixa a variável var para 1
                context.makeBranch(var, 1, IloCplex::BranchUp, estimate);
            }
};

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "Uso: ./exact <instancia>\n";
        return 1;
    }

    std::ifstream arquivo(argv[1]);

    if(!arquivo.is_open())
    {
        std::cout << "Erro ao abrir arquivo\n";
        return 1;
    }

    instance inst;

    if(!carregarInstancia(arquivo, inst))
    {
        std::cout << "Erro ao carregar instância\n";
        return 1;
    }

    // Cria o ambiente CPLEX
    IloEnv env;

    try
    {
        // Cria o modelo
        IloModel model(env);
        
        int n = inst.qtd_instalacoes;
        int m = inst.qtd_clientes;
        int k = inst.qtd_penalidades;

        // ==========================================================
        // Variáveis de decisão
        // ==========================================================
        // Vetor de decisão para instalações abertas
        IloBoolVarArray x(env, n);
        
        // Matriz de decisão para atribuição de clientes a instalações
        IloArray<IloBoolVarArray> y(env, n);
        for(int i = 0; i < n; i++)
        {
            y[i] = IloBoolVarArray(env, m);
        }

        // Penalidades entre os clientes
        IloBoolVarArray p(env, k);
        // ==========================================================


        // ==========================================================
        // Nomeando as variáveis para facilitar leitura do modelo
        for(int i = 0; i < n; i++)
        {
            x[i].setName(("x_" + to_string(i)).c_str());
            for(int j = 0; j < m; j++)
            {
                y[i][j].setName(("y_" + to_string(i) + "_" + to_string(j)).c_str());
            }
        }

        for(int l = 0; l < k; l++)
        {
            p[l].setName(("p_" + to_string(l)).c_str());
        }
        // ==========================================================

        // ==========================================================
        // Função objetivo
        // ==========================================================
        IloExpr obj(env); // Acumulador para a função objetivo

        // Abertura das instalações
        for(int i = 0; i < n; i++)
        {
            obj += inst.instalacoes[i].custo_abertura * x[i];
        }

        // Atribuição de clientes às instalações
        for(int i = 0; i < n; i++)
        {
            for(int j = 0; j < m; j++)
            {
                obj += inst.custo_conexao[i][j].first * y[i][j];
            }
        }

        // Penalidades entre os clientes
        for(int l = 0; l < k; l++)
        {
            obj += inst.penalidades_vetor[l].custo * p[l];
        }

        model.add(IloMinimize(env, obj));
        // =========================================================


        // =========================================================
        // Restrições
        // =========================================================
        // Cada cliente deve ser atendido por exatamente uma instalação
        for(int j = 0; j < m; j++)
        {
            IloExpr sum(env); // Restrição de atendimento dos clientes
            for(int i = 0; i < n; i++)
            {
                sum += y[i][j];
            }

            model.add(sum == 1);

            sum.end();
        }

        // Uma instalação só pode atender clientes se estiver aberta
        for(int i = 0; i < n; i++)
        {
            for(int j = 0; j < m; j++)
            {
                model.add(x[i] >= y[i][j]);
            }
        }

        // Penalidades entre os clientes (pares de clientes conflitantes)
        //! MODIFICAÇÃO
        vector<vector<int>> penalidades(m, vector<int>(m, -1));

        for(int l = 0; l < k; l++)
        {
            int c1 = inst.penalidades_vetor[l].clientes.first;
            int c2 = inst.penalidades_vetor[l].clientes.second;
            
            //! MODIFICAÇÃO
            penalidades[c1][c2] = l;
            penalidades[c2][c1] = l;

            for(int i = 0; i < n; i++)
            {
                model.add(p[l] >= y[i][c1] + y[i][c2] - 1);
            }
        }

        // Penalidades entre os clientes (trio de clientes conflitantes)
        // Conta quantos triangulos existem
        int triangleCount = 0;

        for(int a = 0; a < m; a++)
        {
            for(int b = a + 1; b < m; b++)
            {
                // Se não existe penalidade entre os clientes i e j, conitnua
                if(penalidades[a][b] == -1)
                {
                    continue;
                }

                for(int c = b + 1; c < m; c++)
                {
                    // Se não existe penalidade entre os clientes i e k, ou entre j e k, continua
                    if(penalidades[a][c] == -1 || penalidades[b][c] == -1)
                    {
                        continue;
                    }

                    // Se existe penalidade entre os clientes i, j e k, então encontramos um triangulo
                    int p_ab = penalidades[a][b];
                    int p_ac = penalidades[a][c];
                    int p_bc = penalidades[b][c];

                    triangleCount++;

                    // Cria as restrições para o triangulo encontrado
                    for(int i = 0; i < n; i++)
                    {
                        IloExpr triangle(env);

                        triangle += y[i][a];
                        triangle += y[i][b];
                        triangle += y[i][c];

                        triangle -= p[p_ab];
                        triangle -= p[p_ac];
                        triangle -= p[p_bc];

                        model.add(triangle <= 1);

                        triangle.end();
                    }
                }
            }
        }
        // =========================================================

        IloCplex cplex(model);
        cplex.setParam(IloCplex::Param::TimeLimit, 3600); // Define um tempo limite de 10min para a resolução
        
        //  =========================================
        // | Callback para o branching personalizado |
        //  =========================================
        //XbranchCallBack XbranchCb(x, y, n, m);
        // Define o contexto do callback para ser chamado durante o processo de branching
        //CPXLONG contextMask = IloCplex::Callback::Context::Id::Branching;
        // Registra o callback no CPLEX
        //cplex.use(&XbranchCb, contextMask);
        // ==========================================

        
        cplex.exportModel("C:\\Users\\m4the\\Documents\\Pibit-Pibic\\Repositório\\src\\ExactAlgorithm\\model.lp"); // Exporta o modelo para um arquivo .lp
        cplex.solve();

        cout << "Status: " << cplex.getStatus() << endl;
        cout << fixed << setprecision(0);
        cout << "Obj: " << cplex.getObjValue() << endl; 

        cout << "\nInstalacoes abertas:\n";
        for(int i = 0; i < n; i++)
        {
            if(cplex.getValue(x[i]) > 0.5)
                cout << "Instalação " << i << " aberta\n";
        }

        cout << "\nTriangulos encontrados: " << triangleCount << "\n";
        cout << "\nRestrições triangulares adicionadas: " << triangleCount * n << "\n";

        cout << "\nAtribuições (cliente -> instalação):\n";
        for(int j = 0; j < m; j++)
        {
            for(int i = 0; i < n; i++)

            {
                if(cplex.getValue(y[i][j]) > 0.5)
                {
                    cout << "Cliente " << j << " -> Instalação " << i << "\n";
                    break;
                }
            }
        }
        // =========================================================
        // Encerramento dos objetos para liberar memória
        obj.end();
        // =========================================================
    }
    catch(IloException& e)
    {
        cout << "Erro CPLEX: " << e << endl;
    }
    catch(...)
    {
        cout << "Erro desconhecido" << endl;
    }

    env.end();

    return 0;
}