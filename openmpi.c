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

void save_answer(FILE *output_file, const int *answer, long answer_length);

int main(int argc, char *argv[], char *argp[]) {

    int result_code, mpi_procs_count, current_proc;

    int matrix_size, vector_length;
    double begin, end;
    int matrix_part_size;

    int *matrix;
    int *vector;
    int *answer;
    int *matrix_buffer;

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

        matrix_part_size = (matrix_size * matrix_size / mpi_procs_count) + 1;
        if (LOG) printf("matrix_part_size: %d \n", matrix_part_size);
    }

    printf("Before MPI_Bcast size. Procc: %d of %d\n", current_proc, mpi_procs_count);
    MPI_Bcast(&matrix_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&vector_length, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&matrix_part_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    printf("Before calloc matrix. Procc: %d of %d\n", current_proc, mpi_procs_count);
    matrix = (int *) calloc(matrix_size * matrix_size, sizeof(int));
    printf("Before calloc vector. Procc: %d of %d\n", current_proc, mpi_procs_count);
    vector = (int *) calloc(vector_length, sizeof(int));
    printf("Before calloc answer. Procc: %d of %d\n", current_proc, mpi_procs_count);
    answer = (int *) calloc(vector_length, sizeof(int));

    printf("Before calloc matrix_buffer. Procc: %d of %d\n", current_proc, mpi_procs_count);
    matrix_buffer = (int *) calloc(matrix_size * matrix_size, sizeof(int));

    if (current_proc == MASTER_PROC) {
        printf("read_matrix. Procc: %d of %d\n", current_proc, mpi_procs_count);
        read_matrix(input_file, matrix, matrix_size);
        read_vector(input_file, vector, vector_length);
        begin = MPI_Wtime();
    }

    printf("before MPI_Bcast vector. Procc: %d of %d\n", current_proc, mpi_procs_count);
    MPI_Bcast(vector, vector_length, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(answer, vector_length, MPI_INT, 0, MPI_COMM_WORLD);

    printf("before MPI_Scatter. Procc: %d of %d\n", current_proc, mpi_procs_count);
    if (MPI_Scatter(
            matrix, matrix_part_size, MPI_INT,
            matrix_buffer, matrix_part_size, MPI_INT,
            0, MPI_COMM_WORLD
    ) != MPI_SUCCESS) {
        MPI_Finalize();
        perror("Scatter error");
        exit(1);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    printf("before calc. Procc: %d of %d\n", current_proc, mpi_procs_count);
    int sum = 0;
    for (long i = 0; i < vector_length; i++) {
        for (long j = 0; j < vector_length; j++) {
            sum += matrix_buffer[j * vector_length + i] * vector[j];
        }
        answer[i] = sum;
        sum = 0;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    printf("before free. Procc: %d of %d\n", current_proc, mpi_procs_count);
    free(matrix_buffer);

    printf("before output. Procc: %d of %d\n", current_proc, mpi_procs_count);
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
    free(vector);
    free(answer);

    printf("before MPI_Finalize. Procc: %d of %d\n", current_proc, mpi_procs_count);
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
