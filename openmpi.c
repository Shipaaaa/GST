/*
 * Ускорение программы с помощью OpenMPI
 * Вариант: 7
 * Бизнес логика: Умножение матрицы на вектор
 *
 * Считывание данных происходит из файла.
 * Программа выполняет бизнес-логику и записывает результат в выходной файл.
 * В конце файла с результатами сохраняется информация о времени выполнения вычислений
 * и размере обработанных данных.
 *
 * Запуск: mpicc openmpi.c utils.c -o openmpi.out && \
mpirun $PWD/openmpi.out ./test_data/1mb ./results/openmpi/1mb
 */

#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "/usr/local/Cellar/open-mpi/4.0.3/include/mpi.h"

#define DEBUG 0
#define LOG 1

#define MASTER_PROC 0

void read_matrix(FILE *input_file, int *matrix, long matrix_size);

void read_vector(FILE *input_file, int *vector, long vector_length);

void print_vector(const int *vector, long vector_length);

void calc_answer(const int *matrix_row, const int *vector, int *answer_buffer, int row_count, int vector_length);

void save_answer(FILE *output_file, const int *answer, long answer_length);

int main(int argc, char *argv[], char *argp[]) {

    int result_code, mpi_procs_count, current_proc;

    int matrix_size, vector_length, row_count;
    double begin, end;

    int *matrix;
    int *matrix_rows;
    int *vector;
    int *answer;
    int *answer_buffer;

    char *input_file_name;
    char *output_file_name;

    FILE *input_file = NULL;
    FILE *output_file = NULL;

    if ((result_code = MPI_Init(&argc, &argv)) != MPI_SUCCESS) {
        fprintf(stderr, "Error starting MPI program. Terminating.\n");
        MPI_Abort(MPI_COMM_WORLD, result_code);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &mpi_procs_count);
    MPI_Comm_rank(MPI_COMM_WORLD, &current_proc);

    if (LOG) printf("Processor %d of %d\n", current_proc, mpi_procs_count);

    if (current_proc == MASTER_PROC) {
        if (argc < 3) {
            input_file_name = "input_file";
            output_file_name = "output_file";
        } else {
            input_file_name = argv[1];
            output_file_name = argv[2];
        }

        if (LOG) printf("input file name: %s,\noutput file name: %s.\n\n", input_file_name, output_file_name);

        input_file = NULL;
        input_file = fopen(input_file_name, "r+");
        if (input_file == NULL) {
            showError("input file not found!");
            return -1;
        }

        fscanf(input_file, "%d", &matrix_size);
        if (LOG) printf("matrix_size: %d \n", matrix_size);
        vector_length = matrix_size;
    }

    if (DEBUG) printf("Before MPI_Bcast size. Processor: %d of %d\n", current_proc, mpi_procs_count);
    MPI_Bcast(&matrix_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&vector_length, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int alloc_rest_rows = matrix_size;
    for (int i = 0; i < current_proc; i++) {
        alloc_rest_rows = alloc_rest_rows - alloc_rest_rows / (mpi_procs_count - i);
    }

    row_count = alloc_rest_rows / (mpi_procs_count - current_proc);

    matrix = (int *) calloc(matrix_size * matrix_size, sizeof(int));
    matrix_rows = (int *) calloc(row_count * matrix_size, sizeof(int));
    vector = (int *) calloc(vector_length, sizeof(int));
    answer = (int *) calloc(vector_length, sizeof(int));
    answer_buffer = (int *) calloc(row_count, sizeof(int));

    if (current_proc == MASTER_PROC) {
        if (DEBUG) printf("read_matrix. Processor: %d of %d\n", current_proc, mpi_procs_count);
        read_matrix(input_file, matrix, matrix_size);
        read_vector(input_file, vector, vector_length);
        begin = MPI_Wtime();
    }

    if (DEBUG) printf("Before MPI_Bcast vector. Processor: %d of %d\n", current_proc, mpi_procs_count);
    MPI_Bcast(vector, vector_length, MPI_INT, 0, MPI_COMM_WORLD);

    int *send_num = (int *) calloc(mpi_procs_count, sizeof(int));
    int *send_ind = (int *) calloc(mpi_procs_count, sizeof(int));

    int send_rest_rows = matrix_size;

    int send_row_count = matrix_size / mpi_procs_count;
    send_num[0] = send_row_count * matrix_size;
    send_ind[0] = 0;

    for (int i = 1; i < mpi_procs_count; i++) {
        send_rest_rows -= send_row_count;
        send_row_count = send_rest_rows / (mpi_procs_count - i);
        send_num[i] = send_row_count * matrix_size;
        send_ind[i] = send_ind[i - 1] + send_num[i - 1];
    }

    MPI_Scatterv(
            matrix, send_num, send_ind, MPI_INT,
            matrix_rows, send_num[current_proc], MPI_INT,
            0, MPI_COMM_WORLD
    );

    free(send_num);
    free(send_ind);

    if (DEBUG) printf("Before calc. Processor: %d of %d\n", current_proc, mpi_procs_count);
    calc_answer(matrix_rows, vector, answer_buffer, row_count, vector_length);

    int *receive_num = (int *) calloc(mpi_procs_count, sizeof(int));
    int *receive_ind = (int *) calloc(mpi_procs_count, sizeof(int));

    int result_rest_rows = matrix_size;

    receive_ind[0] = 0;
    receive_num[0] = matrix_size / mpi_procs_count;

    for (int i = 1; i < mpi_procs_count; i++) {
        result_rest_rows -= receive_num[i - 1];
        receive_num[i] = result_rest_rows / (mpi_procs_count - i);
        receive_ind[i] = receive_ind[i - 1] + receive_num[i - 1];
    }

    MPI_Allgatherv(
            answer_buffer, receive_num[current_proc], MPI_INT,
            answer, receive_num, receive_ind, MPI_INT,
            MPI_COMM_WORLD
    );

    free(receive_num);
    free(receive_ind);

    if (DEBUG) printf("Before output. Processor: %d of %d\n", current_proc, mpi_procs_count);
    if (current_proc == MASTER_PROC) {
        end = MPI_Wtime();

        double time_spent_in_sec = end - begin;

        output_file = NULL;
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
            printf("matrix_size: %d\n", matrix_size);
            printf("size_of_input_data: %ld\n", size_of_input_data);
            printf("size_of_input_data_in_mb: %f\n\n", size_of_input_data_in_mb);
        }

        fclose(input_file);
        fclose(output_file);
    }

    free(matrix);
    free(matrix_rows);
    free(vector);
    free(answer);

    if (DEBUG) printf("Before MPI_Finalize. Processor: %d of %d\n", current_proc, mpi_procs_count);
    MPI_Finalize();

    return 0;
}

void read_matrix(FILE *input_file, int *matrix, long matrix_size) {
    if (DEBUG) printf("read_matrix:\n");
    for (long i = 0; i < matrix_size; i++) {
        for (long j = 0; j < matrix_size; j++) {
            fscanf(input_file, "%d", &matrix[i * matrix_size + j]);
            if (DEBUG) printf("%d ", matrix[i * matrix_size + j]);
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
    printf("read_vector:\n");
    for (long i = 0; i < vector_length; i++) {
        printf("%d ", vector[i]);
    }
    printf("\n\n");
}

void calc_answer(const int *matrix_row, const int *vector, int *answer_buffer, int row_count, int vector_length) {
    for (int i = 0; i < row_count; i++) {
        for (int j = 0; j < vector_length; j++)
            answer_buffer[i] += matrix_row[i * vector_length + j] * vector[j];
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
