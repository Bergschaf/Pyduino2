//
// Created by bergschaf on 1/6/24.
//

#ifndef RISCV_EMULATOR_UTIL_H
#define RISCV_EMULATOR_UTIL_H

#include <stdint.h>
#include "config.h"

int64_t sign_extend(int64_t num, int from);

char* ljust(char *string,char *res, int size, char fillchar, char endchar);

int8_t check_flag(int64_t flags, int64_t flag);
#endif //RISCV_EMULATOR_UTIL_H

