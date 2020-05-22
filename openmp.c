/* 
 * Ускорение программы с помощью OpenMP
 * Вариант: 7
 * Бизнес логика: Умножение матрицы на вектор
 * 
 * Считывание данных происходит из файла.
 * Программа выполняет бизнес-логику и записывает результат в выходной файл.
 * В конце файла с результатами сохраняется информация о времени выполнения вычислений 
 * и размере обработанных данных.
 *
 * Запуск: g++-9 -fopenmp openmp.c utils.c -o openmp.out && \
$PWD/openmp.out ./test_data/1mb ./results/openmp/1mb
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"
#include "/usr/local/Cellar/gcc/9.3.0_1/lib/gcc/9/gcc/x86_64-apple-darwin18/9.3.0/include/omp.h"

#define DEBUG 0
#define LOG 1

void read_matrix(FILE *input_file, int **matrix, long matrix_size);

void read_vector(FILE *input_file, int *vector, long vector_length);

void print_vector(const int *vector, long vector_length);

void calc_answer(int **matrix, const int *vector, int *answer, long vector_length);

void save_answer(FILE *output_file, const int *answer, long answer_length);

int main(int argc, char *argv[], char *argp[]) {

    const char *input_file_name;
    const char *output_file_name;

    if (argc < 3) {
        input_file_name = "input_file";
        output_file_name = "output_file";
    } else {
        input_file_name = argv[1];
        output_file_name = argv[2];
    }

    if (LOG) printf("input file name: %s,\noutput file name: %s.\n\n", input_file_name, output_file_name);

    FILE *input_file = NULL;
    input_file = fopen(input_file_name, "r+");
    if (input_file == NULL) {
        showError("input file not found!");
        return -1;
    }

    long matrix_size;
    fscanf(input_file, "%ld", &matrix_size);
    if (LOG) printf("matrix_size: %ld \n", matrix_size);

    int **matrix = (int **) calloc(matrix_size, sizeof(int *));
    for (long i = 0; i < matrix_size; i++) matrix[i] = (int *) calloc(matrix_size, sizeof(int));
    read_matrix(input_file, matrix, matrix_size);

    long vector_length = matrix_size;
    int *vector = (int *) calloc(vector_length, sizeof(int));
    read_vector(input_file, vector, vector_length);

    int *answer = (int *) calloc(vector_length, sizeof(int));
    double begin = omp_get_wtime();
    calc_answer(matrix, vector, answer, vector_length);
    double end = omp_get_wtime();

    double time_spent_in_sec = end - begin;

    FILE *output_file = NULL;
    output_file = fopen(output_file_name, "w+");

    if (output_file == NULL) {
        showError("output file not found!");
        return -1;
    }

    save_answer(output_file, answer, vector_length);

    fprintf(output_file, "time: %f\n", time_spent_in_sec);
    if (LOG) printf("time_spent_in_sec: %f\n", time_spent_in_sec);

    long size_of_input_data = (long) ((matrix_size * matrix_size + matrix_size) * sizeof(int));
    double size_of_input_data_in_mb = (double) size_of_input_data / 1024 / 1024;
    fprintf(output_file, "size: %f\n", size_of_input_data_in_mb);
    if (LOG) {
        printf("matrix_size: %ld\n", matrix_size);
        printf("size_of_input_data: %ld\n", size_of_input_data);
        printf("size_of_input_data_in_mb: %f\n\n", size_of_input_data_in_mb);
    }

    fclose(input_file);
    fclose(output_file);

    for (int i = 0; i < matrix_size; i++) free(matrix[i]);
    free(matrix);
    free(vector);
    free(answer);

    return 0;
}

void read_matrix(FILE *input_file, int **matrix, long matrix_size) {
    if (DEBUG) printf("read_matrix:\n");
    for (long i = 0; i < matrix_size; i++) {
        for (long j = 0; j < matrix_size; j++) {
            fscanf(input_file, "%d", &matrix[i][j]);
            if (DEBUG) printf("%d ", matrix[i][j]);
        }
        if (DEBUG) printf("\n");
    }

    if (DEBUG) printf("\n");
}

void read_vector(FILE *input_file, int *vector, long vector_length) {
    for (long i = 0; i < vector_length; i++) {
        fscanf(input_file, "%d", &vector[i]);
    }

    if (DEBUG) print_vector(vector, vector_length);
}

void print_vector(const int *vector, long vector_length) {
    if (DEBUG) printf("read_vector:\n");
    for (long i = 0; i < vector_length; i++) {
        printf("%d ", vector[i]);
    }
    printf("\n\n");
}

void calc_answer(int **matrix, const int *vector, int *answer, const long vector_length) {
#pragma omp parallel num_threads(8) shared(matrix, vector)
    {
        long i, j;
        int *answer_private = (int *) calloc(vector_length, sizeof(int));

        for (i = 0; i < vector_length; i++) {
#pragma omp for
            for (j = 0; j < vector_length; j++) {
                answer_private[i] += matrix[j][i] * vector[j];
            }
        }
#pragma omp critical
        {
            for (i = 0; i < vector_length; i++) {
                answer[i] += answer_private[i];
            }
        }
        free(answer_private);
    }
}

void save_answer(FILE *output_file, const int *answer, long answer_length) {
    if (DEBUG) printf("save_answer:\n");
    fprintf(output_file, "result:\n");

    for (long i = 0; i < answer_length; i++) {
        fprintf(output_file, "%d ", answer[i]);
        if (DEBUG) printf("%d ", answer[i]);
    }
    fprintf(output_file, "\n");
    if (DEBUG) printf("\n\n");
}
