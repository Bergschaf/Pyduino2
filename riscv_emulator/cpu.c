//
// Created by bergschaf on 1/6/24.
//

#include "cpu.h"
#include "decoder.h"
#include <execinfo.h>


void load_elf_executable(char *filename, Cpu *cpu) {
    ElfFile *file = malloc(sizeof(ElfFile));
    load_elf_file(filename, file);
    cpu->pc = file->entry_pos;
    // initialize the memory with 0
    for (int i = 0; i < MEM_SIZE; i++) {
        cpu->mem[i] = 0;
    }
    // initialize the registers with 0
    for (int i = 0; i < 32; i++) {
        cpu->regs[i] = 0;
    }

    for (int i = 0; i < file->programHeader_num; i++) {
        ProgramHeader *programHeader = &file->programHeaders[i];
        if (programHeader->type == 1) {
            //printf("va: 0x%lx\n", programHeader->vaddr);
            //printf("size: 0x%lx\n", programHeader->fileSize);
            //printf("offset: 0x%lx\n", programHeader->offset);
            for (int j = 0; j < programHeader->fileSize; j++) {
                cpu->mem[programHeader->vaddr + j] = file->data[programHeader->offset + j];
            }
        }
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
        char **syms = backtrace_symbols(&funprt, 1);
        printf("%s\n", syms[0]);
        print_Instruction(callback.inst);
    }
    // print stack pointer
    // print registers
    int64_t prev_pc = cpu->pc;
    callback.func(cpu, callback.inst);
    cpu->regs[0] = 0; // TODO may be very broken
    if (cpu->pc == prev_pc) {
        cpu->pc += 4;
    }
    if (LOG_LEVEL == 0) {
        print_human_debug(cpu);
    }
    else if (LOG_LEVEL == 1) {
        printf("PC: 0x%lx\n", cpu->pc);
    }


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
        if (i > 100000) {
            break;
        }
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

void memory_loads(Cpu *cpu, int64_t address, char *string) {
    int i = 0;
    while (cpu->mem[address + i] != '\0') {
        string[i] =(char)cpu->mem[address + i];
        i++;
    }
    string[i] = '\0';
}


int64_t memory_loadw(Cpu *cpu, int64_t address) {
    int64_t res = 0;
    for (int i = 0; i < 4; i++) {
        res |= cpu->mem[address + i] << (8 * i);
    }
    return res;
}


