//
// Created by bergschaf on 1/5/24.
//
#include "elfLoader.h"


void load_programmHeader(uint8_t *data, ProgramHeader *programHeader) {
    // Size: 56 bytes

    // Check type
    programHeader->type = data[3] << 24 | data[2] << 16 | data[1] << 8 | data[0];

    // Check flags
    programHeader->flags = data[7] << 24 | data[6] << 16 | data[5] << 8 | data[4];

    // Check offset (64 bit)
    programHeader->offset =
            (uint64_t) data[15] << 56 | (uint64_t) data[14] << 48 | (uint64_t) data[13] << 40 | (uint64_t) data[12] << 32 |
            data[11] << 24 | data[10] << 16 | data[9] << 8 | data[8];

    // Check virtual address (64 bit)
    programHeader->vaddr =
            (uint64_t) data[23] << 56 | (uint64_t) data[22] << 48 | (uint64_t) data[21] << 40 | (uint64_t) data[20] << 32 |
            data[19] << 24 | data[18] << 16 | data[17] << 8 | data[16];

    // Check physical address (64 bit)
    programHeader->paddr =
            (uint64_t) data[31] << 56 | (uint64_t) data[30] << 48 | (uint64_t) data[29] << 40 | (uint64_t) data[28] << 32 |
            data[27] << 24 | data[26] << 16 | data[25] << 8 | data[24];

    // Check file size (64 bit)
    programHeader->fileSize =
            (uint64_t) data[39] << 56 | (uint64_t) data[38] << 48 | (uint64_t) data[37] << 40 | (uint64_t) data[36] << 32 |
            data[35] << 24 | data[34] << 16 | data[33] << 8 | data[32];

    // Check memory size (64 bit)
    programHeader->memSize =
            (uint64_t) data[47] << 56 | (uint64_t) data[46] << 48 | (uint64_t) data[45] << 40 | (uint64_t) data[44] << 32 |
            data[43] << 24 | data[42] << 16 | data[41] << 8 | data[40];

    // Check align (64 bit)
    programHeader->align =
            (uint64_t) data[55] << 56 | (uint64_t) data[54] << 48 | (uint64_t) data[53] << 40 | (uint64_t) data[52] << 32 |
            data[51] << 24 | data[50] << 16 | data[49] << 8 | data[48];

}


void load_sectionHeader(uint8_t *data, SectionHeader *sectionHeader) {
    // Size: 64 bytes
    exit(69); // TODO falsch
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


void load_elf_file(char *filename, ElfFile *elfFile) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error opening file\n");
        exit(4242);
    }
    elfFile->filename = filename;

    // Load data
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    printf("Size: %ld\n", size);
    elfFile->data = malloc(size);
    fread(elfFile->data, 1, size, file);

    // Read header
    uint8_t *header = elfFile->data;

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
    elfFile->header_size = header[53] << 8 | header[52];

    // Check program header entry size (16 bit)
    elfFile->programHeader_size = header[55] << 8 | header[54];

    // Check number of program header entries (16 bit)
    elfFile->programHeader_num = header[57] << 8 | header[56];

    // Check section header entry size (16 bit)
    elfFile->sectionHeader_size = header[59] << 8 | header[58];

    // Check number of section header entries (16 bit)
    elfFile->sectionHeader_num = header[61] << 8 | header[60];

    // Free the memory of the header
    free(header);

    // Read program header table
    elfFile->programHeaders = malloc(sizeof(ProgramHeader) * elfFile->programHeader_num);

    for (int i = 0; i < elfFile->programHeader_num; i++) {
        uint8_t *programHeaderData = elfFile->data + elfFile->programHeader_offset + i * elfFile->programHeader_size;
        ProgramHeader *programHeader = malloc(sizeof(ProgramHeader));
        load_programmHeader(programHeaderData, programHeader);
        elfFile->programHeaders[i] = *programHeader;
    }

    /*
    elfFile->sectionHeaders = malloc(sizeof(SectionHeader) * elfFile->sectionHeader_num);
    // Read section header table
    for (int i = 0; i < elfFile->sectionHeader_num; i++) {
        uint8_t *sectionHeaderData = malloc(64);
        fread(sectionHeaderData, 1, 64, file);
        SectionHeader *sectionHeader = malloc(sizeof(SectionHeader));
        load_sectionHeader(sectionHeaderData, sectionHeader);
        elfFile->sectionHeaders[i] = *sectionHeader;
    }*/
}

void print_programHeader(ProgramHeader *programHeader) {
    printf("ProgramHeader:\n type: %x\n flags: %x\n offset: %lx\n vaddr: %lx\n paddr: %lx\n fileSize: %lx\n memSize: %lx\n align: %lx\n",
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
    printf("SectionHeader:\n name: %x\n type: %x\n flags: %lx\n addr: %lx\n offset: %lx\n size: %lx\n link: %x\n info: %x\n align: %lx\n entrySize: %lx\n",
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
    printf("ElfFile:\n filename: %s\n arch: %d\n endianness: %d\n type: %d\n instructionSet: %d\n entry_pos: %lx\n programHeader_offset: %ld\n sectionHeader_offset: %ld\n header_size: %d\n programHeader_size: %d\n programHeader_num: %d\n sectionHeader_size: %d\n sectionHeader_num: %d\n",
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

    //for (int i = 0; i < elfFile->sectionHeader_num; ++i) {
    //    print_sectionHeader(&elfFile->sectionHeaders[i]);
    //}
}


// rewrite load_elf_file, load_programHeader, load_sectionHeader to use little endian
// This means that the bytes are read in reverse order (the higher bytes are shifted to the left)

