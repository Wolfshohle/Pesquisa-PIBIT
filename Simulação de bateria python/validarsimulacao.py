import numpy as np
import matplotlib.pyplot as plt

# Dados iniciais
R = [0.01, 0.01, 0.1, 0.05] 
C = 7500
vs = [2, 1.75]
cs = 10

# Tempo
inicio = 0
fim = 4000
dt = 0.01
tempo = np.arange(inicio, fim, dt)

# Vetores
vf = np.zeros_like(tempo)    # Tensão da carga
dvf = np.zeros_like(tempo)   # Tensão da descarga
vc = np.zeros_like(tempo)
dvc = np.zeros_like(tempo)
i2 = np.zeros_like(tempo)

# ------------------- CARGA --------------------
i2[0] = (vs[0] + (R[2] * cs) - vs[1]) / (R[1] + R[2])
vf[0] = (cs * (R[0] + R[2]) - (R[2] * i2[0]) + vs[0]) * 6

for i in range(1, len(tempo)):
    deltavc = ((i2[i - 1] / C) - (vc[i - 1] / (R[3] * C))) * dt
    vc[i] = vc[i - 1] + deltavc
    i2[i] = (vs[0] + (R[2] * cs) - vc[i] - vs[1]) / (R[1] + R[2])
    vf[i] = (cs * (R[0] + R[2]) - R[2] * i2[i] + vs[0]) * 6

# ------------------- DESCARGA --------------------
dvf[0] = vf[-1]
dvc[0] = vc[-1]
dcs = -cs

for i in range(1, len(tempo)):
    deltavc = ((dcs / C) - (dvc[i - 1] / (R[3] * C))) * dt
    dvc[i] = dvc[i - 1] + deltavc
    dvf[i] = ((R[0] + R[1]) * dcs + dvc[i] + vs[1]) * 6

# ------------------- COMBINAÇÃO --------------------
tempo_total = np.concatenate([tempo, tempo + fim])
tensao_total = np.concatenate([vf, dvf])

# Corrente: +10 A durante carga, -10 A durante descarga
corrente_total = np.concatenate([np.ones_like(tempo) * cs, np.ones_like(tempo) * -cs])

# ------------------- PLOTS --------------------
fig, axs = plt.subplots(2, 1, figsize=(10, 6), sharex=True)

# Plot da tensão
axs[0].plot(tempo_total, tensao_total, label='Tensão da célula', color='blue')
axs[0].axvline(x=fim, color='gray', linestyle='--', label='Início da descarga')
axs[0].set_ylabel('Tensão (V)')
axs[0].set_title('Ciclo de Carga e Descarga da Bateria')
axs[0].legend()
axs[0].grid(True)

# Plot da corrente
axs[1].plot(tempo_total, corrente_total, label='Corrente', color='orange')
axs[1].axhline(0, color='gray', linestyle='--')
axs[1].set_ylabel('Corrente (A)')
axs[1].set_xlabel('Tempo (s)')
axs[1].legend()
axs[1].grid(True)

plt.tight_layout()
plt.show()
