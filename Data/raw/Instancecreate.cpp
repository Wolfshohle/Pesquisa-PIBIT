#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

using namespace std;

// == Função para gerar os dados das instalações, clientes e penalidades ==
// ========================================================================
void looping(int instalacoes, int clientes, int penalidades, ofstream &out)
{
    int i, j;
    out << instalacoes << " " << clientes << " " << penalidades << endl << endl;

    for(i = 0; i < instalacoes; i++)
    {
        if(i == instalacoes - 1)
        {
            out << rand() %7500 + 1 << endl;
        }
        else
        {
            out << rand() %7500 + 1 << " ";
        }
    }

    out << endl;

    for(i = 0; i < instalacoes; i++)
    {
        for(j = 0; j < clientes; j++)
        {
            if(j == clientes - 1)
            {
                out << rand() %7500 + 1 << endl;
            }
            else
            {
                out << rand() %7500 + 1 << " ";
            }
        }
    }

    out << endl;

    for(i = 0; i < penalidades; i++)
    {
        int cliente1 = 1, cliente2 = 1;
        while(cliente1 == cliente2)
        {
            cliente1 = rand() %clientes;
            cliente2 = rand() %clientes;
        }
        out << cliente1 << " " << cliente2 << "  " << rand() %7500 + 1 << endl;
    }
}
// ========================================================================


// == Função para gerar um valor aleatório dentro de um intervalo específico ==
// ============================================================================
int loopingaux(int limite, int intervalo, int clientes, int opc)
{
    int random_valor = rand() % intervalo + 1;
    if(opc == 1)
    {
        while(random_valor < limite)
        {
            random_valor = rand() % intervalo + 1;
        }
    }
    else
    {
        while(random_valor < limite || random_valor > clientes)
        {
            random_valor = rand() % intervalo + 1;
        }
    }
    return random_valor;
}
// ============================================================================


// == Função para gerar a instância com base na opção escolhida pelo usuário ==
// ============================================================================
void gerarinstancia(int opc, int seed)
{
    string filename, filename_identification;
    int instalacoes, clientes, penalidades;

    // Define o identificador do arquivo com base na opção escolhida
    switch (opc)
    {
        case 1:
            filename_identification = "small";
            break;
        case 2:
            filename_identification = "medium";
            break;
        case 3:
            filename_identification = "large";
            break;
        
        default:
            filename_identification = "controlated";
            break;
    }
    
    while(true)
    {
        cout << "Insira o nome do arquivo de saida (ex: in.txt) ou digite 5 para voltar" << endl;
        cout << "> ";

        cin >> filename;


        // Verifica se o usuário deseja voltar ao menu principal
        if(filename == "5")
        {
            cout << "Voltando ao menu principal..." << endl;
            break;
        }


        ifstream check_file("Data/raw/" + filename);
        // Verifica se o arquivo já existe
        if(check_file.is_open())
        {
            cout << "Arquivo já existe!" << endl;
            check_file.close();
            continue;
        }

        // Cria o arquivo e escreve algumas linhas de exemplo
        ofstream out("Data/raw/" + filename + " - " + filename_identification + ".txt");
        if(!out)
        {
            cerr << "Erro ao criar o arquivo!" << endl;
        }
    
        // Gera os dados com base na opção escolhida
        switch(opc)
        {
            case 1:
                instalacoes = loopingaux(4, 10, 0, 1);
                clientes = loopingaux(5, 20, 0, 1);
                penalidades = loopingaux(5, 20, clientes, 0);
                looping(instalacoes, clientes, penalidades, out);
                break;
            case 2:
                instalacoes = loopingaux(10, 50, 0, 1);
                clientes = loopingaux(20, 100, 0, 1);
                penalidades = loopingaux(20, 100, clientes, 0);
                looping(instalacoes, clientes, penalidades, out);
                break;
            case 3:
                instalacoes = loopingaux(20, 100, 0, 1);
                clientes = loopingaux(50, 200, 0, 1);
                penalidades = loopingaux(50, 200, clientes, 0);
                looping(instalacoes, clientes, penalidades, out);
                break;
        }

        out.close();


        // Salva a seed utilizada para gerar a instância em um arquivo de texto
        ofstream seedfile("Data/raw/seeds/seed.txt", ios::app);
        if(!seedfile.is_open())
        {
            cout << "Erro ao abrir o arquivo de seed!" << endl;
        }

        seedfile << filename + " - " + filename_identification << ": " << seed << endl;
        seedfile.close();

        break;
    }

    return;
}

int main()
{
    while(true)
    {
        int opc;
        unsigned int seed = static_cast<unsigned int>(time(0));

        //seed para gerar as instancias
        srand(seed);

        cout << "Bem vindo ao gerador de instancias!" << endl;
        cout << "Qual o tipo de instancia que deseja criar?" << endl;
        cout << "1 - Instancia de pequena (até 10 instalacoes, 20 clientes e 20 penalidades)" << endl;
        cout << "2 - Instancia de media (até 50 instalacoes, 100 clientes e 100 penalidades)" << endl;
        cout << "3 - Instancia de grande (até 100 instalacoes, 200 clientes e 200 penalidades)" << endl;
        cout << "4 - Instancia controlável" << endl;
        cout << "5 - Sair" << endl;
        cout << "> ";

        cin >> opc;

        if(opc == 5)
        {
            cout << "Saindo do gerador de instancias..." << endl;
            break;
        }
        else if(opc >= 1 && opc <= 4)
        {
            gerarinstancia(opc, seed);
        }
        else
        {
            cout << "Opcao invalida! Por favor, escolha uma opcao entre 1 e 5." << endl;
        }
    }

    return 0;
}