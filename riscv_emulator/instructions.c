//
// Created by bergschaf on 1/6/24.
//

#include "instructions.h"

static const char* reg_names[32] = {
        "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
        "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
        "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
        "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

Instruction decode_UType(uint32_t bin_inst){
    struct Instruction *utype = malloc(sizeof(Instruction));
    utype->type = 4;
    utype->rd = (bin_inst >> 7) & 0b11111;
    utype->imm = bin_inst >> 12;
    return *utype;
}

Instruction decode_JType(uint32_t bin_inst){
    struct Instruction *jtype = malloc(sizeof(Instruction));
    jtype->type = 5;
    jtype->rd = (bin_inst >> 7) & 0b11111;
    int imm = (bin_inst >> 12) & 0b11111111111111111111;
    int imm_20 = (imm >> 19) & 0b1;
    int imm_10_1 = (imm >> 9) & 0b1111111111;
    int imm_11 = (imm >> 8) & 0b1;
    int imm_19_12 = imm & 0b11111111;
    jtype->imm = (imm_20 << 20) | (imm_10_1 << 1) | (imm_11 << 11) | (imm_19_12 << 12);
    return *jtype;
}

Instruction decode_IType(uint32_t bin_inst){
    struct Instruction *itype = malloc(sizeof(Instruction));
    itype->type = 1;
    itype->rd = (bin_inst >> 7) & 0b11111;
    itype->funct3 = (bin_inst >> 12) & 0b111;
    itype->rs1 = (bin_inst >> 15) & 0b11111;
    itype->imm = (bin_inst >> 20);
    return *itype;
}

Instruction decode_SType(uint32_t bin_inst){
    struct Instruction *stype = malloc(sizeof(Instruction));
    stype->type = 2;
    stype->funct3 = (bin_inst >> 12) & 0b111;
    stype->rs1 = (bin_inst >> 15) & 0b11111;
    stype->rs2 = (bin_inst >> 20) & 0b11111;
    int imm = (bin_inst >> 7) & 0b1111111;
    int imm_11_5 = (imm >> 5) & 0b1111111;
    int imm_4_0 = imm & 0b11111;
    stype->imm = (imm_11_5 << 5) | imm_4_0;
    return *stype;
}

Instruction decode_BType(uint32_t bin_inst){
    struct Instruction *btype = malloc(sizeof(Instruction));
    btype->type = 3;
    btype->funct3 = (bin_inst >> 12) & 0b111;
    btype->rs1 = (bin_inst >> 15) & 0b11111;
    btype->rs2 = (bin_inst >> 20) & 0b11111;
    int imm = (bin_inst >> 7) & 0b1111111;
    int imm_12 = (imm >> 6) & 0b1;
    int imm_10_5 = (imm >> 5) & 0b111111;
    int imm_4_1 = (imm >> 1) & 0b1111;
    btype->imm = (imm_12 << 12) | (imm_10_5 << 5) | (imm_4_1 << 1);
    return *btype;
}

Instruction decode_RType(uint32_t bin_inst){
    struct Instruction *rtype = malloc(sizeof(Instruction));
    rtype->type = 0;
    rtype->rd = (bin_inst >> 7) & 0b11111;
    rtype->funct3 = (bin_inst >> 12) & 0b111;
    rtype->rs1 = (bin_inst >> 15) & 0b11111;
    rtype->rs2 = (bin_inst >> 20) & 0b11111;
    rtype->funct7 = (bin_inst >> 25) & 0b1111111;
    return *rtype;
}


void print_UType(Instruction utype){
    printf("UType:\n rd: %s\n imm: %lx\n", reg_names[utype.rd], utype.imm);
}

void print_JType(Instruction jtype){
    printf("JType:\n rd: %s\n imm: %lx\n", reg_names[jtype.rd], jtype.imm);
}

void print_IType(Instruction itype){
    printf("IType:\n rd: %s\n funct3: %d\n rs1: %s\n imm: %lx\n", reg_names[itype.rd], itype.funct3, reg_names[itype.rs1], itype.imm);
}

void print_SType(Instruction stype){
    printf("SType:\n funct3: %d\n rs1: %s\n rs2: %s\n imm: %lx\n", stype.funct3, reg_names[stype.rs1], reg_names[stype.rs2], stype.imm);
}

void print_BType(Instruction btype){
    printf("BType:\n funct3: %d\n rs1: %s\n rs2: %s\n imm: %lx\n", btype.funct3, reg_names[btype.rs1], reg_names[btype.rs2], btype.imm);
}

void print_RType(Instruction rtype){
    printf("RType:\n rd: %s\n funct3: %d\n rs1: %s\n rs2: %s\n funct7: %d\n", reg_names[rtype.rd], rtype.funct3, reg_names[rtype.rs1], reg_names[rtype.rs2], rtype.funct7);
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
