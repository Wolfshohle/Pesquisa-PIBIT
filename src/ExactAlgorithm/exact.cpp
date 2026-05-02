#include <ilcplex/ilocplex.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "../io/inputload.hpp"

using namespace std;

ILOSTLBEGIN

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

        for(int l = 0; l < k; l++)
        {
            int c1 = inst.penalidades_vetor[l].clientes.first;
            int c2 = inst.penalidades_vetor[l].clientes.second;

            for(int i = 0; i < n; i++)
            {
                model.add(p[l] >= y[i][c1] + y[i][c2] - 1);
            }
        }
        // =========================================================

        IloCplex cplex(model);
        cplex.setParam(IloCplex::Param::TimeLimit, 600); // Define um tempo limite de 10min para a resolução
        cplex.exportModel("C:\\Users\\m4the\\Documents\\Pibit-Pibic\\Repositório\\src\\ExactAlgorithm\\model.lp"); // Exporta o modelo para um arquivo .lp
        cplex.solve();

        cout << "Status: " << cplex.getStatus() << endl;
        cout << "Obj: " << cplex.getObjValue() << endl; 

        cout << "\nInstalacoes abertas:\n";
        for(int i = 0; i < n; i++)
        {
            if(cplex.getValue(x[i]) > 0.5)
                cout << "Instalação " << i << " aberta\n";
        }

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