import re
import random as ran

with open("Data/examples/wlp18.txt", 'r', encoding='utf_8') as file:
    data_file = file.read()

# Pega a qtd de instalacoes
match_f = re.search(r'Warehouses\s*=\s*(\d+)', data_file)
qtd_instalacoes = int(match_f.group(1))

# Pega a qtd de clientes
match_c = re.search(r'Stores\s*=\s*(\d+)', data_file)
qtd_clientes = int(match_c.group(1))

# Pega a qtd de penalidades
match_p = re.search(r'Incompatibilities\s*=\s*(\d+)', data_file)
qtd_penalidades = int(match_p.group(1))

# Pega os custos de abertura das instalacoes
match_fc = re.search(r'FixedCost\s*=\s*\[(.*?)\]', data_file, re.DOTALL)
custo_de_abrir = re.findall(r'\d+', match_fc.group(1))
custo_de_abrir = list(map(int, custo_de_abrir))

# Pega o custo de alocacao
match_cc = re.search(r'SupplyCost\s*=\s*\[(.*?)\]', data_file, re.DOTALL)
conteudo = match_cc.group(1)
linhas = conteudo.strip().split("\n")

matriz_de_alocacao = []
matriz = []

for linha in linhas:
    numeros = re.findall(r'\d+', linha)
    matriz.append(list(map(int, numeros)))

matriz_de_alocacao = list(map(list, zip(*matriz)))

# Pega as restricoes
match_pe = re.search(r'IncompatiblePairs\s*=\s*\[(.*?)\]', data_file, re.DOTALL)

pares_com_penalidade = []
conteudo = match_pe.group(1)
pares = re.findall(r'\|\s*(\d+)\s*,\s*(\d+)', conteudo)

for a,b in pares:
    i = int(a) - 1
    j = int(b) - 1

    penalidade = ran.randint(10000, 70000)

    pares_com_penalidade.append((i, j, penalidade))

data = {
    'qtd_instalacoes': qtd_instalacoes,
    'qtd_clientes': qtd_clientes,
    'qtd_penalidades': qtd_penalidades,
    'custo_de_abertura': custo_de_abrir,
    'custo_de_alocacao': matriz_de_alocacao,
    'penalidades': pares_com_penalidade
}


with open("Data/examples/wlp18-convertida.txt", "w", encoding='utf_8') as file2:
    file2.write(
            f"{data['qtd_instalacoes']} "
            f"{data['qtd_clientes']} "
            f"{data['qtd_penalidades']}\n\n"
        )
    
    for custo in data['custo_de_abertura']:
        file2.write(f"{custo} ")
    file2.write("\n\n")

    for linha in data['custo_de_alocacao']:
        for valor in linha:
            file2.write(f"{valor} ")
        file2.write("\n")
    file2.write("\n")

    for c1, c2, custo in data['penalidades']:
        file2.write(f"{c1} {c2} {custo}\n")