#!/bin/sh
set -e

test_data_output_path="test_data"
mkdir -p $test_data_output_path

results_output_path="results/cuda"
mkdir -p $results_output_path

nvcc cuda.cu -o cuda.out

# shellcheck disable=SC2045
for filename in $(ls -Sr ./$test_data_output_path); do
  "$PWD"/cuda.out ./$test_data_output_path/"${filename##*/}" ./$results_output_path/"${filename##*/}"
done
