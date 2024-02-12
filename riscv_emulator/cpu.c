//
// Created by bergschaf on 1/6/24.
//

#include "cpu.h"
#include "decoder.h"
#include "kernel.h"
#include <execinfo.h>


void load_elf_executable(char *filename, Cpu *cpu) {
    initialize_kernel();
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
                cpu->curr_break = programHeader->vaddr + j;
            }
        }
    }
    free(file->data);
    free(file);
    // Set sp to the end of the memory
    cpu->last_mmap = MEM_SIZE - 1;
    cpu->regs[2] = MEM_SIZE - STACK_SIZE;
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
        printf("Instruction: 0x%lx\n", inst);
        print_Instruction(*callback.inst);
    }
    // print stack pointer
    // print registers
    int64_t prev_pc = cpu->pc;
    callback.func(cpu, *callback.inst);
    cpu->regs[0] = 0; // TODO may be very broken

    if (LOG_LEVEL == 0) {
        print_human_debug(cpu);
    } else if (LOG_LEVEL == 1) {
        printf("PC: 0x%lx\n", cpu->pc);
    }


    if (cpu->pc == prev_pc) {
        cpu->pc += 4;
    }

    free(callback.inst);
}

void print_human_debug(Cpu *cpu) {
    // highlight program counter bold white
    printf("\033[1;1m");
    printf("---------\nPC: 0x%lx\n", cpu->pc);
    printf("\033[0m");
    print_human_registers(cpu);
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
        if (LOG_LEVEL <= 1) {
            printf("Instruction count: %d\n", i);
        }
    }
}

void print_registers(Cpu *cpu) {
    for (int i = 0; i < 32; i++) {
        printf("%d: 0x%lx (%s)\n", i, cpu->regs[i], reg_names[i]);
    }
}


void print_human_registers(Cpu *cpu) {
    for (int i = 0; i < 32; i++) {
        if (cpu->regs[i] != 0) { printf("%d: 0x%lx (%s)\n", i, cpu->regs[i], reg_names[i]); }
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
    for (int i = 0; i < size; ++i) {
        string[i] = cpu->mem[address + i];
    }
    string[size - 1] = '\0';
}


int64_t memory_loadw(Cpu *cpu, int64_t address) {
    int64_t res = 0;
    for (int i = 0; i < 4; i++) {
        res |= cpu->mem[address + i] << (8 * i);
    }
    return res;
}


int64_t memory_loaddw(Cpu *cpu, int64_t address) {
    int64_t res = 0;
    for (int i = 0; i < 8; i++) {
        res |= cpu->mem[address + i] << (8 * i);
    }
    return res;
}


int64_t memory_mmap_anonymous(Cpu *cpu, int64_t size) {
    cpu->last_mmap -= size;
    // set the memory to 0
    for (int i = 0; i < size; i++) {
        cpu->mem[cpu->last_mmap + i] = 0;
    }
    printf("mmap: 0x%lx\n", cpu->last_mmap - size);
    return cpu->last_mmap - size;
}
