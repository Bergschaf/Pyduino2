//
// Created by bergschaf on 1/6/24.
//

#ifndef RISCV_EMULATOR_INSTRUCTIONS_H
#define RISCV_EMULATOR_INSTRUCTIONS_H

#include <stdint.h>
#include <stdio.h>
#include "config.h"
#include "cpu.h"

struct Instruction {
    uint8_t rd;
    uint8_t funct3;
    uint8_t rs1;
    uint8_t rs2;
    uint8_t funct7;
    uint64_t imm;
    uint8_t type; // 0 = RType, 1 = IType, 2 = SType, 3 = BType, 4 = UType, 5 = JType
};
typedef struct Instruction Instruction;

Instruction decode_UType(int64_t bin_inst);

void print_Instruction(Instruction inst);

typedef void (*execute_instruction)(Cpu *cpu, Instruction inst);

void execute_lui(Cpu *cpu, Instruction inst);

#endif //RISCV_EMULATOR_INSTRUCTIONS_H
