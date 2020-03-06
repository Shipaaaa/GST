/*
 * Последовательная реализация.
 * Вариант: 7
 * Бизнес логика: Умножение матрицы на вектор
 *
 * Данные генерируются утилитой, принимающей в качестве параметров
 * размер данных для обработки в мегабайтах
 * и имя файла (TCP хост-порт) куда будут выгружены данные.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG 0
#define LOG 1

void change_printf_to_error();

void change_printf_to_log();

void showMessage(char *message);

void showError(char *errorMessage);

int main(int argc, char *argv[], char *argp[]) {
    if (argc < 3) {
        showError("Вы не ввели размер данных и имя файла.");
        return -1;
    }

    int size_in_mb = atoi(argv[1]);
    char *output_file_name = argv[2];
    if (LOG) printf("size: %dmb, output file name: %s.\n", size_in_mb, output_file_name);

    int size_in_bytes = size_in_mb * 1024 * 1024;
    if (LOG) printf("size_in_bytes: %d\n", size_in_bytes);

    int count_of_elements = size_in_bytes / sizeof(int);
    if (LOG) printf("count_of_elements: %d, size_of_int: %ld\n", count_of_elements, sizeof(int));

    int matrix_size = (-1 + sqrt(1 - 4 * (-count_of_elements))) / 2;
    if (LOG) printf("matrix_size: %d\n\n", matrix_size);

    FILE *output_file = NULL;
    output_file = fopen(output_file_name, "w+");

    if (output_file == NULL) {
        showError("output file not found!");
        return -1;
    }

    fprintf(output_file, "%d\n", matrix_size);

    #pragma region [rgba(155, 89, 182, 0.15)]
    // generate_matrix
    for (int i = 0; i < matrix_size; i++) {
        for (int j = 0; j < matrix_size; j++) {
            int num = rand();
            fprintf(output_file, "%d ", num);
            if (DEBUG) printf("%d ", num);
        }
        fprintf(output_file, "\n");
        if (DEBUG) printf("\n");
    }
    fprintf(output_file, "\n");
    if (DEBUG) printf("\n");
    #pragma endregion

    #pragma region [rgba(155, 89, 182, 0.15)]
    // generate_vector
    for (int i = 0; i < matrix_size; i++) {
        int num = rand();
        fprintf(output_file, "%d ", num);
        if (DEBUG) printf("%d ", num);
    }
    fprintf(output_file, "\n");
    if (DEBUG) printf("\n");
    #pragma endregion

    fclose(output_file);
    return 0;
};

void change_printf_to_error() {
    printf("\033[0;31m");
}

void change_printf_to_log() {
    printf("\033[0m");
}

void showMessage(char *message) {
    printf("%s", message);
}

void showError(char *errorMessage) {
    change_printf_to_error();
    printf("%s\n", errorMessage);
    change_printf_to_log();
}
