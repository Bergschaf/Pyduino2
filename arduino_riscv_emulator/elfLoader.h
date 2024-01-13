//
// Created by bergschaf on 1/6/24.
//

#ifndef RISCV_EMULATOR_ELFLOADER_H
#define RISCV_EMULATOR_ELFLOADER_H


//
// Created by bergschaf on 1/5/24.
//
//#include <stdio.h>
#include <stdint.h>
//#include <stdlib.h>

struct ProgramHeader {
    int32_t type; // 1 for loadable segment
    int32_t flags; // 1 for executable, 2 for writable, 4 for readable
    int64_t offset; // offset of segment in file image
    int64_t vaddr; // virtual address of segment in memory
    int64_t paddr; // physical address of segment in memory
    int64_t fileSize; // size of segment in file image
    int64_t memSize; // size of segment in memory
    int64_t align; // alignment of segment
};
typedef struct ProgramHeader ProgramHeader;


struct SectionHeader {
    int32_t name; // offset of name in string table
    int32_t type; // 0 for null, 1 for program data, 2 for symbol table, 3 for string table, 4 for relocation entries with explicit addends, 5 for symbol hash table, 6 for dynamic linking information, 7 for notes, 8 for program space with no data (bss), 9 for relocation entries, 10 for reserved, 11 for dynamic linker symbol table, 12 for array of constructors, 13 for array of destructors, 14 for array of pre-constructors, 15 for section group, 16 for extended section indeces, 17 for number of defined types
    int64_t flags; // 1 for writable, 2 for occupied, 4 for executable, 0xF0000000 for mask for processor-specific flags
    int64_t addr; // virtual address of section in memory
    int64_t offset; // offset of section in file image
    int64_t size; // size of section in file image
    int32_t link; // section header table index link
    int32_t info; // extra information
    int64_t align; // alignment of section
    int64_t entrySize; // size of entries if section holds table
};
typedef struct SectionHeader SectionHeader;


struct ElfFile {
    char *filename;
    uint8_t *data;
    int arch; // 32 or 64
    int endianness; // 0 for little, 1 for big
    int type; // 1 for relocatable, 2 for executable, 3 for shared, 4 for core
    int instructionSet; // 0xF3 for RISC-V
    int64_t entry_pos; // entry point
    int64_t programHeader_offset; // offset of program header table
    int64_t sectionHeader_offset; // offset of section header table
    int header_size; // size of this header
    int programHeader_size; // size of a program header table entry
    int programHeader_num; // number of entries in program header table
    int sectionHeader_size; // size of a section header table entry
    int sectionHeader_num; // number of entries in section header table

    ProgramHeader *programHeaders;
};

typedef struct ElfFile ElfFile;


void load_programmHeader(uint8_t *data, ProgramHeader *programHeader);

void load_sectionHeader(uint8_t *data, SectionHeader *sectionHeader);

void load_elf_header(ElfFile *elfFile, uint8_t* data);

void receive_elf_file(ElfFile *elfFile);


#endif //RISCV_EMULATOR_ELFLOADER_H
