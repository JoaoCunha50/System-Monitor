#!/bin/bash
# script.sh

# Caminho para o executável do cliente
CLIENT="./client"

# Envia várias tarefas para o servidor
$CLIENT execute 5000 -u "sleep 6" &
sleep 0.25
$CLIENT execute 3000 -u "sleep 5" &
sleep 0.25
$CLIENT execute 4000 -u "sleep 4" &
sleep 0.25
$CLIENT execute 2000 -u "sleep 2" &
sleep 0.25
$CLIENT execute 1000 -u "sleep 1" &
sleep 0.25
$CLIENT execute 1000 -u "echo 'Hello World'" &

# Aguarda todos os processos em background terminarem
wait