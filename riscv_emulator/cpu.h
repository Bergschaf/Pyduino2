//
// Created by bergschaf on 1/6/24.
//

#ifndef RISCV_EMULATOR_CPU_H
#define RISCV_EMULATOR_CPU_H

#endif //RISCV_EMULATOR_CPU_H

#include "config.h"
#include "util.h"
#include "elfLoader.c"

struct cpu {
    uint64_t pc;
    int64_t regs[32];
    uint8_t mem[MEM_SIZE];
};
typedef struct cpu Cpu;

void load_elf_executable(char *filename, Cpu *cpu);