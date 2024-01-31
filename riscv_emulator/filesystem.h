//
// Created by bergschaf on 1/31/24.
//

#ifndef RISCV_EMULATOR_FILESYSTEM_H
#define RISCV_EMULATOR_FILESYSTEM_H

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

struct file {
    int fd; // TODO not very good but works for now
    char *name; // with path
    char *content;
    int64_t size;
    struct file *next;
};
typedef struct file File;

// all files
File *file;

int open_file(char *pathname, int flags, int mode);

void initialize_filesystem();

void close(int fd);

void create_file(char *name, char *content, int64_t size);

#endif //RISCV_EMULATOR_FILESYSTEM_H
