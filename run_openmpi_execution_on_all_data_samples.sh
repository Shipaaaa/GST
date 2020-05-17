#!/bin/sh

test_data_output_path="test_data"
mkdir -p $test_data_output_path

results_output_path="results/openmpi"
mkdir -p $results_output_path

mpicc openmpi.c utils.c -o openmpi.out &&
  mpirun "$PWD"/openmpi.out ./$test_data_output_path/10mb ./$results_output_path/10mb --use-hwthreads-as-cpus &&
  mpirun "$PWD"/openmpi.out ./$test_data_output_path/20mb ./$results_output_path/20mb --use-hwthreads-as-cpus &&
  mpirun "$PWD"/openmpi.out ./$test_data_output_path/30mb ./$results_output_path/30mb --use-hwthreads-as-cpus &&
  mpirun "$PWD"/openmpi.out ./$test_data_output_path/40mb ./$results_output_path/40mb --use-hwthreads-as-cpus &&
  mpirun "$PWD"/openmpi.out ./$test_data_output_path/50mb ./$results_output_path/50mb --use-hwthreads-as-cpus &&
  mpirun "$PWD"/openmpi.out ./$test_data_output_path/100mb ./$results_output_path/100mb --use-hwthreads-as-cpus &&
  mpirun "$PWD"/openmpi.out ./$test_data_output_path/500mb ./$results_output_path/500mb --use-hwthreads-as-cpus &&
  mpirun "$PWD"/openmpi.out ./$test_data_output_path/1000mb ./$results_output_path/1000mb --use-hwthreads-as-cpus &&
  mpirun "$PWD"/openmpi.out ./$test_data_output_path/1600mb ./$results_output_path/1600mb --use-hwthreads-as-cpus &&
  mpirun "$PWD"/openmpi.out ./$test_data_output_path/3200mb ./$results_output_path/3200mb --use-hwthreads-as-cpus
