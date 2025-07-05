import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from bateria import circuito as b

# Função que cria e simula o processo de formação das baterias
def CriaeSimula(begin, end, dt, capacitor, resistencia, vs, cs):
    baterias = []
    total = None

    for i in range(0, 5):
        bateria = b(begin, end, dt, capacitor[i], resistencia[i], vs, cs)
        bateria.simular()
    
        if total is None:
            total = bateria.vf_charge.copy()
        else:
            total += bateria.vf_charge

        baterias.append(bateria)

    return baterias, total


# Função erro quadrado médio
def erroquadrado(real, estimado):
    return np.mean((real - estimado) ** 2)



# dados de bateria base
base_resistencias = [[0.01, 0.01, 0.1, 0.05], [0.01, 0.01, 0.1, 0.05], [0.01, 0.01, 0.1, 0.05], 
                     [0.01, 0.01, 0.1, 0.05], [0.01, 0.01, 0.1, 0.05]] 
base_capacitor = [7500, 7500, 7500, 7500, 7500]
base_vs = [2, 1.75]
cs = 10

# Definição de tempo
begin = 0
end = 2000
dt = 0.01

#Baterias base
baterias_base, total_base = CriaeSimula(begin, end, dt, base_capacitor, base_resistencias, base_vs, cs)
vb1 = baterias_base[0].vf_charge
vb2 = baterias_base[1].vf_charge
vb3 = baterias_base[2].vf_charge
vb4 = baterias_base[3].vf_charge
vb5 = baterias_base[4].vf_charge
vt = total_base

# saida real para comparação
saida_real = np.column_stack([vb2, vb3, vb4, vb5])
# Entradas
entradas = np.column_stack([vb1, vt])
# Pesos
pesos = np.ones((2, 4)) * 0.01
# Vies
bias = np.ones(4) * 0.01

print(entradas)