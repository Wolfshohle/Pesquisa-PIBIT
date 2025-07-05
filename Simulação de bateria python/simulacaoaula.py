from bateria import circuito as circ
import numpy as np
import matplotlib.pyplot as plt
import random


# Função que cria e simula o processo de formação das baterias
def CriaeSimula(begin, end, dt, capacitor, resistencia, vs, cs):
    baterias = []
    total = None

    for i in range(0, 5):
        bateria = circ(begin, end, dt, capacitor[i], resistencia[i], vs, cs)
        bateria.simular()
    
        if total is None:
            total = bateria.vf_charge.copy()
        else:
            total += bateria.vf_charge

        baterias.append(bateria)

    return baterias, total


# Configurações basicas
base_resistencias = [[0.01, 0.01, 0.1, 0.05], [0.01, 0.01, 0.1, 0.05], [0.01, 0.01, 0.1, 0.05], [0.01, 0.01, 0.1, 0.05], [0.01, 0.01, 0.1, 0.05]] 
base_capacitor = [7500, 7500, 7500, 7500, 7500]
base_vs = [2, 1.75]
cs = 10


# Definição de tempo
begin = 0
end = 2000
dt = 0.01



#Baterias base
baterias_base, total_base = CriaeSimula(begin, end, dt, base_capacitor, base_resistencias, base_vs, cs)



# Perfil 1(Variando R4):
bateriaM = []
totalM = []

for i in range(0, 5):
    resistenciasmodificadas =  [[0.01, 0.01, 0.1, 0.05], [0.01, 0.01, 0.1, 0.05], [0.01, 0.01, 0.1, 0.05], [0.01, 0.01, 0.1, 0.05], [0.01, 0.01, 0.1, 0.05]]
    for j in range(0, i + 1):
        resistenciasmodificadas[j][3] = resistenciasmodificadas[j][3] + random.uniform(-0.01, 0.1)

    bateriasmodificadas, totalmodificado = CriaeSimula(begin, end, dt, base_capacitor, resistenciasmodificadas, base_vs, cs)

    bateriaM.append(bateriasmodificadas)
    totalM.append(totalmodificado)



fig, axs = plt.subplots(2, 3, figsize=(12, 8), constrained_layout=True)
# bateria 1
axs[0, 0].plot(baterias_base[0].time, baterias_base[0].vf_charge, label = f"R4: {baterias_base[0].resistors[3]:.3f} Ω")
for i in range(0, 5):
    axs[0, 0].plot(bateriaM[i][0].time, bateriaM[i][0].vf_charge, label = f"R4: {bateriaM[i][0].resistors[3]:.3f} Ω")
axs[0, 0].set_title("Bateria 1")
axs[0, 0].set_xlabel("Tempo(s)")
axs[0, 0].set_ylabel("Tensão(v)")
axs[0, 0].grid(True)
axs[0, 0].legend(fontsize="small")

# bateria 2
axs[0, 1].plot(baterias_base[1].time, baterias_base[1].vf_charge, label = f"R4: {baterias_base[1].resistors[3]:.3f} Ω")
for i in range(0, 5):
    axs[0, 1].plot(bateriaM[i][1].time, bateriaM[i][1].vf_charge, label = f"R4: {bateriaM[i][1].resistors[3]:.3f} Ω")
axs[0, 1].set_title("Bateria 2")
axs[0, 1].set_xlabel("Tempo(s)")
axs[0, 1].set_ylabel("Tensão(v)")
axs[0, 1].grid(True)
axs[0, 1].legend(fontsize="small")

# bateria 3
axs[0, 2].plot(baterias_base[2].time, baterias_base[2].vf_charge, label = f"R4: {baterias_base[2].resistors[3]:.3f} Ω")
for i in range(0, 5):
    axs[0, 2].plot(bateriaM[i][2].time, bateriaM[i][2].vf_charge, label = f"R4: {bateriaM[i][2].resistors[3]:.3f} Ω")
axs[0, 2].set_title("Bateria 3")
axs[0, 2].set_xlabel("Tempo(s)")
axs[0, 2].set_ylabel("Tensão(v)")
axs[0, 2].grid(True)
axs[0, 2].legend(fontsize="small")

# bateria 4
axs[1, 0].plot(baterias_base[3].time, baterias_base[3].vf_charge, label = f"R4: {baterias_base[3].resistors[3]:.3f} Ω")
for i in range(0, 5):
    axs[1, 0].plot(bateriaM[i][3].time, bateriaM[i][3].vf_charge, label = f"R4: {bateriaM[i][3].resistors[3]:.3f} Ω")
axs[1, 0].set_title("Bateria 4")
axs[1, 0].set_xlabel("Tempo(s)")
axs[1, 0].set_ylabel("Tensão(v)")
axs[1, 0].grid(True)
axs[1, 0].legend(fontsize="small")

# bateria 5
axs[1, 1].plot(baterias_base[4].time, baterias_base[4].vf_charge, label = f"R4: {baterias_base[4].resistors[3]:.3f} Ω")
for i in range(0, 5):
    axs[1, 1].plot(bateriaM[i][4].time, bateriaM[i][4].vf_charge, label = f"R4: {bateriaM[i][4].resistors[3]:.3f} Ω")
