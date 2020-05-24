#!/bin/sh
set -e

test_data_output_path="test_data"
mkdir -p $test_data_output_path

results_output_path="results/openmp"
mkdir -p $results_output_path

gcc-9 -fopenmp openmp.c utils.c -o openmp.out

# shellcheck disable=SC2045
for filename in $(ls -Sr ./$test_data_output_path); do
  "$PWD"/openmp.out ./$test_data_output_path/"${filename##*/}" ./$results_output_path/"${filename##*/}"
done
