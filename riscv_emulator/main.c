//
// Created by bergschaf on 1/5/24.
//

#include <stdio.h>
#include "decoder.c"
#include "elfLoader.c"


int main() {
    ElfFile *file = malloc(sizeof(ElfFile));
    load_elf_file("test", file);
    printf("Hello, World!\n");
    print_elf_file(file);
    return 0;
/*

uint8_t test = 0b00111011;
int64_t test2 = (uint64_t) test << 56;
uint64_t test3 = (uint64_t) test << 56;
printf("%lx\n", test2);
printf("%lx\n", test3);
return 0;*/

}