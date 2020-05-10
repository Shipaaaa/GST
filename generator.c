/*
 * Последовательная реализация.
 * Вариант: 7
 * Бизнес логика: Умножение матрицы на вектор
 *
 * Данные генерируются утилитой, принимающей в качестве параметров
 * размер данных для обработки в мегабайтах
 * и имя файла (TCP хост-порт) куда будут выгружены данные.
 *
 * Запуск: gcc generator.c utils.c -o generator.out && \
./generator.out 1 $PWD/1mb
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utils.h"

#define DEBUG 0
#define LOG 1

void generate_matrix(FILE *output_file, long matrix_size);

void generate_vector(FILE *output_file, long matrix_size);

int main(int argc, char *argv[], char *argp[]) {

    if (argc < 3) {
        showError("Вы не ввели размер данных и имя файла.");
        return -1;
    }

    double size_in_mb = atof(argv[1]);
    char *output_file_name = argv[2];
    if (LOG) printf("size: %5.2fmb, output file name: %s.\n", size_in_mb, output_file_name);

    double size_in_bytes = size_in_mb * 1024 * 1024;
    if (LOG) printf("size_in_bytes: %5.2f\n", size_in_bytes);

    long count_of_elements = (long) (size_in_bytes / sizeof(int));
    if (LOG) printf("count_of_elements: %ld, size_of_int: %ld\n", count_of_elements, sizeof(int));

    long matrix_size = (long) ((-1 + sqrt(1 - 4 * (-count_of_elements))) / 2);
    if (LOG) printf("matrix_size: %ld\n\n", matrix_size);

    FILE *output_file = NULL;
    output_file = fopen(output_file_name, "w+");
    if (output_file == NULL) {
        showError("output file not found!");
        return -1;
    }

    fprintf(output_file, "%ld\n", matrix_size);
    generate_matrix(output_file, matrix_size);
    generate_vector(output_file, matrix_size);

    fclose(output_file);
    return 0;
}

void generate_matrix(FILE *output_file, const long matrix_size) {
    for (long i = 0; i < matrix_size; i++) {
        for (long j = 0; j < matrix_size; j++) {
            int num = rand(); // NOLINT(cert-msc30-c, cert-msc50-cpp)
            fprintf(output_file, "%d ", num);
            if (DEBUG) printf("%d ", num);
        }
        fprintf(output_file, "\n");
        if (DEBUG) printf("\n");
    }
    fprintf(output_file, "\n");
    if (DEBUG) printf("\n");
}

void generate_vector(FILE *output_file, const long matrix_size) {
    for (long i = 0; i < matrix_size; i++) {
        int num = rand(); // NOLINT(cert-msc30-c, cert-msc50-cpp)
        fprintf(output_file, "%d ", num);
        if (DEBUG) printf("%d ", num);
    }
    fprintf(output_file, "\n");
    if (DEBUG) printf("\n");
}