import re
import os
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# =============================================
# Pega os principais dados da instância que iremos construir o gráfico e a tabela
# Retorna:
#   instance_name
#   qtd_facilits
#   qtd_clients
#   qtd_penalitys
#   max_penalitys
#   percent_penalitys
#   p
#   mode
#   instance_type
#   seed
# =============================================
def read_instance(instance_name):
    qtd_facilits = None
    qtd_clients = None
    qtd_penalitys = None
    max_penalitys = None
    percent_penalitys = None
    p = None
    mode = None
    instance_type = None
    seed = None

    # Pegando os dados de entrada da instância
    with open(instance_name, 'r', encoding='utf-8') as instance_file:
        data_instace_file = instance_file.readline()
        qtd_facilits = int(data_instace_file.split()[0])
        qtd_clients = int(data_instace_file.split()[1])
        qtd_penalitys = int(data_instace_file.split()[2])

    # Pega o valor de P
    match_p = re.search(r'P(\d+)', instance_name)
    p = float(match_p.group(1)) / 100

    # Pega o modo da geração
    match_mode = re.search(r'M(\d+)', instance_name)
    mode = int(match_mode.group(1))

    # Pega se foi probabilistico uniforme, densidade fixa uniforme ou densidade fixa HUB
    match_type = re.search(r'_(Continuo|HUB)_', instance_name, re.IGNORECASE)
    if match_type.group(1) == "Continuo" and mode == 0:
        instance_type = "PROB_UNIF"
    elif match_type.group(1) == "Continuo" and mode == 1:
        instance_type = "DENS_FIX_UNIF"
    else:
        instance_type = "DENS_FIX_HUB"

    # Pega a seed
    match_seed = re.search(r'_(\d+)', instance_name)
    seed = match_seed.group(1)

    # Pega o total de penalidades para a instância
    max_penalitys = int((qtd_clients * (qtd_clients - 1)) / 2)

    # Pega a porcentagem de penalidades da instância atual
    percent_penalitys = float(qtd_penalitys / max_penalitys) 

    instance_data = {
        'instance_name': instance_name,
        'qtd_facilits': qtd_facilits,
        'qtd_clients': qtd_clients,
        'qtd_penalitys': qtd_penalitys,
        'max_penalitys': max_penalitys,
        'percent_penalitys': percent_penalitys,
        'p': p,
        'mode': mode,
        'type': instance_type,
        'seed': seed
    }

    return instance_data
# =============================================


# =============================================
# Pega os dados do log do CPLEX
# Retorna:
#   time_limit
#   total_time
#   status
#   Obj
#   best_integer
#   best_bound
#   gap
# =============================================
#! Modificar URGENTE!!!
def read_cplex_log(log_cplex_name):
    time_limit = None
    total_time = None
    status = None
    Obj = None
    best_integer = None
    best_bound = None
    gap = None

    with open(log_cplex_name, 'r', encoding='utf-8') as cplex_data:
        data_cplex = cplex_data.read()

    # Pega o time limit
    match_time_limit = re.search(r'CPXPARAM_TimeLimit\s*(\d+)', data_cplex)
    time_limit = int(match_time_limit.group(1))

    # Pega o total time
    match_total_time = re.search(r'Total \(root\+branch&cut\)\s*=\s*(\d+(?:\.\d+)?)', data_cplex)
    total_time = float(match_total_time.group(1))

    # Pega o Obj
    match_obj = re.search(r'Obj:\s*(\d+)', data_cplex)
    Obj = int(match_obj.group(1))

    # Pega o status
    match_status = re.search(r'Status:\s*(\w+)', data_cplex)
    status = match_status.group(1)

    # Pega o best bound, best integer e o gap
    text = data_cplex.splitlines()

    marcadores = [
        "Implied bound cuts applied",
        "Zero-half cuts applied"
    ]

    table_last = None
    for i, linha in enumerate(text):
        for marcador in marcadores:
            if marcador in linha:
                table_last = i
                break

        if table_last is not None:
            break
    
    for j in range(table_last - 1, -1, -1):
        linha = text[j].strip()
        if "%" in linha:
            important_values = re.findall(r'(\d+\.\d+)', linha)
            if status == "Optimal":
                gap = 0.0
                best_bound = Obj
                best_integer = Obj
            else:
                gap = float(important_values[-1])
                best_bound = float(important_values[-2])
                best_integer = float(important_values[-3])
            break

    cplex_log_data = {
        'time_limit': time_limit,
        'total_time': total_time,
        'status': status,
        'Obj': Obj,
        'best_integer': best_integer,
        'best_bound': best_bound,
        'gap': gap
    }

    return cplex_log_data
