//
// Created by bergschaf on 1/6/24.
//

#ifndef RISCV_EMULATOR_DECODERH_H
#define RISCV_EMULATOR_DECODERH_H
#include <stdint.h>
#include "config.h"
#include "instructionType.h"
#include "cpu.h"

typedef void (*instruction_callback)(Cpu *cpu, Instruction inst);

(*)(*Cpu) decode(uint64_t inst);
#endif //RISCV_EMULATOR_DECODERH_H
