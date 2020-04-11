//
// Created by Vladislav Shipugin on 06/03/2020.
//

#ifndef GST_UTILS_H
#define GST_UTILS_H

#include <stdio.h>
#include <stdlib.h>

void showMessage(char *message);

void showError(const char *errorMessage);

void change_printf_to_error();

void change_printf_to_log();

#endif //GST_UTILS_H
