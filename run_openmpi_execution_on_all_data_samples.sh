#!/bin/sh
set -e

test_data_output_path="test_data"
mkdir -p $test_data_output_path

results_output_path="results/openmpi"
mkdir -p $results_output_path

mpicc openmpi.c utils.c -o openmpi.out

# shellcheck disable=SC2045
for filename in $(ls -Sr ./$test_data_output_path); do
  mpirun "$PWD"/openmpi.out ./$test_data_output_path/"${filename##*/}" ./$results_output_path/"${filename##*/}"
done
