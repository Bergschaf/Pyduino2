//
// Created by bergschaf on 1/5/24.
//
#include "decoder.h"
#include "instructions.h"


InstructionCallback decode(uint64_t inst){
    int opcode = inst & 0b1111111;

    switch (opcode) {
        case 0b0110111: {
            Instruction utype = decode_UType(inst);
            return (InstructionCallback) {&execute_lui, utype};
        }
        case 0b0010111: {
            // auipc
            Instruction utype = decode_UType(inst);
            return (InstructionCallback) {&execute_auipc, utype};
        }

        default: {
            printf("Unknown opcode: %b\n", opcode);
            exit(1409);
        }

    }
    exit(1409);
}