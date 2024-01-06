//
// Created by bergschaf on 1/5/24.
//
#include "decoder.h"

 decode(uint64_t inst){
    int opcode = inst & 0b1111111;

    switch (opcode) {
        case 0b0110111: {
            UType utype = decode_UType(inst);
        }

    }
}