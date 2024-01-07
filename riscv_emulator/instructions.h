//
// Created by bergschaf on 1/6/24.
//

#ifndef RISCV_EMULATOR_INSTRUCTIONS_H
#define RISCV_EMULATOR_INSTRUCTIONS_H
#include "cpu.h"

struct Instruction {
    uint8_t rd;
    uint8_t funct3;
    uint8_t rs1;
    uint8_t rs2;
    uint8_t funct7;
    uint64_t imm;
};
typedef struct Instruction Instruction;

Instruction decode_UType(int64_t bin_inst){
    struct Instruction *utype = malloc(sizeof(Instruction));
    utype->rd = (bin_inst >> 7) & 0b11111;
    utype->imm = bin_inst >> 12;
    return *utype;
}

void print_UType(Instruction utype){
    printf("UType:\n rd: %b\n imm: %lb\n", utype.rd, utype.imm);
}

typedef void (*execute_instruction)(Cpu *cpu, Instruction inst);

void execute_lui(Cpu *cpu, Instruction inst);

#endif //RISCV_EMULATOR_INSTRUCTIONS_H
