//
// Created by bergschaf on 1/5/24.
//

#include <stdio.h>
#include "decoder.c"
#include "elfLoader.c"


int main() {
    ElfFile *file = load_elf_file("test");
    printf("Hello, World!\n");
    print_elf_file(file);
    return 0;
}