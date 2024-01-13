//
// Created by bergschaf on 1/8/24.
//

#ifndef RISCV_EMULATOR_KERNEL_H
#define RISCV_EMULATOR_KERNEL_H

#include "util.h"
#include "cpu.h"

void do_syscall(Cpu *cpu);

#endif //RISCV_EMULATOR_KERNEL_H
