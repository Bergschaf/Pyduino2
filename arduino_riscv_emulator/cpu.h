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
};
typedef struct cpu Cpu;

void memory_puts(Cpu *cpu, int64_t address, char *string, int64_t size);

void memory_loads(Cpu *cpu, int64_t address, char *string, int64_t size);

int64_t memory_loadw(Cpu *cpu, int64_t address);

int64_t memory_loaddw(Cpu *cpu, int64_t address);


void load_elf_executable(char *filename, Cpu *cpu);

uint32_t get_next_inst(Cpu *cpu);

void run(Cpu *cpu);

void run_next(Cpu *cpu);

void print_registers(Cpu *cpu);

void print_human_debug(Cpu *cpu);

void print_debug(Cpu *cpu);

#endif //RISCV_1EMULATOR_CPU_H


