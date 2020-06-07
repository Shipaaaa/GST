/*
 * Последовательная реализация.
 * Вариант: 7
 * Бизнес логика: Умножение матрицы на вектор
 *
 * Данные генерируются утилитой, принимающей в качестве параметров
 * размер данных для обработки в мегабайтах
 * и имя файла (TCP хост-порт) куда будут выгружены данные.
 *
 * Запуск: gcc generator_with_tcp.c utils.c -o generator_with_tcp.out && \
$PWD/generator_with_tcp.out 1
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>
#include "utils.h"

#define PORT 8080
#define DEBUG 0
#define LOG 1

void generate_matrix(int *matrix, long matrix_size);

void generate_vector(int *vector, long matrix_size);

void send_data(long matrix_size, const int *matrix, long vector_length, const int *vector);

int write_to_client(int descriptor, const int *array, size_t array_size, int buffer_size);

/**
 * gcc tcp_server.c -o tcp_server.out && ./tcp_server.out
 */
int main(int argc, char *argv[]) {

    if (argc < 2) {
        showError("Вы не ввели размер данных");
        return -1;
    }

    double size_in_mb = atof(argv[1]);
    if (LOG) printf("size: %5.2fmb.\n", size_in_mb);

    double size_in_bytes = size_in_mb * 1024 * 1024;
    if (LOG) printf("size_in_bytes: %5.2f\n", size_in_bytes);

    long count_of_elements = (long) (size_in_bytes / sizeof(int));
    if (LOG) printf("count_of_elements: %ld, size_of_int: %ld\n", count_of_elements, sizeof(int));

    long matrix_size = (long) ((-1 + sqrt(1 - 4 * (-count_of_elements))) / 2);
    if (LOG) printf("matrix_size: %ld\n\n", matrix_size);

    int *matrix = (int *) calloc(matrix_size * matrix_size, sizeof(int));
    generate_matrix(matrix, matrix_size);

    long vector_length = matrix_size;
    int *vector = (int *) calloc(vector_length, sizeof(int));
    generate_vector(vector, vector_length);

    send_data(matrix_size, matrix, vector_length, vector);

    free(matrix);
    free(vector);

    if (LOG) printf("Exit...\n");
    return 0;
}

void send_data(long matrix_size, const int *matrix, long vector_length, const int *vector) {

    int listen_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_descriptor == -1) {
        showError("Socket creation failed...\n");
        return;
    } else {
        if (LOG) printf("Socket successfully created..\n");
    }

    struct sockaddr_in server_address;
    memset(&server_address, '0', sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(PORT);


    if ((bind(listen_descriptor, (struct sockaddr *) &server_address, sizeof(server_address))) != 0) {
        showError("Socket bind failed...\n");
        return;
    } else {
        if (LOG) printf("Socket successfully binded..\n");
    }

    if ((listen(listen_descriptor, 10)) != 0) {
        showError("Listen failed...\n");
        return;
    } else {
        if (LOG) printf("Server listening..\n");
    }

    for (;;) {
        int client_connect_descriptor = accept(listen_descriptor, (struct sockaddr *) NULL, NULL);

        if (client_connect_descriptor < 0) {
            showError("Server accept failed...\n");
            return;
        } else {
            if (LOG) printf("Server accept the client...\n");
        }

        write(client_connect_descriptor, &matrix_size, sizeof(long));
        write_to_client(client_connect_descriptor, matrix, matrix_size * matrix_size, 255);
        write_to_client(client_connect_descriptor, vector, vector_length, 255);

        close(client_connect_descriptor);
        sleep(1);
    }
}

int write_to_client(int descriptor, const int *array, size_t array_size, int buffer_size) {
    int i = 0, sent_size = 0;
    for (i = 0; i < array_size; i += buffer_size) {
        while (buffer_size - sent_size) {
            sent_size += write(descriptor, array + i + sent_size, buffer_size - sent_size);
        }
        sent_size = 0;
    }
    return i;
}

void generate_matrix(int *matrix, const long matrix_size) {
    for (long i = 0; i < matrix_size; i++) {
        for (long j = 0; j < matrix_size; j++) {
            int num = rand(); // NOLINT(cert-msc30-c, cert-msc50-cpp)
            matrix[i * matrix_size + j] = num;
            if (DEBUG) printf("%d ", num);
        }
        if (DEBUG) printf("\n");
    }
    if (DEBUG) printf("\n");
}

void generate_vector(int *vector, const long matrix_size) {
    for (long i = 0; i < matrix_size; i++) {
        int num = rand(); // NOLINT(cert-msc30-c, cert-msc50-cpp)
        vector[i] = num;
        if (DEBUG) printf("%d ", num);
    }
    if (DEBUG) printf("\n");
}
