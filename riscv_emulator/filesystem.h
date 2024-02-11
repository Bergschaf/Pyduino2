//
// Created by bergschaf on 1/31/24.
//

#ifndef RISCV_EMULATOR_FILESYSTEM_H
#define RISCV_EMULATOR_FILESYSTEM_H

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

struct file_struct {
    int fd; // TODO not very good but works for now
    char *name; // with path
    char *content;
    int64_t size;
    struct file_struct *next;
};
typedef struct file_struct File;

// all files
extern File *entry_file;

int open_file(char*, int, int);

void initialize_filesystem();

void close_file(int);

void read_file(int fd, char *buf, int64_t count);

void create_file(char*, char*, int64_t);

#endif //RISCV_EMULATOR_FILESYSTEM_H
