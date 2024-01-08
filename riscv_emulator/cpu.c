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
    for (int i = 0; i < file->programHeader_num; i++) {
        ProgramHeader *programHeader = &file->programHeaders[i];
        if (programHeader->type == 1) {
            for (int j = 0; j < programHeader->fileSize; j++) {
                cpu->mem[programHeader->vaddr + j] = file->data[programHeader->offset + j];
            }
        }
    }
    free(file);
}

uint64_t get_next_inst(Cpu *cpu) {
    uint64_t inst = 0;
    for (int i = 0; i < 4; i++) {
        inst |= cpu->mem[cpu->pc + i] << (8 * i);
    }
    cpu->pc += 4;
    return inst;
}

void run_next(Cpu *cpu){
    uint64_t inst = get_next_inst(cpu);
    InstructionCallback callback = decode(inst);
    // print the name of the function
    void *funprt = callback.func;
    if (LOG_LEVEL == 0) {
        printf("PC: %lx\n", cpu->pc - 4);
        backtrace_symbols_fd(&funprt, 1, 1);
        print_Instruction(callback.inst);
        printf("\n\n");
    }
    printf("PC: %lx\n", cpu->pc - 4);
    // print stack pointer
    callback.func(cpu, callback.inst);
}

void run(Cpu *cpu){
    while (cpu->pc < MEM_SIZE){
        run_next(cpu);
    }
}
