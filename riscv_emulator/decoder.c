//
// Created by bergschaf on 1/5/24.
//
#include "decoder.h"

InstructionCallback decode(int64_t inst){
    int opcode = inst & 0b1111111;

    switch (opcode) {
        case 0b0110111: {
            Instruction utype = decode_UType(inst);
            return (InstructionCallback) {&execute_lui, utype};
        }

    }
    exit(1409);
}