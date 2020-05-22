#!/bin/sh
#1. Прежде всего надо инициализировать переменные, которые разрешают запуск MPI от root:
#  export OMPI_ALLOW_RUN_AS_ROOT=1
#  export OMPI_ALLOW_RUN_AS_ROOT_CONFIRM=1
#
#2. Компиляция (std=c99, чтобы не ругался на for)
#    mpicc mixcolumns.c -o mc --std=c99
#
#3. Запуск
#    Общий вид: mpirun -np <кол-во потоков> -H <host1>:<количество ядер с хоста 1>,<host2>:<количество ядер с хоста 2> <имя программы> <аргументы>
#    Например, запускаем на двух хостах, беря по 4 ядра с каждого:
#    ┆   mpirun -np 8 -H 192.168.12.62:4,192.168.12.63:4 mc -f ../data/outG_1 -o OutT

test_data_output_path="test_data"
mkdir -p $test_data_output_path

results_output_path="results/openmpi"
mkdir -p $results_output_path

mpicc openmpi.c utils.c -o openmpi.out

for i in $(ls -Sr ./$test_data_output_path/); do
  mpirun -np 8 -H 192.168.12.62:4,192.168.12.63:4 "$PWD"/openmpi.out ./$test_data_output_path/$i ./$results_output_path/$i
done
