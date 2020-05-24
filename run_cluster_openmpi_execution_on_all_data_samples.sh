#!/bin/sh
set -e
# Запуск
#    Общий вид: mpirun -np <кол-во потоков> -H <host1>:<количество ядер с хоста 1>,<host2>:<количество ядер с хоста 2> <имя программы> <аргументы>
#    Например, запускаем на двух хостах, беря по 4 ядра с каждого:
#    ┆   mpirun -np 8 -H 192.168.12.62:4,192.168.12.63:4 mc -f ../data/outG_1 -o OutT

# Разрешаем запуск MPI от root
export OMPI_ALLOW_RUN_AS_ROOT=1
export OMPI_ALLOW_RUN_AS_ROOT_CONFIRM=1

test_data_output_path="test_data"
mkdir -p $test_data_output_path

results_output_path="results/openmpi"
mkdir -p $results_output_path

mpicc openmpi.c utils.c -o openmpi.out --std=c11

# shellcheck disable=SC2045
for filename in $(ls -Sr ./$test_data_output_path); do
  mpirun -np 8 -H 192.168.12.62:4,192.168.12.63:4 "$PWD"/openmpi.out ./$test_data_output_path/"${filename##*/}" ./$results_output_path/"${filename##*/}"
done
