#!/bin/sh

test_data_output_path="test_data"
mkdir -p $test_data_output_path

results_output_path="results/openmpi"
mkdir -p $results_output_path

mpicc openmpi.c utils.c -o openmpi.out &&
  mpirun "$PWD"/openmpi.out ./$test_data_output_path/1mb ./$results_output_path/1mb &&
  mpirun "$PWD"/openmpi.out ./$test_data_output_path/10mb ./$results_output_path/10mb &&
  mpirun "$PWD"/openmpi.out ./$test_data_output_path/20mb ./$results_output_path/20mb &&
  mpirun "$PWD"/openmpi.out ./$test_data_output_path/30mb ./$results_output_path/30mb &&
  mpirun "$PWD"/openmpi.out ./$test_data_output_path/40mb ./$results_output_path/40mb &&
  mpirun "$PWD"/openmpi.out ./$test_data_output_path/50mb ./$results_output_path/50mb &&
  mpirun "$PWD"/openmpi.out ./$test_data_output_path/100mb ./$results_output_path/100mb &&
  mpirun "$PWD"/openmpi.out ./$test_data_output_path/500mb ./$results_output_path/500mb &&
  mpirun "$PWD"/openmpi.out ./$test_data_output_path/1000mb ./$results_output_path/1000mb &&
  mpirun "$PWD"/openmpi.out ./$test_data_output_path/1600mb ./$results_output_path/1600mb &&
  mpirun "$PWD"/openmpi.out ./$test_data_output_path/3200mb ./$results_output_path/3200mb
