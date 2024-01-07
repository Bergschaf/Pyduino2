//
// Created by bergschaf on 1/5/24.
//
#include "decoder.h"
#include "instructions.h"


InstructionCallback decode(uint64_t inst) {
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

        case 0b1101111: {
            // jal
            Instruction jtype = decode_JType(inst);
            return (InstructionCallback) {&execute_jal, jtype};
        }

        case 0b1100111: {
            // jalr
            Instruction itype = decode_IType(inst);
            return (InstructionCallback) {&execute_jalr, itype};
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

        case 0b0000011: {
            // IType
            Instruction itype = decode_IType(inst);
            switch (itype.funct3) {
                case 0b000: {
                    // lb
                    return (InstructionCallback) {&execute_lb, itype};
                }
                case 0b001: {
                    // lh
                    return (InstructionCallback) {&execute_lh, itype};
                }
                case 0b010: {
                    // lw
                    return (InstructionCallback) {&execute_lw, itype};
                }
                case 0b011: {
                    // ld
                    return (InstructionCallback) {&execute_ld, itype};
                }

                case 0b100: {
                    // lbu
                    return (InstructionCallback) {&execute_lbu, itype};
                }
                case 0b101: {
                    // lhu
                    return (InstructionCallback) {&execute_lhu, itype};
                }

                case 0b110: {
                    // lwu
                    return (InstructionCallback) {&execute_lwu, itype};
                }
                default: {
                    printf("Unknown funct3: %b\n", itype.funct3);
                    exit(1409);
                }
            }
        }

        case 0b0100011: {
            // SType
            Instruction stype = decode_SType(inst);
            switch (stype.funct3) {
                case 0b000: {
                    // sb
                    return (InstructionCallback) {&execute_sb, stype};
                }
                case 0b001: {
                    // sh
                    return (InstructionCallback) {&execute_sh, stype};
                }
                case 0b010: {
                    // sw
                    return (InstructionCallback) {&execute_sw, stype};
                }
                case 0b011: {
                    // sd
                    return (InstructionCallback) {&execute_sd, stype};
                }
                default: {
                    printf("Unknown funct3: %b\n", stype.funct3);
                    exit(1409);
                }
            }
        }

        case 0b1100011: {
            // BType
            Instruction btype = decode_BType(inst);
            switch (btype.funct3) {
                case 0b000: {
                    // beq
                    return (InstructionCallback) {&execute_beq, btype};
                }
                case 0b001: {
                    // bne
                    return (InstructionCallback) {&execute_bne, btype};
                }
                case 0b100: {
                    // blt
                    return (InstructionCallback) {&execute_blt, btype};
                }
                case 0b101: {
                    // bge
                    return (InstructionCallback) {&execute_bge, btype};
                }
                case 0b110: {
                    // bltu
                    return (InstructionCallback) {&execute_bltu, btype};
                }
                case 0b111: {
                    // bgeu
                    return (InstructionCallback) {&execute_bgeu, btype};
                }
                default: {
                    printf("Unknown funct3: %b\n", btype.funct3);
                    exit(1409);
                }
            }

        }

        case 0b0110011: {
            Instruction rtype = decode_RType(inst);
            switch (rtype.funct3) {
                case 0b000: {
                    // add
                    if (rtype.funct7 == 0b0000000) {
                        return (InstructionCallback) {&execute_add, rtype};
                    } else {
                        // sub
                        return (InstructionCallback) {&execute_sub, rtype};
                    }
                }
                case 0b001: {
                    // sll
                    return (InstructionCallback) {&execute_sll, rtype};
                }
                case 0b010: {
                    // slt
                    return (InstructionCallback) {&execute_slt, rtype};
                }
                case 0b011: {
                    // sltu
                    return (InstructionCallback) {&execute_sltu, rtype};
                }
                case 0b100: {
                    // xor
                    return (InstructionCallback) {&execute_xor, rtype};
                }
                case 0b101: {
                    // srl
                    if (rtype.funct7 == 0b0000000) {
                        return (InstructionCallback) {&execute_srl, rtype};
                    } else {
                        // sra
                        return (InstructionCallback) {&execute_sra, rtype};
                    }
                }
                case 0b110: {
                    // or
                    return (InstructionCallback) {&execute_or, rtype};
                }
                case 0b111: {
                    // and
                    return (InstructionCallback) {&execute_and, rtype};
                }
                default: {
                    printf("Unknown funct3: %b\n", rtype.funct3);
                    exit(1409);
                }
            }
        }

        case 0b1110011: {
            // ecall
            Instruction itype = decode_IType(inst);
            return (InstructionCallback) {&execute_ecall, itype};
        }

        case 0b0011011: {
            // IType
            Instruction itype = decode_IType(inst);
            switch (itype.funct3) {
                case 0b000: {
                    // addiw
                    return (InstructionCallback) {&execute_addiw, itype};
                }
                case 0b001: {
                    // slliw
                    return (InstructionCallback) {&execute_slliw, itype};
                }
                case 0b101: {
                    // srliw
                    if (itype.imm & 0b1000000) {
                        // sraiw
                        return (InstructionCallback) {&execute_sraiw, itype};
                    } else {
                        // srliw
                        return (InstructionCallback) {&execute_srliw, itype};
                    }
                }
            }


        }

        case 0b0111011: {
            // RType
            Instruction rtype = decode_RType(inst);
            switch (rtype.funct3) {
                case 0b000: {
                    // addw
                    if (rtype.funct7 == 0b0000000) {
                        return (InstructionCallback) {&execute_addw, rtype};
                    } else {
                        // subw
                        return (InstructionCallback) {&execute_subw, rtype};
                    }
                }
                case 0b001: {
                    // sllw
                    return (InstructionCallback) {&execute_sllw, rtype};
                }
                case 0b101: {
                    // srlw
                    if (rtype.funct7 == 0b0000000) {
                        return (InstructionCallback) {&execute_srlw, rtype};
                    } else {
                        // sraw
                        return (InstructionCallback) {&execute_sraw, rtype};
                    }
                }
                default: {
                    printf("Unknown funct3: %b\n", rtype.funct3);
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