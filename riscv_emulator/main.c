//
// Created by bergschaf on 1/5/24.
//

#include <stdio.h>
#include "decoder.c"
#include "util.h"
#include "cpu.h"


int main() {
    Cpu cpu;
    load_elf_executable("test.elf", &cpu);


}