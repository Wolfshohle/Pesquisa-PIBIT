#pragma once
#include <vector>


struct solutioninfo
{
    // ===================================================================
    // Armazena a quantidade de instalações abertas na solução
    int amountOpenFacilities = 0;

    // Incrementa a quantidade de instalações abertas
    void incrementFacilitiesCount()
    {
        amountOpenFacilities++;
    }

    // Decrementa a quantidade de instalações abertas
    void decrementFacilitiesCount()
    {
        amountOpenFacilities--;
    }

    // Reseta a quantidade de instalações abertas para zero
    void resetFacilitiesCount()
    {
        amountOpenFacilities = 0;
    }
    // ===================================================================
    

    // ===================================================================
    // Armazena os índices das instalações abertas na solução
    std::vector<int> OpenFacilities; 

    // Armazena os índices das instalações fechadas na solução
    std::vector<int> ClosedFacilities;
    // ===================================================================

    
    // ===================================================================
    // Armzena a quantidade de clientes alocados a cada instalação aberta
    std::vector<int> ClientsPerFacility;

    // Armazena a penalidade total por instalação
    std::vector<int> PenalityPerFacility;

    // Armazena posição das facilidades no vetor de facilidades abertas para acesso rápido
    std::vector<int> positionopen;
    // Armazena posição das facilidades no vetor de facilidades fechadas para acesso rápido
    std::vector<int> positionclose;

    // Inicializa as estruturas das facilidades
    void initializePerFacility(int numFacilities)
    {
        ClientsPerFacility.resize(numFacilities, 0);
        PenalityPerFacility.resize(numFacilities, 0);
        positionopen.resize(numFacilities, -1);
        positionclose.resize(numFacilities, -1);
    }
    // ===================================================================


    // ===================================================================
    // Atuailiza a opisção das facilidades abertas e fechadas para facilitar o acesso as variáveis
    void atualizeAllPositionOpenClose()
    {
        int i;
        int Open = OpenFacilities.size();
        int Close = ClosedFacilities.size();

        fill(positionopen.begin(), positionopen.end(), -1);
        fill(positionclose.begin(), positionclose.end(), -1);

        // Atualiza a posição das facilidades abertas
        for(i = 0; i < Open; i++)
        {
            positionopen[OpenFacilities[i]] = i;
        }

        // Atualiza a posição das facilidades fechadas
        for(i = 0; i < Close; i++)
        {
            positionclose[ClosedFacilities[i]] = i;
        }
    }

    // Atualiza a posição de uma instalação específica no vetor de instalações abertas e fechadas
    void atualizeOnePositionOpenClose(int facility)
    {
        int last_facility;

        // Se a instalação estiver aberta, atualiza para a instalação fechada
        if(positionopen[facility] != -1)
        {
            // Pega a última instalação aberta
            last_facility = OpenFacilities.back();
            
            // Coloca a última instalação aberta na posição da instalação que está sendo fechada
            OpenFacilities[positionopen[facility]] = last_facility;

            // Atualiza a posição da última instalação para a posição da instalação que está sendo fechada
            positionopen[last_facility] = positionopen[facility];

            // Remove a instalação que está sendo fechada do vetor de abertas
            positionopen[facility] = -1;
            OpenFacilities.pop_back();

            // Adiciona a instalação que está sendo fechada ao vetor de instalações fechadas
            ClosedFacilities.push_back(facility);
            positionclose[facility] = ClosedFacilities.size() - 1;
        }
        // Se a instalação estiver fechada, atualiza para a instalação aberta
        else
        {
            // Pega a última instalação aberta
            last_facility = ClosedFacilities.back();
            
            // Coloca a última instalação fechada na posição da instalação que está sendo aberta
            ClosedFacilities[positionclose[facility]] = last_facility;

            // Atualiza a posição da última instalação para a posição da instalação que está sendo aberta
            positionclose[last_facility] = positionclose[facility];

            // Remove a instalação que está sendo aberta do vetor de fechadas
            positionclose[facility] = -1;
            ClosedFacilities.pop_back();

            // Adiciona a instalação que está sendo aberta ao vetor de instalações abertas
            OpenFacilities.push_back(facility);
            positionopen[facility] = OpenFacilities.size() - 1;
        }
    }
    // ===================================================================
};