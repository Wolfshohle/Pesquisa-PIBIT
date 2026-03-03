#!/bin/bash

# ===============================
# Variáveis de destinos
instancias="Data/raw/"
logs="results/logs"
exe="./main"

# Variáveis de configuração
TIME=120
# SEED=42
REPETIR=20
# ===============================


# Criar diretório de logs se não existir
# ===============================
mkdir -p $logs
# ===============================


# Compilar o código
# ===============================
echo "Compilando o código..."
make

# Verificar se o executável foi criado com sucesso
if [ ! -f "$exe" ]; 
then
    echo "Erro: Executável não encontrado. Certifique-se de que a compilação foi bem-sucedida."
    exit 1
fi

echo "Compilação concluída com sucesso!"
# ===============================


# Executar o programa para cada instância
# ===============================
for i in "$instancias"/*.txt
do
    j=$(basename "$i")

    echo "Executando $i"

    if [ -e "$logs/$j.log" ]; 
    then
        echo "O log para $i já existe. Pulando execução."
        continue
    fi
    
    for k in $(seq 1 $REPETIR)
    do
        SEED=$((RANDOM % 100 + 1))
        echo "Execução $k"
        echo "-------------------------------" >> "$logs/$j.log"
        echo "Execução" $k >> "$logs/$j.log"
        $exe -T $TIME -s $SEED "$i" >> "$logs/$j.log"
        echo "-------------------------------" >> "$logs/$j.log"
    done

    echo "-------------------------------"
done
# ===============================

make clean

echo "Execuções concluídas! Logs salvos em $logs."