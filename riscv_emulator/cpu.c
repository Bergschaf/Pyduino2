//
// Created by bergschaf on 1/6/24.
//
#include <stdio.h>
#include <stdint.h>
#include "cpu.h"


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
