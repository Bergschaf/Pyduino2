//
// Created by bergschaf on 1/5/24.
//
#include <stdio.h>
#include <stdint.h>
#include "config.h"
#include "util.h"

int64_t sign_extend(int64_t num, int from) {
    // Sign extends to XLEN bits
    return num << (XLEN - from) >> (XLEN - from);
}
