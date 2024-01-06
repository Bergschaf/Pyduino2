//
// Created by bergschaf on 1/5/24.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "util.c"

struct ProgramHeader {
    int type; // 1 for loadable segment
    int flags; // 1 for executable, 2 for writable, 4 for readable
    uint64_t offset; // offset of segment in file image
    int64_t vaddr; // virtual address of segment in memory
    int64_t paddr; // physical address of segment in memory
    int64_t fileSize; // size of segment in file image
    int64_t memSize; // size of segment in memory
    int64_t align; // alignment of segment
};
typedef struct ProgramHeader ProgramHeader;

void load_programmHeader(char *data, ProgramHeader *programHeader) {
    // Size: 56 bytes

    // Check type
    programHeader->type = data[3] << 24 | data[2] << 16 | data[1] << 8 | data[0];

    // Check flags
    programHeader->flags = data[7] << 24 | data[6] << 16 | data[5] << 8 | data[4];

    // Check offset (64 bit)
    programHeader->offset =
            (int64_t) data[15] << 56 | (int64_t) data[14] << 48 | (int64_t) data[13] << 40 | (int64_t) data[12] << 32 |
            data[11] << 24 | data[10] << 16 | data[9] << 8 | data[8];

    // Check virtual address (64 bit)
    programHeader->vaddr =
            (int64_t) data[23] << 56 | (int64_t) data[22] << 48 | (int64_t) data[21] << 40 | (int64_t) data[20] << 32 |
            data[19] << 24 | data[18] << 16 | data[17] << 8 | data[16];

    // Check physical address (64 bit)
    programHeader->paddr =
            (int64_t) data[31] << 56 | (int64_t) data[30] << 48 | (int64_t) data[29] << 40 | (int64_t) data[28] << 32 |
            data[27] << 24 | data[26] << 16 | data[25] << 8 | data[24];

    // Check file size (64 bit)
    programHeader->fileSize =
            (int64_t) data[39] << 56 | (int64_t) data[38] << 48 | (int64_t) data[37] << 40 | (int64_t) data[36] << 32 |
            data[35] << 24 | data[34] << 16 | data[33] << 8 | data[32];

    // Check memory size (64 bit)
    programHeader->memSize =
            (int64_t) data[47] << 56 | (int64_t) data[46] << 48 | (int64_t) data[45] << 40 | (int64_t) data[44] << 32 |
            data[43] << 24 | data[42] << 16 | data[41] << 8 | data[40];

    // Check align (64 bit)
    programHeader->align =
            (int64_t) data[55] << 56 | (int64_t) data[54] << 48 | (int64_t) data[53] << 40 | (int64_t) data[52] << 32 |
            data[51] << 24 | data[50] << 16 | data[49] << 8 | data[48];

}


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

void load_sectionHeader(char *data, SectionHeader *sectionHeader) {
    // Size: 64 bytes

    // Check name
    sectionHeader->name = data[3] << 24 | data[2] << 16 | data[1] << 8 | data[0];

    // Check type
    sectionHeader->type = data[7] << 24 | data[6] << 16 | data[5] << 8 | data[4];

    // Check flags (64 bit)
    sectionHeader->flags =
            (int64_t) data[15] << 56 | (int64_t) data[14] << 48 | (int64_t) data[13] << 40 | (int64_t) data[12] << 32 |
            data[11] << 24 | data[10] << 16 | data[9] << 8 | data[8];

    // Check virtual address (64 bit)
    sectionHeader->addr =
            (int64_t) data[23] << 56 | (int64_t) data[22] << 48 | (int64_t) data[21] << 40 | (int64_t) data[20] << 32 |
            data[19] << 24 | data[18] << 16 | data[17] << 8 | data[16];

    // Check offset (64 bit)
    sectionHeader->offset =
            (int64_t) data[31] << 56 | (int64_t) data[30] << 48 | (int64_t) data[29] << 40 | (int64_t) data[28] << 32 |
            data[27] << 24 | data[26] << 16 | data[25] << 8 | data[24];

    // Check size (64 bit)
    sectionHeader->size =
            (int64_t) data[39] << 56 | (int64_t) data[38] << 48 | (int64_t) data[37] << 40 | (int64_t) data[36] << 32 |
            data[35] << 24 | data[34] << 16 | data[33] << 8 | data[32];

    // Check link (32 bit)
    sectionHeader->link = data[43] << 24 | data[42] << 16 | data[41] << 8 | data[40];

    // Check info (32 bit)
    sectionHeader->info = data[47] << 24 | data[46] << 16 | data[45] << 8 | data[44];

    // Check align (64 bit)
    sectionHeader->align =
            (int64_t) data[55] << 56 | (int64_t) data[54] << 48 | (int64_t) data[53] << 40 | (int64_t) data[52] << 32 |
            data[51] << 24 | data[50] << 16 | data[49] << 8 | data[48];

    // Check entry size (64 bit)
    sectionHeader->entrySize =
            (int64_t) data[63] << 56 | (int64_t) data[62] << 48 | (int64_t) data[61] << 40 | (int64_t) data[60] << 32 |
            data[59] << 24 | data[58] << 16 | data[57] << 8 | data[56];

}

// rewrite load_sectionHeader and load_programHeader to use little endian
// This means that the bytes are read in reverse order (the higher bytes are shifted to the left)



struct ElfFile {
    char *filename;
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
    SectionHeader *sectionHeaders;
};

typedef struct ElfFile ElfFile;

