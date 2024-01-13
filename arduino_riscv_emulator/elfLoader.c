//
// Created by bergschaf on 1/5/24.
//
#include "elfLoader.h"
#include "serial.h"
#include "memory.h"
#include <stdlib.h>
#include <stdio.h>

void load_programmHeader(uint8_t *data, ProgramHeader *programHeader) {
    // Size: 56 bytes

    // Check type
    programHeader->type = (int32_t) data[3] << 24 | (int32_t) data[2] << 16 | data[1] << 8 | data[0];

    // Check flags
    //programHeader->flags = data[7] << 24 | data[6] << 16 | data[5] << 8 | data[4];

    // Check offset (64 bit)
    programHeader->offset =
            (uint64_t) data[15] << 56 | (uint64_t) data[14] << 48 | (uint64_t) data[13] << 40 |
            (uint64_t) data[12] << 32 |
            (uint64_t) data[11] << 24 | (uint64_t) data[10] << 16 | data[9] << 8 | data[8];

    // Check virtual address (64 bit)
    programHeader->vaddr =
            (uint64_t) data[23] << 56 | (uint64_t) data[22] << 48 | (uint64_t) data[21] << 40 |
            (uint64_t) data[20] << 32 |
            (uint64_t) data[19] << 24 | (uint64_t) data[18] << 16 | data[17] << 8 | data[16];

    // Check physical address (64 bit)
    programHeader->paddr =
            (uint64_t) data[31] << 56 | (uint64_t) data[30] << 48 | (uint64_t) data[29] << 40 |
            (uint64_t) data[28] << 32 |
            (uint64_t) data[27] << 24 | (uint64_t) data[26] << 16 | data[25] << 8 | data[24];

    // Check file size (64 bit)
    programHeader->fileSize =
            (uint64_t) data[39] << 56 | (uint64_t) data[38] << 48 | (uint64_t) data[37] << 40 |
            (uint64_t) data[36] << 32 |
            (uint64_t) data[35] << 24 | (uint64_t) data[34] << 16 | data[33] << 8 | data[32];

    // Check memory size (64 bit)
    programHeader->memSize =
            (uint64_t) data[47] << 56 | (uint64_t) data[46] << 48 | (uint64_t) data[45] << 40 |
            (uint64_t) data[44] << 32 |
            (uint64_t) data[43] << 24 | (uint64_t) data[42] << 16 | data[41] << 8 | data[40];

    // Check align (64 bit)
    programHeader->align =
            (uint64_t) data[55] << 56 | (uint64_t) data[54] << 48 | (uint64_t) data[53] << 40 |
            (uint64_t) data[52] << 32 |
            (uint64_t) data[51] << 24 | (uint64_t) data[50] << 16 | data[49] << 8 | data[48];

}


void load_sectionHeader(uint8_t *data, SectionHeader *sectionHeader) {/*
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
*/
}


