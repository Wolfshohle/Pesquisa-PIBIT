from bateria import circuito as circ
import numpy as np
import matplotlib.pyplot as plt

# Definição de dados
baterias = {
    "Bateria 1": {"Resistencias": [0.01, 0.01, 0.1, 0.05], "Capacitor": 7500, "FontesTensao": [2, 1.75], "FonteCorrente": 10, "TensaoFonteCorrente":[]}, 
    "Bateria 2": {"Resistencias": [0.01, 0.01, 0.1, 0.05], "Capacitor": 7500, "FontesTensao": [2, 1.75], "FonteCorrente": 10, "TensaoFonteCorrente":[]},
    "Bateria 3": {"Resistencias": [0.01, 0.01, 0.1, 0.05], "Capacitor": 7500, "FontesTensao": [2, 1.75], "FonteCorrente": 10, "TensaoFonteCorrente":[]}, 
    "Bateria 4": {"Resistencias": [0.01, 0.01, 0.1, 0.05], "Capacitor": 7500, "FontesTensao": [2, 1.75], "FonteCorrente": 10, "TensaoFonteCorrente":[]}, 
    "Bateria 5": {"Resistencias": [0.01, 0.01, 0.1, 0.05], "Capacitor": 7500, "FontesTensao": [2, 1.75], "FonteCorrente": 10, "TensaoFonteCorrente":[]}
}

# Definição de tempo
begin = 0
end = 1000
dt = 0.01

# Tensão total
Tensao_total = None

for nome, bateria in baterias.items():
    tensao = circ(begin, end, dt, bateria["Capacitor"], bateria["Resistencias"], bateria["FontesTensao"], bateria["FonteCorrente"])
    tensao.simular()
    
    bateria["TensaoFonteCorrente"] = tensao
    
    if Tensao_total is None:
        Tensao_total = tensao
    else:
        Tensao_total.vf_charge += tensao.vf_charge


# Cria figura com layout 2x3
fig, axs = plt.subplots(2, 3, figsize=(16, 8), constrained_layout=True)

# Nomes das baterias
baterias_nomes = list(baterias.keys())

# Plot das baterias (5 primeiros subplots)
for i, nome in enumerate(baterias_nomes):
    row = i // 3
    col = i % 3
    ax = axs[row, col]
    tensao = baterias[nome]["TensaoFonteCorrente"]
    ax.plot(tensao.time, tensao.vf_charge, label="Perfil 1")
    ax.set_title(nome)
    ax.set_xlabel("Tempo (s)")
    ax.set_ylabel("Tensão (V)")
    ax.grid(True)
    ax.legend()

# Último subplot (posição 5) reservado para Tensão Total
ax_total = axs[1, 2]
ax_total.clear()
ax_total.plot(Tensao_total.time, Tensao_total.vf_charge, color='black', label="Perfil 1")
ax_total.set_title("Tensão Total")
ax_total.set_xlabel("Tempo (s)")
ax_total.set_ylabel("Tensão (V)")
ax_total.grid(True)
ax_total.legend()

plt.show()