ElfFile *load_elf_file(char *filename) {
    ElfFile *elfFile = malloc(sizeof(ElfFile));
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error opening file\n");
        exit(4242);
    }
    elfFile->filename = filename;

    // Read header
    uint8_t *header = malloc(64);
    fread(header, 1, 64, file);

    // Check magic number
    // TODO

    // Check architecture
    elfFile->arch = header[4];
    if (elfFile->arch != 2) {
        printf("Only 64 bit Arch supported\n");
        exit(4242);
    }

    // Check endianness
    elfFile->endianness = header[5];
    if (elfFile->endianness != 1) {
        printf("Only little endianness supported\n");
        exit(4242);
    }

    // Check type at 16 to 18
    elfFile->type = header[17] << 8 | header[16];

    // Check instruction set (byteorder little endian) (18 to 19)
    elfFile->instructionSet = header[19] << 8 | header[18];
    printf("%x\n", header[19]);
    if (elfFile->instructionSet != 0xF3) {
        printf("Only RISC-V instruction set supported\n");
        exit(4242);
    }

    // Check entry point (64 bit) TODO maybe not correct
    elfFile->entry_pos = ((int64_t) header[31]) << 56 | (int64_t) header[30] << 48 | (int64_t) header[29] << 40 |
                         (int64_t) header[28] << 32 | header[27] << 24 | header[26] << 16 | header[25] << 8 |
                         header[24];

    // Check program header offset
    elfFile->programHeader_offset =
            (int64_t) header[39] << 56 | (int64_t) header[38] << 48 | (int64_t) header[37] << 40 |
            (int64_t) header[36] << 32 | header[35] << 24 | header[34] << 16 | header[33] << 8 | header[32];

    // Check section header offset
    elfFile->sectionHeader_offset =
            (int64_t) header[47] << 56 | (int64_t) header[46] << 48 | (int64_t) header[45] << 40 |
            (int64_t) header[44] << 32 | header[43] << 24 | header[42] << 16 | header[41] << 8 | header[40];

    // Check header size (52 to 55)
    elfFile->header_size = header[55] << 8 | header[54];

    // Check program header entry size (16 bit)
    elfFile->programHeader_size = header[57] << 8 | header[56];

    // Check program header number (16 bit)
    elfFile->programHeader_num = header[59] << 8 | header[58];

    // Check section header entry size (16 bit)
    elfFile->sectionHeader_size = header[61] << 8 | header[60];

    // Check section header number (16 bit)
    elfFile->sectionHeader_num = header[63] << 8 | header[62];

    // Read program header table
    for (int i = 0; i < elfFile->programHeader_num; i++) {
        char *programHeaderData = malloc(56);
        fread(programHeaderData, 1, 56, file);
        ProgramHeader *programHeader = malloc(sizeof(ProgramHeader));
        load_programmHeader(programHeaderData, programHeader);
        elfFile->programHeaders[i] = *programHeader;
    }

    // Read section header table
    for (int i = 0; i < elfFile->sectionHeader_num; i++) {
        char *sectionHeaderData = malloc(64);
        fread(sectionHeaderData, 1, 64, file);
        SectionHeader *sectionHeader = malloc(sizeof(SectionHeader));
        load_sectionHeader(sectionHeaderData, sectionHeader);
        elfFile->sectionHeaders[i] = *sectionHeader;
    }
    return elfFile;
}

void print_programHeader(ProgramHeader *programHeader) {
    printf("ProgramHeader:\n type: %d\n flags: %d\n offset: %ld\n vaddr: %ld\n paddr: %ld\n fileSize: %ld\n memSize: %ld\n align: %ld\n",
           programHeader->type,
           programHeader->flags,
           programHeader->offset,
           programHeader->vaddr,
           programHeader->paddr,
           programHeader->fileSize,
           programHeader->memSize,
           programHeader->align);

}

void print_sectionHeader(SectionHeader *sectionHeader) {
    printf("SectionHeader:\n name: %d\n type: %d\n flags: %d\n addr: %d\n offset: %d\n size: %d\n link: %d\n info: %d\n align: %d\n entrySize: %d\n",
           sectionHeader->name,
           sectionHeader->type,
           sectionHeader->flags,
           sectionHeader->addr,
           sectionHeader->offset,
           sectionHeader->size,
           sectionHeader->link,
           sectionHeader->info,
           sectionHeader->align,
           sectionHeader->entrySize);
}

void print_elf_file(ElfFile *elfFile) {
    printf("ElfFile:\n filename: %s\n arch: %d\n endianness: %d\n type: %d\n instructionSet: %d\n entry_pos: %ld\n programHeader_offset: %ld\n sectionHeader_offset: %ld\n header_size: %d\n programHeader_size: %d\n programHeader_num: %d\n sectionHeader_size: %d\n sectionHeader_num: %d\n",
           elfFile->filename,
           elfFile->arch,
           elfFile->endianness,
           elfFile->type,
           elfFile->instructionSet,
           elfFile->entry_pos,
           elfFile->programHeader_offset,
           elfFile->sectionHeader_offset,
           elfFile->header_size,
           elfFile->programHeader_size,
           elfFile->programHeader_num,
           elfFile->sectionHeader_size,
           elfFile->sectionHeader_num);
    for (int i = 0; i < elfFile->programHeader_num; ++i) {
        print_programHeader(&elfFile->programHeaders[i]);
    }

    for (int i = 0; i < elfFile->sectionHeader_num; ++i) {
        print_sectionHeader(&elfFile->sectionHeaders[i]);
    }
}


// rewrite load_elf_file, load_programHeader, load_sectionHeader to use little endian
// This means that the bytes are read in reverse order (the higher bytes are shifted to the left)

