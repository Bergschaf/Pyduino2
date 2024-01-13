//
// Created by bergschaf on 1/6/24.
//

#include "cpu.h"
#include "decoder.h"
#include "elfLoader.h"
#include "serial.h"
#include "memory.h"

void load_elf_executable(char *filename, Cpu *cpu) {
    ElfFile *file = malloc(sizeof(ElfFile));
    receive_elf_file(file);
    cpu->pc = file->entry_pos;
    // initialize the memory with 0 TODO

    // initialize the registers with 0
    for (int i = 0; i < 32; i++) {
        cpu->regs[i] = 0;
    }


    free(file);
    // Set sp to the end of the memory
    cpu->regs[2] = MEM_SIZE - 1000;
}

uint64_t get_next_inst(Cpu *cpu) {
    uint64_t inst = 0;
    for (int i = 0; i < 4; i++) {
        inst |= cpu->mem[cpu->pc + i] << (8 * i);
    }
    return inst;
}


void run_next(Cpu *cpu) {
    uint64_t inst = get_next_inst(cpu);
    InstructionCallback callback = decode(inst);
    // print the name of the function
    void *funprt = callback.func;
    if (LOG_LEVEL == 0) {
        //char **syms = backtrace_symbols(&funprt, 1);
        //printf("%s\n", syms[0]);
        //print_Instruction(*callback.inst);
    }
    // print stack pointer
    // print registers
    int64_t prev_pc = cpu->pc;
    callback.func(cpu, *callback.inst);
    cpu->regs[0] = 0; // TODO may be very broken
    if (cpu->pc == prev_pc) {
        cpu->pc += 4;
    }
    if (LOG_LEVEL == 0) {
        print_human_debug(cpu);
    }
    else if (LOG_LEVEL == 1) {
        serial_printf("PC: 0x%lx\n", cpu->pc);
    }

    free(callback.inst);
}

void print_human_debug(Cpu *cpu) {
    printf("PC: 0x%lx\n", cpu->pc);
    print_registers(cpu);
    printf("end\n\n");
}

void print_debug(Cpu *cpu) {
    printf("PC: 0x%lx\n", cpu->pc);
    for (int i = 0; i < 32; i++) {
        printf("%d: 0x%lx\n", i, cpu->regs[i]);
    }
    printf("end\n");
}

void run(Cpu *cpu) {
    // stop and ask for input after 1000 instructions
    int i = 0;
    while (cpu->pc < MEM_SIZE) {
        run_next(cpu);
        i++;
        //if (i > 100000) {
        //    break;
        //}
    }
}

void print_registers(Cpu *cpu) {
    for (int i = 0; i < 32; i++) {
        printf("%d: 0x%lx (%s)\n", i, cpu->regs[i], reg_names[i]);
    }
}

void memory_puts(Cpu *cpu, int64_t address, char *string) {
    int i = 0;
    while (string[i] != '\0') {
        cpu->mem[address + i] = string[i];
        i++;
    }
    cpu->mem[address + i] = '\0';
}

void memory_loads(Cpu *cpu, int64_t address, char *string, int64_t size) {
    memory_load(address, string, size);
}


int64_t memory_loadw(Cpu *cpu, int64_t address) {
    uint8_t bytes[4];
    memory_load(address, bytes, 4);
    int64_t res = 0;
    for (int i = 0; i < 4; i++) {
        res |= bytes[i] << (8 * i);
    }
    return res;
}


int64_t memory_loaddw(Cpu *cpu, int64_t address) {
    uint8_t bytes[8];
    memory_load(address, bytes, 8);
    int64_t res = 0;
    for (int i = 0; i < 8; i++) {
        res |= bytes[i] << (8 * i);
    }
    return res;
}

