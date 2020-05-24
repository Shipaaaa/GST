#!/bin/sh
set -e

test_data_output_path="test_data"
mkdir -p $test_data_output_path

results_output_path="results/sequential_execution"
mkdir -p $results_output_path

gcc sequential_execution.c utils.c -o sequential_execution.out

# shellcheck disable=SC2045
for filename in $(ls -Sr ./$test_data_output_path); do
  "$PWD"/sequential_execution.out ./$test_data_output_path/"${filename##*/}" ./$results_output_path/"${filename##*/}"
done
