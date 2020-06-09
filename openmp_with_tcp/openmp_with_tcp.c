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
 * Запуск: g++-9 -fopenmp openmp_with_tcp.c utils.c -o openmp_with_tcp.out && \
$PWD/openmp_with_tcp.out 127.0.0.1 ./results/openmp_with_tcp/1mb
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "utils.h"
#include "omp.h"
#include "../../../../../../../usr/local/Cellar/gcc/9.3.0_1/lib/gcc/9/gcc/x86_64-apple-darwin18/9.3.0/include/omp.h"

#define PORT 8080
#define DEBUG 0
#define LOG 1

int create_socket(const char *ip_address);

int *receive_from_server(int descriptor, size_t array_size, int buffer_size);

void print_matrix(const int *matrix, long matrix_size);

void print_vector(const int *vector, long vector_length);

void calc_answer(const int *matrix, const int *vector, int *answer, long vector_length);

void save_answer(FILE *output_file, const int *answer, long answer_length);

int main(int argc, char *argv[]) {

    if (argc < 3) {
        showError("Вы не ввели ip адрес сервера и названия файла для сохранения результата");
        return -1;
    }

    const char *ip_address = argv[1];
    const char *output_file_name = argv[2];

    if (LOG) printf("ip_address: %s,\noutput file name: %s.\n\n", ip_address, output_file_name);

    int socket_descriptor = create_socket(ip_address);
    if (socket_descriptor == -1) return -1;

    long matrix_size;
    read(socket_descriptor, &matrix_size, sizeof(long));
    if (LOG) printf("matrix_size: %ld \n", matrix_size);

    int *matrix = receive_from_server(socket_descriptor, matrix_size * matrix_size, 255);
    if (DEBUG) print_matrix(matrix, matrix_size);

    long vector_length = matrix_size;
    int *vector = receive_from_server(socket_descriptor, vector_length, 255);

    if (DEBUG) print_vector(vector, vector_length);

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

    fclose(output_file);
    close(socket_descriptor);

    free(matrix);
    free(vector);

    return 0;
}

int create_socket(const char *ip_address) {
    int socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_descriptor < 0) {
        showError("Socket creation failed...\n");
        return -1;
    } else {
        if (LOG) printf("Socket successfully created..\n");
    }

    struct sockaddr_in server_address;
    memset(&server_address, '0', sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, ip_address, &server_address.sin_addr) <= 0) {
        showError("inet_pton error occured");
        return -1;
    }

    if (connect(socket_descriptor, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        showError("Connect Failed");
        return -1;
    }

    return socket_descriptor;
}

int *receive_from_server(int descriptor, size_t array_size, int buffer_size) {
    int *array = (int *) calloc(array_size, sizeof(int));

    int received_size = 0, rt = 0;

    for (int i = 0; i < array_size; i += buffer_size) {
        while (buffer_size - received_size) {
            rt = read(descriptor, array + i + received_size, buffer_size - received_size);
            if (rt == -1) {
                printf("an error occurs\n");
                return NULL;
            }
            if (!rt) return array;
            received_size += rt;
        }
        received_size = 0;
    }
    return array;
}

void print_matrix(const int *matrix, long matrix_size) {
    for (long i = 0; i < matrix_size; i++) {
        for (long j = 0; j < matrix_size; j++) {
            printf("%d ", matrix[i * matrix_size + j]);
        }
        printf("\n");
    }
    printf("\n");
}

void print_vector(const int *vector, long vector_length) {
    if (DEBUG) printf("read_vector:\n");
    for (long i = 0; i < vector_length; i++) {
        printf("%d ", vector[i]);
    }
    printf("\n\n");
}

void calc_answer(const int *matrix, const int *vector, int *answer, const long vector_length) {
#pragma omp parallel shared(matrix, vector)
    {
        if (LOG) printf("omp_get_num_threads: %d.\n\n", omp_get_num_threads());

        long i, j;
        int *answer_private = (int *) calloc(vector_length, sizeof(int));

        for (i = 0; i < vector_length; i++) {
#pragma omp for
            for (j = 0; j < vector_length; j++) {
                answer_private[i] += matrix[i * vector_length + j] * vector[j];
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