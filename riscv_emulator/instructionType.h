//
// Created by bergschaf on 1/5/24.
//

#include <stdio.h>
#include <stdlib.h>
#include "util.h"

struct Instruction {
    uint8_t rd;
    uint8_t funct3;
    uint8_t rs1;
    uint8_t rs2;
    uint8_t funct7;
    uint64_t imm;
};
typedef struct Instruction Instruction;

Instruction decode_UType(int64_t inst){
    struct Instruction *utype = malloc(sizeof(Instruction));
    utype->rd = (inst >> 7) & 0b11111;
    utype->imm = inst >> 12;
    return *utype;
}

void print_UType(Instruction utype){
    printf("UType:\n rd: %b\n imm: %lb\n", utype.rd, utype.imm);
}



