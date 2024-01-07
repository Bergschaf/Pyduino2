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

        case 0b0010011: {
            // IType
            Instruction itype = decode_IType(inst);
            switch (itype.funct3) {
                case 0b000: {
                    // addi
                    return (InstructionCallback) {&execute_addi, itype};
                }
                case 0b010: {
                    // slti
                    return (InstructionCallback) {&execute_slti, itype};
                }
                case 0b011: {
                    // sltiu
                    return (InstructionCallback) {&execute_sltiu, itype};
                }
                case 0b100: {
                    // xori
                    return (InstructionCallback) {&execute_xori, itype};
                }
                case 0b110: {
                    // ori
                    return (InstructionCallback) {&execute_ori, itype};
                }
                case 0b111: {
                    // andi
                    return (InstructionCallback) {&execute_andi, itype};
                }
                case 0b001: {
                    // slli
                    return (InstructionCallback) {&execute_slli, itype};
                }
                case 0b101: {
                    // srli
                    if (itype.imm & 0b1000000) {
                        // srai
                        return (InstructionCallback) {&execute_srai, itype};
                    } else {
                        // srli
                        return (InstructionCallback) {&execute_srli, itype};
                    }
                }
                default: {
                    printf("Unknown funct3: %b\n", itype.funct3);
                    exit(1409);
                }
            }
        }


        default: {
            printf("Unknown opcode: %b\n", opcode);
            exit(1409);
        }

    }
    exit(1409);
}