void receive_elf_file(ElfFile *elfFile) {
    // Wait for the handshake byte
    USART_WaitForByteInfinite(ELF_FILE_START_BYTE);
    // send elf file ack byte
    const uint8_t ack_byte = ELF_FILE_ACK_BYTE;
    USART_TransmitPolling(&ack_byte, 1);

    uint8_t header[64];
    USART_ReceiveBytes(header, 64);

    load_elf_header(elfFile, header);

    // load program headers
    elfFile->programHeaders = malloc(sizeof(ProgramHeader) * elfFile->programHeader_num);

    for (int i = 0; i < elfFile->programHeader_num; i++) {
        uint8_t programHeaderData[56];
        USART_ReceiveBytes(programHeaderData, 56);
        load_programmHeader(programHeaderData, &elfFile->programHeaders[i]);
    }

    uint64_t offset = 64 + 56 * elfFile->programHeader_num;
    // sort out the programm headers that are not loadable
    int num_loadable = 0;
    ProgramHeader *loadableProgramHeaders = malloc(sizeof(ProgramHeader) * elfFile->programHeader_num);
    for (int i = 0; i < elfFile->programHeader_num; i++) {
        if (elfFile->programHeaders[i].type == 1) {
            loadableProgramHeaders[num_loadable] = elfFile->programHeaders[i];
            num_loadable++;
        }
    }
    free(elfFile->programHeaders);

    printf("loaded (%d) loadable program headers\n", num_loadable);

    // sort the program headers by their offset in the elf file TODO probably the wrong way
    /*
    for (int i = 0; i < num_loadable; i++) {
        for (int j = i + 1; j < num_loadable; j++) {
            if (loadableProgramHeaders[i].offset > loadableProgramHeaders[j].offset) {
                ProgramHeader temp = loadableProgramHeaders[i];
                loadableProgramHeaders[i] = loadableProgramHeaders[j];
                loadableProgramHeaders[j] = temp;
            }
        }
    }*/
    uint8_t *data_chunk = malloc(CHUNK_SIZE);
    // load the data
    // print num_loadable
    serial_printf("num_loadable: %d\n", num_loadable);

    for (int i = 0; i < num_loadable; ++i) {
        uint16_t file_offset = loadableProgramHeaders[i].offset;
        uint64_t size = loadableProgramHeaders[i].fileSize;
        uint64_t vaddr = loadableProgramHeaders[i].vaddr;

        // print file offset, size, vaddr
        serial_printf("file_offset: %uld\n", file_offset);
        serial_printf("size: %uld\n", size);
        serial_printf("vaddr: %uld\n", vaddr);
        if (file_offset < offset) {
            // TODO error
            serial_printf("Hier, nicht gut, file_offset: %uld, offset: %uld\n", file_offset, offset);

            size -= offset - file_offset;
            file_offset = offset;
            vaddr += offset - file_offset;
        }
        if (file_offset > offset) {
            // discard the bytes in between
            // print the number of bytes to discard

            uint64_t discard_size = file_offset - offset;
            while (discard_size > 0) {
                int chunk_size = CHUNK_SIZE;
                if (discard_size < CHUNK_SIZE) {
                    chunk_size = discard_size;
                }
                USART_ReceiveBytes(data_chunk, chunk_size);
                discard_size -= chunk_size;
            }
        }

        while (size > 0) {
            //serial_printf("size: %uld\n", size);
            int chunk_size = CHUNK_SIZE;
            if (size < CHUNK_SIZE) {
                chunk_size = size;
            }
            USART_ReceiveBytes(data_chunk, chunk_size);
            memory_write(vaddr, data_chunk, chunk_size);
            size -= chunk_size;
            vaddr += chunk_size;
        }

        offset = file_offset + size;
    }
}


void load_elf_header(ElfFile *elfFile, uint8_t *data) {
    // Check magic number
    // TODO

    // Check architecture
    elfFile->arch = data[4];
    if (elfFile->arch != 2) {
        serial_printf("Only 64 bit architecture supported\n");
    }

    // Check endianness
    elfFile->endianness = data[5];
    if (elfFile->endianness != 1) {
        serial_printf("Only big endian supported\n");
    }

    // Check type at 16 to 18
    elfFile->type = data[17] << 8 | data[16];

    // Check instruction set (byteorder little endian) (18 to 19)
    elfFile->instructionSet = data[19] << 8 | data[18];
    if (elfFile->instructionSet != 0xF3) {
        serial_printf("Only RISC-V supported\n");
    }

    // Check entry point (64 bit) TODO maybe not correct
    elfFile->entry_pos = ((int64_t) data[31]) << 56 | (int64_t) data[30] << 48 | (int64_t) data[29] << 40 |
                         (int64_t) data[28] << 32 | (int64_t) data[27] << 24 | (int64_t) data[26] << 16 |
                         (int64_t) data[25] << 8 |
                         (int64_t) data[24];

    // Check program header offset
    elfFile->programHeader_offset =
            (int64_t) data[39] << 56 | (int64_t) data[38] << 48 | (int64_t) data[37] << 40 |
            (int64_t) data[36] << 32 | (int64_t) data[35] << 24 | (int64_t) data[34] << 16 | (int64_t) data[33] << 8 |
            (int64_t) data[32];

    // Check section header offset
    elfFile->sectionHeader_offset =
            (int64_t) data[47] << 56 | (int64_t) data[46] << 48 | (int64_t) data[45] << 40 |
            (int64_t) data[44] << 32 | (int64_t) data[43] << 24 | (int64_t) data[42] << 16 | (int64_t) data[41] << 8 |
            (int64_t) data[40];

    // Check header size (52 to 55)
    elfFile->header_size = data[53] << 8 | data[52];

    // Check program header entry size (16 bit)
    elfFile->programHeader_size = data[55] << 8 | data[54];

    // Check number of program header entries (16 bit)
    elfFile->programHeader_num = data[57] << 8 | data[56];

    // Check section header entry size (16 bit)
    elfFile->sectionHeader_size = data[59] << 8 | data[58];

    // Check number of section header entries (16 bit)
    elfFile->sectionHeader_num = data[61] << 8 | data[60];

}

