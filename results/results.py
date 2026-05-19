import re
import os
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# =============================================
# Pega os principais dados da instância que iremos construir o gráfico e a tabela
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
# =============================================
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

    table_last = None
    for i, linha in enumerate(text):
        if "Implied bound cuts applied" in linha:
            table_last = i
            break

    if table_last == None:
        return None
    
    for j in range(table_last - 1, -1, -1):
        linha = text[j].strip()
        if "%" in linha:
            important_values = re.findall(r'(\d+\.\d+)', linha)
            if status == "Optimal":
                gap = 0.0
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
        with open(f"{log_heuristic_name}_seed_{i}.log", 'r', encoding='utf-8') as heuristic_data:
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
        "seed": seed / 10,
        "obj": obj / 10,
        "time_to_best": time_to_best / 10,
        "it_to_best": it_to_best / 10,
        "best_obj": best_obt,
        "mean_obj": mean_obj / 10,
        "worst_obj": worst_obj / 10,
        "bad_obj": bad_obj,
        "improvment": improvments / 10
    }

    return heuristic_log_data
# =============================================

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
        if not os.path.exists(heuristic_log_path + "_seed_1.log"):
            print("Log heurística não encontrado:", heuristic_log_path + "_seed_1.log")
            continue

        print("Processando:", base_name)

        # Pega os dados da instancia, do cplex e da heuristica
        instance_data = read_instance(instance_path)
        cplex_data = read_cplex_log(cplex_log_path)
        heuristic_data = read_heuristic_log(heuristic_log_path)

        row = {}

        row.update(instance_data)

        for key, value in cplex_data.items():
            row["cplex_" + key] = value

        for key, value in heuristic_data.items():
            row["ils_" + key] = value

        row["gap_best_ils_vs_cplex"] = ((row["ils_best_obj"] - row["cplex_Obj"]) / row["cplex_Obj"]) * 100
        row["gap_mean_ils_vs_cplex"] = ((row["ils_obj"] - row["cplex_Obj"]) / row["cplex_Obj"]) * 100

        all_results.append(row)
    
    df = pd.DataFrame(all_results)