# =============================================


# =============================================
# Pegar os dados da heurística
# Retorna:
#   limit_time / 10
#   obj / 10
#   time_to_best / 10
#   it_to_best / 10
#   best_obt
#   worst_obj / 10
#   bad_obj
#   improvments / 10
# =============================================
def read_heuristic_log(log_heuristic_name):
    limit_time = 0
    seed = 0
    obj = 0
    time_to_best = 0
    it_to_best = 0
    mean_obj = 0
    worst_obj = 0
    improvments = 0

    best_obt = None
    bad_obj = None

    for i in range(1, 11):
        with open(f"{log_heuristic_name}_seed{i}.log", 'r', encoding='utf-8') as heuristic_data:
            data_heuristic = heuristic_data.read()

        # Pega o tempo limite
        match_limit_time = re.search(r'Tempo limite:\s*(\d+)', data_heuristic)
        limit_time += int(match_limit_time.group(1))

        # Pega a seed
        match_seed = re.search(r'Seed:\s*(\d+)', data_heuristic)
        seed += int(match_seed.group(1))

        # Pega o Obj
        match_obj = re.findall(r'Custo Total:\s*(\d+)', data_heuristic)
        obj += int(match_obj[-1])

        if(best_obt == None or best_obt > int(match_obj[-1])):
            best_obt = int(match_obj[-1])

        # Pega o time_to_best
        match_time_to_best = re.search(r'Tempo até a melhor solução:\s*([\d.]+)', data_heuristic)
        time_to_best += float(match_time_to_best.group(1))

        # Pega o it_to_best
        match_it_to_best = re.search(r'Iterações até a melhor solução:\s*(\d+)', data_heuristic)
        it_to_best += int(match_it_to_best.group(1))

        # Pega o mean obj
        match_mean_obj = re.search(r'Média dos custos:\s*([\d.eE+-]+)', data_heuristic)
        mean_obj += float(match_mean_obj.group(1))

        # Pega o worst
        match_worst = re.search(r'Pior valor encontrado:\s*(\d+)', data_heuristic)
        worst_obj += int(match_worst.group(1))

        if bad_obj == None or bad_obj < int(match_worst.group(1)):
            bad_obj = int(match_worst.group(1))

        # Pega o improvments
        match_improvments = re.search(r'Melhorias:\s*(\d+)', data_heuristic)
        improvments += int(match_improvments.group(1))

    heuristic_log_data = {
        "limit_time": limit_time / 10,
        "obj": obj / 10,
        "time_to_best": time_to_best / 10,
        "it_to_best": it_to_best / 10,
        "best_obj": best_obt,
        "bad_obj": bad_obj,
        "worst_obj": worst_obj / 10,
        "improvment": improvments / 10
    }

    return heuristic_log_data
# =============================================

def plota_grafico(title1, title2, y, pathsave1, pathsave2, avaliation, df_mode0, df_mode1):
    p_mode_0 = df_mode0.groupby(["p", "type"])[avaliation].mean().reset_index()


    plt.figure()

    dados_tipo = p_mode_0[p_mode_0["type"] == "PROB_UNIF"]

    plt.plot(
        dados_tipo["p"],
        dados_tipo[avaliation],
        marker="o",
        label= "Probabilístico uniforme"
    )

    plt.xlabel("p")
    plt.ylabel(y)
    plt.title(title1)
    plt.legend()
    plt.grid(True)
    plt.savefig(os.path.join(directory_output, pathsave1), dpi=300, bbox_inches="tight")
    plt.close()

    p_mode_1 = df_mode1.groupby(["p", "type"])[avaliation].mean().reset_index()

    plt.figure()

    dados_uniforme = p_mode_1[p_mode_1["type"] == "DENS_FIX_UNIF"]
    dados_hub = p_mode_1[p_mode_1["type"] == "DENS_FIX_HUB"]

    plt.plot(
        dados_uniforme["p"],
        dados_uniforme[avaliation],
        marker="o",
        label= "Densidade fixa uniforme"
    )
    
    plt.plot(
        dados_hub["p"],
        dados_hub[avaliation],
        marker="o",
        label= "Densidade fixa HUB"
    )

    plt.xlabel("p")
    plt.ylabel(y)
    plt.title(title2)
    plt.legend()
    plt.grid(True)
    plt.savefig(os.path.join(directory_output, pathsave2), dpi=300, bbox_inches="tight")