axs[1, 1].set_title("Bateria 5")
axs[1, 1].set_xlabel("Tempo(s)")
axs[1, 1].set_ylabel("Tensão(v)")
axs[1, 1].grid(True)
axs[1, 1].legend(fontsize="small")

# total
axs[1, 2].plot(baterias_base[0].time, total_base, label = f"R4: {baterias_base[0].resistors[3]:.3f} Ω")
j = 0
for i in range(0, 5):
    axs[1, 2].plot(bateriaM[i][0].time, totalM[i], label = f"R4: {bateriaM[i][j].resistors[3]:.3f} Ω")
    j+=1
axs[1, 2].set_title("total")
axs[1, 2].set_xlabel("Tempo(s)")
axs[1, 2].set_ylabel("Tensão(v)")
axs[1, 2].grid(True)
axs[1, 2].legend(fontsize="small")

plt.show()



# Perfil 2(Variando C):
bateriaM2 = []
totalM2 = []

for i in range(0, 5):
    capacitormodificado =  [7500, 7500, 7500, 7500, 7500]
    for j in range(0, i + 1):
        capacitormodificado[j] = capacitormodificado[j] + random.uniform(-7000, 20000)

    bateriasmodificadas2, totalmodificado2 = CriaeSimula(begin, end, dt, capacitormodificado, base_resistencias, base_vs, cs)

    bateriaM2.append(bateriasmodificadas2)
    totalM2.append(totalmodificado2)



fig, axs = plt.subplots(2, 3, figsize=(12, 8), constrained_layout=True)
# bateria 1
axs[0, 0].plot(baterias_base[0].time, baterias_base[0].vf_charge, label = f"C: {baterias_base[0].capacitor:.3f} F")
for i in range(0, 5):
    axs[0, 0].plot(bateriaM2[i][0].time, bateriaM2[i][0].vf_charge, label = f"C: {bateriaM2[i][0].capacitor:.3f} F")
axs[0, 0].set_title("Bateria 1")
axs[0, 0].set_xlabel("Tempo(s)")
axs[0, 0].set_ylabel("Tensão(v)")
axs[0, 0].grid(True)
axs[0, 0].legend(fontsize="small")

# bateria 2
axs[0, 1].plot(baterias_base[1].time, baterias_base[1].vf_charge, label = f"C: {baterias_base[1].capacitor:.3f} F")
for i in range(0, 5):
    axs[0, 1].plot(bateriaM2[i][1].time, bateriaM2[i][1].vf_charge, label = f"C: {bateriaM2[i][1].capacitor:.3f} F")
axs[0, 1].set_title("Bateria 2")
axs[0, 1].set_xlabel("Tempo(s)")
axs[0, 1].set_ylabel("Tensão(v)")
axs[0, 1].grid(True)
axs[0, 1].legend(fontsize="small")

# bateria 3
axs[0, 2].plot(baterias_base[2].time, baterias_base[2].vf_charge, label = f"C: {baterias_base[2].capacitor:.3f} F")
for i in range(0, 5):
    axs[0, 2].plot(bateriaM2[i][2].time, bateriaM2[i][2].vf_charge, label = f"C: {bateriaM2[i][2].capacitor:.3f} F")
axs[0, 2].set_title("Bateria 3")
axs[0, 2].set_xlabel("Tempo(s)")
axs[0, 2].set_ylabel("Tensão(v)")
axs[0, 2].grid(True)
axs[0, 2].legend(fontsize="small")

# bateria 4
axs[1, 0].plot(baterias_base[3].time, baterias_base[3].vf_charge, label = f"C: {baterias_base[3].capacitor:.3f} F")
for i in range(0, 5):
    axs[1, 0].plot(bateriaM2[i][3].time, bateriaM2[i][3].vf_charge, label = f"C: {bateriaM2[i][3].capacitor:.3f} F")
axs[1, 0].set_title("Bateria 4")
axs[1, 0].set_xlabel("Tempo(s)")
axs[1, 0].set_ylabel("Tensão(v)")
axs[1, 0].grid(True)
axs[1, 0].legend(fontsize="small")

# bateria 5
axs[1, 1].plot(baterias_base[4].time, baterias_base[4].vf_charge, label = f"C: {baterias_base[4].capacitor:.3f} F")
for i in range(0, 5):
    axs[1, 1].plot(bateriaM2[i][4].time, bateriaM2[i][4].vf_charge, label = f"C: {bateriaM2[i][4].capacitor:.3f} F")
axs[1, 1].set_title("Bateria 5")
axs[1, 1].set_xlabel("Tempo(s)")
axs[1, 1].set_ylabel("Tensão(v)")
axs[1, 1].grid(True)
axs[1, 1].legend(fontsize="small")

# total
axs[1, 2].plot(baterias_base[0].time, total_base, label = f"C: {baterias_base[0].capacitor:.3f} F")
j = 0
for i in range(0, 5):
    axs[1, 2].plot(bateriaM2[i][0].time, totalM2[i], label = f"C: {bateriaM2[i][j].capacitor:.3f} F")
    j+=1
axs[1, 2].set_title("total")
axs[1, 2].set_xlabel("Tempo(s)")
axs[1, 2].set_ylabel("Tensão(v)")
axs[1, 2].grid(True)
axs[1, 2].legend(fontsize="small")

plt.show()