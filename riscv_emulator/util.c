//
// Created by bergschaf on 1/5/24.
//
#include <stdint.h>
#include "config.h"
#include "util.h"


int64_t sign_extend(int64_t num, int from) {
    // Sign extends to XLEN bits
    return num << (XLEN - from) >> (XLEN - from);
}

void print_bin(int64_t num) {
    for (int i = 64 - 1; i >= 0; i--) {
        printf("%d", (num >> i) & 1);
    }
}
