import re
import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

def get_data(filepath, instancepath):
    gap = None
    time = None
    obj = None
    status = None
    P = None

    with open(filepath, 'r', encoding='utf-8') as log_file:
        data_file = log_file.read()

    # Ler o arquivo de instância para obter a quantidade de facilitys, customers e penalties
    with open(instancepath, 'r', encoding='utf-8') as instance_file:
        data_instance = instance_file.read()

    qtd_facilitys = int(data_instance.split()[0])
    qtd_customers = int(data_instance.split()[1])
    qtd_penalties = int(data_instance.split()[2])

    # --------------------
    # Contar as penalidades de cada cliente
    # --------------------
    data_instance_lines = data_instance.splitlines()
    penalities = [0] * qtd_customers
    
    for line in data_instance_lines[-qtd_penalties:]:
        a = int(line.split()[0])
        b = int(line.split()[0])
        penalities[a] += 1
        penalities[b] += 1
    max_degree = max(penalities)
    avg_degree = np.mean(penalities)
    std_degree = np.std(penalities)

    threshold = avg_degree * 1.5
    num_high_degree = sum(1 for g in penalities if g > threshold)

    # ---------------------
    # Calcular a densidade
    # ---------------------
    max_edges = (qtd_customers * (qtd_customers - 1)) / 2
    density = qtd_penalties / max_edges

    final = data_file[-10000:]

    # -------------------
    # P
    # -------------------
    match_p = re.search(r'P(\d+)', filepath)

    if match_p:
        P = float(match_p.group(1)) / 100

    # -------------------
    # GAP
    # -------------------
    match_gap = re.findall(r'(\d+\.\d+)%', final)

    if match_gap:
        gap = float(match_gap[-1]) / 100
    else:
        print("Não encontrou gap")


    # -------------------
    # Tempo
    # -------------------
    match_time = re.search(r'Total .*?=\s*([\d\.]+) sec', final)
    if match_time:
        time = float(match_time.group(1))
    else:
        print("Não encontrou tempo")

    # -------------------
    # Objetivo
    # -------------------
    match_obj = re.search(r'Obj:\s*([\d\.]+)', final)
    if match_obj:
        obj = int(match_obj.group(1))
    else:
        print("Não encontrou objetivo")

    # -------------------
    # Status
    # -------------------
    match_status = re.search(r'Status:\s*(\w+)', final)
    if match_status:
        status = match_status.group(1)
    else:
        print("Não encontrou status")

    # --------------------
    # Contando as penalidades de cada cliente
    # --------------------


    # -------------------
    # Montar o dicionário de dados
    # -------------------
    data = {
        'Facilitys': qtd_facilitys,
        'Customers': qtd_customers,
        'Penalties': qtd_penalties,
        'P': P,
        'Density': density,
        'Gap': gap,
        'Time': time,
        'Objective': obj,
        'Status': status,
        'MaxDegree': max_degree,
        'AvgDegree': avg_degree,
        'StdDegree': std_degree,
        'NumHighDegree': num_high_degree
    }

    return data

if __name__ == "__main__":
    data_list = []

    # Caminho para o diretório contendo os arquivos de log e instância
    log_directory = "logs/cplex_test"
    instance_directory = "Data/raw"

    data = get_data("logs/cplex_test/80_P100_1.log", "Data/raw/80_P100_1.txt")
    # Iterar sobre os arquivos de log no diretório
    
    for filename in sorted(os.listdir(log_directory)):
        if not filename.endswith(".log"):
            continue

        # Construir o caminho completo para o arquivo de log
        log_filepath = os.path.join(log_directory, filename)

        # Encontrar o arquivo de instância correspondente
        instance_filename = filename.replace(".log", ".txt")
        instance_filepath = os.path.join(instance_directory, instance_filename)

        # Verificar se o arquivo de instância existe
        if not os.path.isfile(instance_filepath):
            print(f"Arquivo de instância não encontrado para {filename}")
            continue

        # Obter os dados do arquivo de log e instância
        dados = get_data(log_filepath, instance_filepath)
        data_list.append(dados)
    
    # Criar um DataFrame a partir da lista de dados
    df = pd.DataFrame(data_list)
    
    df_agrupado = df.groupby('P').agg(
        {
            'Gap': 'mean',
            'Time': 'mean',
            'Objective': 'mean',
            'Density': 'mean'
        }
    ).reset_index()

    print("Def_agrupado:\n")
    print(df_agrupado)

    plt.plot(df_agrupado['P'], df_agrupado['Gap'], marker='o')
    plt.xlabel("P")
    plt.ylabel("Gap Médio")
    plt.title("Gap Médio por P")
    plt.grid()
    plt.show()

    plt.scatter(df["Density"], df["Gap"])
    plt.xlabel("Densidade")
    plt.ylabel("Gap")
    plt.title("Gap vs Densidade")
    plt.grid()
    plt.show()

    print("Std_por_p\n")
    std_por_p = df.groupby("P")["Gap"].std()
    print(std_por_p.reset_index())

    df.boxplot(column="Gap", by="P")
    plt.show()

    print("corr: DensityXGap\n")
    print(df[["Density", "Gap"]].corr())

    print("Observando a instância p = 0.8:\n")
    print(df[df["P"] == 0.8].sort_values("Gap"))

    df_p = df[df["P"] == 0.8]
    df_p = df_p.sort_values("Gap")
    facil = df_p.head(3)
    dificil = df_p.tail(3)
    print("Classificando:\n")
    print("Facil:")
    print(facil[["Penalties", "Density", "Gap", "MaxDegree", "StdDegree"]])
    print("Dificil:")
    print(dificil[["Penalties", "Density", "Gap", "MaxDegree", "StdDegree"]])

    print(df[["Gap", "MaxDegree", "StdDegree"]].corr())

    plt.scatter(df["NumHighDegree"], df["Gap"])
    plt.xlabel("Clientes problemáticos")
    plt.ylabel("Gap")
    plt.title("Gap vs Clientes com alto grau")
    plt.show()

    print("Fácil:")
    print(facil[["Gap", "NumHighDegree"]])

    print("\nDifícil:")
    print(dificil[["Gap", "NumHighDegree"]])

    df.boxplot(column="NumHighDegree", by="P")
    plt.show()

    