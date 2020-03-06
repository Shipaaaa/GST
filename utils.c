//
// Created by Vladislav Shipugin on 06/03/2020.
//

#include "utils.h"

void showMessage(char *message) {
    printf("%s", message);
}

void showError(char *errorMessage) {
    change_printf_to_error();
    printf("%s\n", errorMessage);
    change_printf_to_log();
}

void change_printf_to_error() {
    printf("\033[0;31m");
}

void change_printf_to_log() {
    printf("\033[0m");
}