if __name__ == "__main__":

    log_cplex_directory = "logs/exato"
    log_heuristic_directory = "logs/heuristica"
    instance_directory = "Data/raw"
    directory_output = "results"

    all_results = []

    for file_name in os.listdir(instance_directory):
        if not file_name.endswith(".txt"):
            continue

        # Pega o caminho da instância
        instance_path = os.path.join(instance_directory, file_name)

        # Pega a base do nome para pegar os dados de log do cplex e da heurística
        base_name = file_name.replace(".txt", "")

        # Pega o caminho do cplex
        cplex_log_path = os.path.join(log_cplex_directory, base_name + ".log")
        # Verifica se existe log do CPLEX
        if not os.path.exists(cplex_log_path):
            print("Log CPLEX não encontrado:", cplex_log_path)
            continue

        # Pega o caminho da heurística
        heuristic_log_path = os.path.join(log_heuristic_directory, base_name)
        # Verifica se existe pelo menos seed 1 da heurística
        if not os.path.exists(heuristic_log_path + "_seed1.log"):
            print("Log heurística não encontrado:", heuristic_log_path + "_seed_1.log")
            continue

        print("Processando:", base_name)

        # Pega os dados da instancia, do cplex e da heuristica
        instance_data = read_instance(instance_path)
        cplex_data = read_cplex_log(cplex_log_path)
        heuristic_data = read_heuristic_log(heuristic_log_path)

        if cplex_data is None:
            print("Não consegui ler CPLEX:", cplex_log_path)
            continue

        row = {}

        row.update(instance_data)

        for key, value in cplex_data.items():
            row["cplex_" + key] = value

        for key, value in heuristic_data.items():
            row["ils_" + key] = value

        row["gap_best_ils_vs_cplex"] = ((row["ils_best_obj"] - row["cplex_Obj"]) / row["cplex_Obj"]) * 100
        row["gap_obj_ils_vs_cplex"] = ((row["ils_obj"] - row["cplex_Obj"]) / row["cplex_Obj"]) * 100

        all_results.append(row)
        
    df = pd.DataFrame(all_results)
    
    avg_for_p = df.groupby("p").agg({
        "qtd_penalitys": "mean",
        "percent_penalitys": "mean",

        "cplex_total_time": "mean",
        "cplex_Obj": "mean",
        "cplex_gap": "mean",
        "cplex_status": lambda x: x.mode()[0],

        "ils_obj": "mean",
        "ils_best_obj": "mean",
        "ils_time_to_best": "mean",
        "ils_it_to_best": "mean",

        "gap_best_ils_vs_cplex": "mean",
        "gap_obj_ils_vs_cplex": "mean",

    }).reset_index()


    avg_for_p_and_type = df.groupby(["p", "type"]).agg({
        "qtd_penalitys": "mean",
        "percent_penalitys": "mean",

        "cplex_total_time": "mean",
        "cplex_Obj": "mean",
        "cplex_gap": "mean",
        "cplex_status": lambda x: x.mode()[0],

        "ils_obj": "mean",
        "ils_time_to_best": "mean",
        "ils_best_obj": "mean",

        "gap_best_ils_vs_cplex": "mean",
        "gap_obj_ils_vs_cplex": "mean",
    }).reset_index()

    # ==========================================
    # Organização para o Excel
    # ==========================================

    df["seed"] = df["seed"].astype(int)

    df = df.sort_values(by=["mode", "type", "p", "seed"])

    colunas_resultados = [
        "instance_name",
        "mode",
        "type",
        "p",
        "seed",

        "qtd_facilits",
        "qtd_clients",
        "qtd_penalitys",
        "max_penalitys",
        "percent_penalitys",

        "cplex_time_limit",
        "cplex_total_time",
        "cplex_status",
        "cplex_Obj",
        "cplex_gap",

        "ils_limit_time",
        "ils_obj",
        "ils_best_obj",
        "ils_time_to_best",
        "ils_it_to_best",
        "ils_improvment",

        "gap_best_ils_vs_cplex",
        "gap_obj_ils_vs_cplex",
    ]

    df_excel = df[colunas_resultados].copy()

    df_excel = df_excel.round({
        "p": 2,
        "percent_penalitys": 4,
        "cplex_total_time": 2,
        "cplex_gap": 2,
        "ils_obj": 2,
        "ils_best_obj": 2,
        "ils_worst_obj": 2,
        "ils_bad_obj": 2,
        "ils_time_to_best": 2,
        "ils_it_to_best": 2,
        "ils_improvment": 2,
        "gap_best_ils_vs_cplex": 2,
        "gap_obj_ils_vs_cplex": 2,
    })

    avg_for_p = avg_for_p.sort_values(by=["p"]).round(2)
    avg_for_p_and_type = avg_for_p_and_type.sort_values(by=["type", "p"]).round(2)

    nomes_colunas1 = {
        "instance_name": "Instância",
        "mode": "Modo",
        "type": "Tipo",
        "p": "P",
        "seed": "Seed",

        "qtd_facilits": "Qtd. Instalações",
        "qtd_clients": "Qtd. Clientes",
        "qtd_penalitys": "Qtd. Penalidades",
        "max_penalitys": "Máx. Penalidades",
        "percent_penalitys": "% Penalidades",

        "cplex_time_limit": "CPLEX Limite Tempo",
        "cplex_total_time": "CPLEX Tempo Total",
        "cplex_status": "CPLEX Status",
        "cplex_Obj": "CPLEX Objetivo",
        "cplex_best_integer": "CPLEX Best Integer",
        "cplex_best_bound": "CPLEX Best Bound",
        "cplex_gap": "CPLEX Gap (%)",

        "ils_limit_time": "ILS Limite Tempo",
        "ils_obj": "Obj médio do ILS",
        "ils_best_obj": "Melhor Obj do ILS",
        "ils_worst_obj": "ILS Pior Médio",
        "ils_bad_obj": "ILS Pior Encontrado",
        "ils_time_to_best": "ILS Tempo até Melhor",
        "ils_it_to_best": "ILS Iterações até Melhor",
        "ils_improvment": "ILS Melhorias",

        "gap_best_ils_vs_cplex": "Gap Melhor obj ILS vs CPLEX (%)",
        "gap_obj_ils_vs_cplex": "Gap ILS vs CPLEX (%)(Média do obj encontrado)",
    }

    nomes_colunas2 = {
        "instance_name": "Instância",
        "p": "P",

        "qtd_penalitys": "Qtd. Penalidades",
        "percent_penalitys": "% Penalidades",

        "cplex_total_time": "CPLEX Tempo Total",
        "cplex_Obj": "CPLEX Objetivo",
        "cplex_status": "CPLEX Status",
        "cplex_gap": "CPLEX Gap (%)",

        "ils_limit_time": "ILS Limite Tempo",
        "ils_obj": "Obj médio do ILS",
        "ils_best_obj": "Melhor Obj do ILS",
        "ils_time_to_best": "ILS Tempo até Melhor",
        "ils_it_to_best": "ILS Iterações até Melhor",

        "gap_best_ils_vs_cplex": "Gap Melhor obj ILS vs CPLEX (%)",
        "gap_obj_ils_vs_cplex": "Gap ILS vs CPLEX (%)(Média do obj encontrado)",
    }

    df_excel = df_excel.rename(columns=nomes_colunas1)
    avg_for_p_excel = avg_for_p.rename(columns=nomes_colunas2)
    avg_for_p_and_type_excel = avg_for_p_and_type.rename(columns=nomes_colunas1)

    
    with pd.ExcelWriter("tabela de saída.xlsx") as writer:
        df_excel.to_excel(writer, sheet_name="Resultados completos", index=False)
        avg_for_p_and_type_excel.to_excel(writer, sheet_name="Media por p e tipo", index=False)
        avg_for_p_excel.to_excel(writer, sheet_name="Media por p", index=False)

    df_m0 = df[df["mode"] == 0]
    df_m1 = df[df["mode"] == 1]

    # ==========================================================================
    # Plotando o gráfico Tempo total do CPLEX por p
    # ==========================================================================
    plota_grafico("Tempo total do cplex por p - mode 0", 
                  "Tempo total do cplex por p - mode 1",
                  "Tempo médio do CPLEX (s)",
                  "tempo_cplex_por_p_Mode_0.png",
                  "tempo_cplex_por_p_Mode_1.png",
                  "cplex_total_time",
                  df_m0, df_m1)
    # ==========================================================================

    # ==========================================================================
    # Plotando o gráfico Gap médio do CPLEX por p
    # ==========================================================================  
    plota_grafico("Gap médio do CPLEX por p: mode 0", 
                  "Gap médio do CPLEX por p: mode 1",
                  "Gap médio do CPLEX (%)",
                  "gap_cplex_por_p_Mode_0.png",
                  "gap_cplex_por_p_Mode_1.png",
                  "cplex_gap",
                  df_m0, df_m1)
    # ==========================================================================

    # ==========================================================================
    # Plotando o gráfico do Gap entre o melhor do ILS vs CPLEX por p
    # ==========================================================================
    plota_grafico("Gap entre o melhor do ILS vs CPLEX por p: mode 0", 
                  "Gap entre o melhor do ILS vs CPLEX por p: mode 1",
                  "Gap entre o melhor do ILS vs CPLEX (%)",
                  "gap_best_ils_vs_cplex_por_p_Mode_0.png",
                  "gap_best_ils_vs_cplex_por_p_Mode_1.png",
                  "gap_best_ils_vs_cplex",
                  df_m0, df_m1)
    # ==========================================================================

    # ==========================================================================
    # Plotando o gráfico do Gap ILS vs CPLEX por p
    # ==========================================================================
    plota_grafico("Gap entre ILS vs CPLEX por p: mode 0", 
                  "Gap entre ILS vs CPLEX por p: mode 1",
                  "Gap entre ILS vs CPLEX (%)",
                  "gap_obj_ils_vs_cplex_por_p_Mode_0.png",
                  "gap_obj_ils_vs_cplex_por_p_Mode_1.png",
                  "gap_obj_ils_vs_cplex",
                  df_m0, df_m1)
    # ==========================================================================

    # Gráfico 5
    obj_por_p_mode0 = df_m0.groupby(["p", "type"])[["cplex_Obj", "ils_best_obj", "ils_obj"]].mean().reset_index()

    obj_mode0 = obj_por_p_mode0[obj_por_p_mode0["type"] == "PROB_UNIF"]

    plt.figure()
    plt.plot(obj_mode0["p"], obj_mode0["cplex_Obj"], marker="o", label="CPLEX")
    plt.plot(obj_mode0["p"], obj_mode0["ils_best_obj"], marker="o", label="Melhor ILS")
    plt.plot(obj_mode0["p"], obj_mode0["ils_obj"], marker="o", label="Média das SEEDS ILS")
    plt.xlabel("p")
    plt.ylabel("Custo médio")
    plt.title("CPLEX vs ILS por p")
    plt.legend()
    plt.grid(True)
    plt.savefig(os.path.join(directory_output, "obj_cplex_vs_ils_por_p_mode0.png"), dpi=300, bbox_inches="tight")
    plt.close()

    obj_por_p_mode1 = df_m1.groupby(["p", "type"])[["cplex_Obj", "ils_best_obj", "ils_obj"]].mean().reset_index()

    obj_mode1_continuo = obj_por_p_mode1[obj_por_p_mode1["type"] == "DENS_FIX_UNIF"]

    plt.figure()
    plt.plot(obj_mode1_continuo["p"], obj_mode1_continuo["cplex_Obj"], marker="o", label="CPLEX")
    plt.plot(obj_mode1_continuo["p"], obj_mode1_continuo["ils_best_obj"], marker="o", label="Melhor ILS")
    plt.plot(obj_mode1_continuo["p"], obj_mode1_continuo["ils_obj"], marker="o", label="Média das SEEDS ILS")
    plt.xlabel("p")
    plt.ylabel("Custo médio")
    plt.title("CPLEX vs ILS por p")
    plt.legend()
    plt.grid(True)
    plt.savefig(os.path.join(directory_output, "obj_cplex_vs_ils_por_p_mode1_den_fix_uni.png"), dpi=300, bbox_inches="tight")
    plt.close()

    obj_mode1_hub = obj_por_p_mode1[obj_por_p_mode1["type"] == "DENS_FIX_UNIF"]

    plt.figure()
    plt.plot(obj_mode1_hub["p"], obj_mode1_hub["cplex_Obj"], marker="o", label="CPLEX")
    plt.plot(obj_mode1_hub["p"], obj_mode1_hub["ils_best_obj"], marker="o", label="Melhor ILS")
    plt.plot(obj_mode1_hub["p"], obj_mode1_hub["ils_obj"], marker="o", label="Média das SEEDS ILS")
    plt.xlabel("p")
    plt.ylabel("Custo médio")
    plt.title("CPLEX vs ILS por p")
    plt.legend()
    plt.grid(True)
    plt.savefig(os.path.join(directory_output, "obj_cplex_vs_ils_por_p_mode1_den_fix_hub.png"), dpi=300, bbox_inches="tight")
    plt.close()

    print("Gráficos gerados na pasta:", directory_output)