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
        backtrace_symbols_fd(&funprt, 1, 1);
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
    if (LOG_LEVEL <= 1) {
        printf("PC: %lx\n", cpu->pc - 4);
    }
    if (LOG_LEVEL == 0) {
        print_registers(cpu);
        printf("\n\n");
    }


}


void print_debug(Cpu *cpu) {
    printf("PC: 0x%lx\n", cpu->pc);
    print_registers(cpu);
    printf("\n\n");
}

void run(Cpu *cpu) {
    while (cpu->pc < MEM_SIZE) {
        run_next(cpu);
    }
}

void print_registers(Cpu *cpu) {
    for (int i = 0; i < 32; i++) {
        printf("%d: 0x%lx\n", i, cpu->regs[i]);
    }
}


