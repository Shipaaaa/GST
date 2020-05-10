#!/bin/sh

test_data_output_path="test_data"
mkdir -p $test_data_output_path

results_output_path="results/sequential_execution"
mkdir -p $results_output_path

gcc sequential_execution.c utils.c -o sequential_execution.out &&
  "$PWD"/sequential_execution.out ./$test_data_output_path/1mb ./$results_output_path/1mb &&
  "$PWD"/sequential_execution.out ./$test_data_output_path/10mb ./$results_output_path/10mb &&
  "$PWD"/sequential_execution.out ./$test_data_output_path/20mb ./$results_output_path/20mb &&
  "$PWD"/sequential_execution.out ./$test_data_output_path/30mb ./$results_output_path/30mb &&
  "$PWD"/sequential_execution.out ./$test_data_output_path/40mb ./$results_output_path/40mb &&
  "$PWD"/sequential_execution.out ./$test_data_output_path/50mb ./$results_output_path/50mb &&
  "$PWD"/sequential_execution.out ./$test_data_output_path/100mb ./$results_output_path/100mb &&
  "$PWD"/sequential_execution.out ./$test_data_output_path/500mb ./$results_output_path/500mb &&
  "$PWD"/sequential_execution.out ./$test_data_output_path/1000mb ./$results_output_path/1000mb &&
  "$PWD"/sequential_execution.out ./$test_data_output_path/1600mb ./$results_output_path/1600mb &&
  "$PWD"/sequential_execution.out ./$test_data_output_path/3200mb ./$results_output_path/3200mb
