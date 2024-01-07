//
// Created by bergschaf on 1/6/24.
//

#include "instructions.h"

Instruction decode_UType(int64_t bin_inst){
    struct Instruction *utype = malloc(sizeof(Instruction));
    utype->rd = (bin_inst >> 7) & 0b11111;
    utype->imm = bin_inst >> 12;
    return *utype;
}

void print_UType(Instruction utype){
    printf("UType:\n rd: %b\n imm: %lb\n", utype.rd, utype.imm);
}

void print_RType(Instruction rtype){
    printf("RType:\n rd: %b\n funct3: %b\n rs1: %b\n rs2: %b\n funct7: %b\n", rtype.rd, rtype.funct3, rtype.rs1, rtype.rs2, rtype.funct7);
}

void print_IType(Instruction itype){
    printf("IType:\n rd: %b\n imm: %lb\n", itype.rd, itype.imm);
}

void print_SType(Instruction stype){
    printf("SType:\n funct3: %b\n rs1: %b\n rs2: %b\n imm: %lb\n", stype.funct3, stype.rs1, stype.rs2, stype.imm);
}

void print_BType(Instruction btype){
    printf("BType:\n funct3: %b\n rs1: %b\n rs2: %b\n imm: %lb\n", btype.funct3, btype.rs1, btype.rs2, btype.imm);
}

void print_JType(Instruction jtype){
    printf("JType:\n rd: %b\n imm: %lb\n", jtype.rd, jtype.imm);
}


void print_Instruction(Instruction inst){
    switch (inst.type) {
        case 0: {
            print_RType(inst);
            break;
        }
        case 1: {
            print_IType(inst);
            break;
        }
        case 2: {
            print_SType(inst);
            break;
        }
        case 3: {
            print_BType(inst);
            break;
        }
        case 4: {
            print_UType(inst);
            break;
        }
        case 5: {
            print_JType(inst);
            break;
        }
        default: {
            printf("Unknown instruction type: %d\n", inst.type);
            exit(1409);
        }
    }
}

void execute_lui(Cpu *cpu, Instruction inst){
    cpu->regs[inst.rd] = inst.imm;
}

void execute_auipc(Cpu *cpu, Instruction inst){
    //
}

void execute_jal(Cpu *cpu, Instruction inst){
    //
}

void execute_jalr(Cpu *cpu, Instruction inst){
    //
}

void execute_beq(Cpu *cpu, Instruction inst){
    //
}

void execute_bne(Cpu *cpu, Instruction inst){
    //
}

void execute_blt(Cpu *cpu, Instruction inst){
    //
}

void execute_bge(Cpu *cpu, Instruction inst){
    //
}

void execute_bltu(Cpu *cpu, Instruction inst){
    //
}

void execute_bgeu(Cpu *cpu, Instruction inst){
    //
}

void execute_lb(Cpu *cpu, Instruction inst){
    //
}

void execute_lh(Cpu *cpu, Instruction inst){
    //
}

void execute_lw(Cpu *cpu, Instruction inst){
    //
}

void execute_lbu(Cpu *cpu, Instruction inst){
    //
}

void execute_lhu(Cpu *cpu, Instruction inst){
    //
}

void execute_sb(Cpu *cpu, Instruction inst){
    //
}

void execute_sh(Cpu *cpu, Instruction inst){
    //
}

void execute_sw(Cpu *cpu, Instruction inst){
    //
}

void execute_addi(Cpu *cpu, Instruction inst){
    //
}

void execute_slti(Cpu *cpu, Instruction inst){
    //
}

void execute_sltiu(Cpu *cpu, Instruction inst){
    //
}

void execute_xori(Cpu *cpu, Instruction inst){
    //
}

void execute_ori(Cpu *cpu, Instruction inst){
    //
}

void execute_andi(Cpu *cpu, Instruction inst){
    //
}

void execute_slli(Cpu *cpu, Instruction inst){
    //
}

void execute_srli(Cpu *cpu, Instruction inst){
    //
}

void execute_srai(Cpu *cpu, Instruction inst){
    //
}

void execute_add(Cpu *cpu, Instruction inst){
    //
}

void execute_sub(Cpu *cpu, Instruction inst){
    //
}

void execute_sll(Cpu *cpu, Instruction inst){
    //
}

void execute_slt(Cpu *cpu, Instruction inst){
    //
}

void execute_sltu(Cpu *cpu, Instruction inst){
    //
}

void execute_xor(Cpu *cpu, Instruction inst){
    //
}

void execute_srl(Cpu *cpu, Instruction inst){
    //
}

void execute_sra(Cpu *cpu, Instruction inst){
    //
}

void execute_or(Cpu *cpu, Instruction inst){
    //
}

void execute_and(Cpu *cpu, Instruction inst){
    //
}

void execute_fence(Cpu *cpu, Instruction inst){
    //
}

void execute_ecall(Cpu *cpu, Instruction inst){
    //
}
