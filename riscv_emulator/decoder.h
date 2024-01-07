//
// Created by bergschaf on 1/6/24.
//

#ifndef RISCV_EMULATOR_DECODERH_H
#define RISCV_EMULATOR_DECODERH_H
#include <stdint.h>
#include "config.h"
#include "cpu.h"
#include "instructions.h"


struct instruction_callback {
    execute_instruction func;
    Instruction inst;
};
typedef struct instruction_callback InstructionCallback;

InstructionCallback decode(uint64_t inst);

#endif //RISCV_EMULATOR_DECODERH_H
