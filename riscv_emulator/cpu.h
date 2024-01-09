//
// Created by bergschaf on 1/6/24.
//

#ifndef RISCV_EMULATOR_CPU_H
#define RISCV_EMULATOR_CPU_H
#include "util.h"
#include <stdlib.h>
#include "config.h"
#include "elfLoader.h"

// List for register names



struct cpu {
    int64_t pc;
    int64_t regs[32];
    uint8_t mem[MEM_SIZE];
};
typedef struct cpu Cpu;

void load_elf_executable(char *filename, Cpu *cpu);

uint64_t get_next_inst(Cpu *cpu);

void run(Cpu *cpu);

void print_registers(Cpu *cpu);

void print_debug(Cpu *cpu);

#endif //RISCV_1EMULATOR_CPU_H


