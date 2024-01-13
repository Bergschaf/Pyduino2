//
// Created by bergschaf on 1/5/24.
//

#include "config.h"
#include "util.h"

int64_t sign_extend(int64_t num, int from) {
    // Sign extends to XLEN bits
    return num << (XLEN - from) >> (XLEN - from);
}

char* ljust(char *string, char *res, int size, char fillchar, char endchar) {
    int i;
    uint8_t end = 0;
    for (i = 0; i < size; i++) {
        if (end) {
            res[i] = fillchar;
        } else {
            res[i] = string[i];
            if (string[i] == '\0') {
                end = 1;
            }
        }
    }
    res[size - 1] = endchar;
    return res;
}
