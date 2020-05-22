#!/bin/sh

test_data_output_path="test_data"
mkdir -p $test_data_output_path

results_output_path="results/openmp"
mkdir -p $results_output_path

g++-9 -fopenmp openmp.c utils.c -o openmp.out &&
  "$PWD"/openmp.out ./$test_data_output_path/1mb ./$results_output_path/1mb &&
  "$PWD"/openmp.out ./$test_data_output_path/10mb ./$results_output_path/10mb &&
  "$PWD"/openmp.out ./$test_data_output_path/20mb ./$results_output_path/20mb &&
  "$PWD"/openmp.out ./$test_data_output_path/30mb ./$results_output_path/30mb &&
  "$PWD"/openmp.out ./$test_data_output_path/40mb ./$results_output_path/40mb &&
  "$PWD"/openmp.out ./$test_data_output_path/50mb ./$results_output_path/50mb &&
  "$PWD"/openmp.out ./$test_data_output_path/100mb ./$results_output_path/100mb &&
  "$PWD"/openmp.out ./$test_data_output_path/500mb ./$results_output_path/500mb &&
  "$PWD"/openmp.out ./$test_data_output_path/1000mb ./$results_output_path/1000mb &&
  "$PWD"/openmp.out ./$test_data_output_path/1600mb ./$results_output_path/1600mb &&
  "$PWD"/openmp.out ./$test_data_output_path/3200mb ./$results_output_path/3200mb
