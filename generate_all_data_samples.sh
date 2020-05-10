#!/bin/sh

test_data_output_path="test_data"
mkdir -p $test_data_output_path

gcc generator.c utils.c -o generator.out &&
  "$PWD"/generator.out 1 ./$test_data_output_path/1mb &&
  "$PWD"/generator.out 10 ./$test_data_output_path/10mb &&
  "$PWD"/generator.out 20 ./$test_data_output_path/20mb &&
  "$PWD"/generator.out 30 ./$test_data_output_path/30mb &&
  "$PWD"/generator.out 40 ./$test_data_output_path/40mb &&
  "$PWD"/generator.out 50 ./$test_data_output_path/50mb &&
  "$PWD"/generator.out 100 ./$test_data_output_path/100mb &&
  "$PWD"/generator.out 500 ./$test_data_output_path/500mb &&
  "$PWD"/generator.out 1000 ./$test_data_output_path/1000mb &&
  "$PWD"/generator.out 1600 ./$test_data_output_path/1600mb &&
  "$PWD"/generator.out 3200 ./$test_data_output_path/3200mb
