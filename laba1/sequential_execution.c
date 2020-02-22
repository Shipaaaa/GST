/* 
 * Последовательная реализация.
 * Вариант: 7
 * Бизнес логика: Умножение матрицы на вектор
 * 
 * Считывание данных происходит из файла.
 * Программа выполняет бизнес-логику и записывает результат в выходной файл.
 * В конце файла с результатами сохраняется информация о времени выполнения вычислений 
 * и размере обработанных данных.
 * 
 * Запуск: crun ./generator 5 input_data && crun ./sequential_execution input_data result
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DEBUG 0
#define LOG 1

void change_printf_to_error();

void change_printf_to_log();

void showMessage(char *message);

void showError(char *errorMessage);

void print_vector(int vector_length, const int *vector);

int main(int argc, char *argv[], char *argp[]) {
    char *input_file_name;
    char *output_file_name;

    if (argc < 3) {
        input_file_name = "input_file";
        output_file_name = "output_file";
    } else {
        input_file_name = argv[1];
        output_file_name = argv[2];
    }

    if (LOG) printf("input file name: %s, output file name: %s.\n", input_file_name, output_file_name);

    FILE *input_file = NULL;
    input_file = fopen(input_file_name, "r+");

    if (input_file == NULL) {
        showError("input file not found!");
        return -1;
    }

    int matrix_size;
    // region get_matrix_size
    fscanf(input_file, "%d", &matrix_size);
    if (LOG) printf("matrix_size: %d \n", matrix_size);
    // endregion
    
    int matrix[matrix_size][matrix_size];
    #pragma region [rgba(155, 89, 182, 0.15)]
    // read_matrix
    memset(matrix, 0, matrix_size * matrix_size * sizeof(int));
    
    for (int i = 0; i < matrix_size; i++) {
        for (int j = 0; j < matrix_size; j++) {
            fscanf(input_file, "%d", &matrix[i][j]);
            if (DEBUG) printf("%d ", matrix[i][j]);
        }
        if (DEBUG) printf("\n");
    }
    #pragma endregion

    int vector_length = matrix_size;

    int vector[vector_length];
    #pragma region [rgba(155, 89, 182, 0.15)]
    // read_matrix
    memset(vector, 0, vector_length * sizeof(int));

    for (int i = 0; i < vector_length; i++) {
        fscanf(input_file, "%d", &vector[i]);
    }

    if (DEBUG) print_vector(vector_length, vector);
    #pragma endregion

    /* ЗАПУСК СЕКУНДАМЕРА */
    clock_t begin = clock();

    int answer[vector_length];
    #pragma region [rgba(155, 89, 182, 0.15)]
    // calc_answer
    memset(answer, 0, vector_length * sizeof(int));

    for (int i = 0; i < vector_length; i++) {
        for (int j = 0; j < vector_length; j++) {
            answer[j] += matrix[j][i] * vector[i];
        }
    }
    #pragma endregion

    /* ОСТАНОВКА СЕКУНДАМЕРА */
    clock_t end = clock();
    double time_spent_in_sec = (double)(end - begin) / CLOCKS_PER_SEC;

    FILE *output_file = NULL;
    output_file = fopen(output_file_name, "w+");

    if (output_file == NULL) {
        showError("output file not found!");
        return -1;
    }

    #pragma region [rgba(155, 89, 182, 0.15)]
    // save_answer
    fprintf(output_file, "result:\n");
    for (int i = 0; i < vector_length; i++) {
        fprintf(output_file, "%d ", answer[i]);
        if (DEBUG) printf("%d ", answer[i]);
    }
    fprintf(output_file, "\n");
    if (DEBUG) printf("\n");
    #pragma endregion

    fprintf(output_file, "time: %f\n", time_spent_in_sec);
    if (LOG) printf("time_spent_in_sec: %f\n", time_spent_in_sec);

    int size_of_input_data = (matrix_size * matrix_size + matrix_size) * sizeof(int);
    double size_of_input_data_in_mb = (double)size_of_input_data / 1024 / 1024;
    fprintf(output_file, "size: %f\n", size_of_input_data_in_mb);
    if (LOG) {
        printf("matrix_size: %d\n", matrix_size);
        printf("size_of_input_data: %d\n", size_of_input_data);
        printf("size_of_input_data_in_mb: %f\n\n", size_of_input_data_in_mb);
    }

    fclose(input_file);
    fclose(output_file);

    return 0;
}

void print_vector(int vector_length, const int *vector) {
    for (int i = 0; i < vector_length; i++) {
        printf("%d ", vector[i]);
    }
    printf("\n");
}

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
    printf("%s", errorMessage);
    change_printf_to_log();
}
