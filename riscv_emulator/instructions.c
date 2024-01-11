//
// Created by bergschaf on 1/6/24.
//

#include "instructions.h"


Instruction decode_UType(uint32_t bin_inst) {
    struct Instruction *utype = malloc(sizeof(Instruction));
    utype->type = 4;
    utype->rd = (bin_inst >> 7) & 0b11111;
    utype->imm = sign_extend(bin_inst & 0xFFFFF000, 32);
    return *utype;
}

Instruction decode_JType(uint32_t bin_inst) {
    struct Instruction *jtype = malloc(sizeof(Instruction));
    jtype->type = 5;
    jtype->rd = (bin_inst >> 7) & 0b11111;
    int imm = (bin_inst >> 12) & 0b11111111111111111111;
    int imm_20 = (imm >> 19) & 0b1;
    int imm_10_1 = (imm >> 9) & 0b1111111111;
    int imm_11 = (imm >> 8) & 0b1;
    int imm_19_12 = imm & 0b11111111;
    jtype->imm = sign_extend((imm_20 << 20) | (imm_10_1 << 1) | (imm_11 << 11) | (imm_19_12 << 12), 21);
    return *jtype;
}

Instruction decode_IType(uint32_t bin_inst) {
    struct Instruction *itype = malloc(sizeof(Instruction));
    itype->type = 1;
    itype->rd = (bin_inst >> 7) & 0b11111;
    itype->funct3 = (bin_inst >> 12) & 0b111;
    itype->rs1 = (bin_inst >> 15) & 0b11111;
    itype->imm = sign_extend(bin_inst >> 20, 12);
    return *itype;
}

Instruction decode_SType(uint32_t bin_inst) {
    struct Instruction *stype = malloc(sizeof(Instruction));
    stype->type = 2;
    stype->funct3 = (bin_inst >> 12) & 0b111;
    stype->rs1 = (bin_inst >> 15) & 0b11111;
    stype->rs2 = (bin_inst >> 20) & 0b11111;
    int imm_11_5 = (bin_inst >> 25) & 0b1111111;
    int imm_4_0 = (bin_inst >> 7) & 0b11111;
    stype->imm = sign_extend((imm_11_5 << 5) | imm_4_0, 12);
    return *stype;
}

Instruction decode_BType(uint32_t bin_inst) {
    struct Instruction *btype = malloc(sizeof(Instruction));
    btype->type = 3;
    btype->funct3 = (bin_inst >> 12) & 0b111;
    btype->rs1 = (bin_inst >> 15) & 0b11111;
    btype->rs2 = (bin_inst >> 20) & 0b11111;
    int imm_11 = (bin_inst >> 31) & 0b1;
    int imm_12 = (bin_inst >> 7) & 0b1;
    int imm_10_5 = (bin_inst >> 25) & 0b111111;
    int imm_4_1 = (bin_inst >> 8) & 0b1111;
    btype->imm = sign_extend((imm_12 << 12) | (imm_11 << 11) | (imm_10_5 << 5) | imm_4_1 << 1, 12);
    return *btype;
}

Instruction decode_RType(uint32_t bin_inst) {
    struct Instruction *rtype = malloc(sizeof(Instruction));
    rtype->type = 0;
    rtype->rd = (bin_inst >> 7) & 0b11111;
    rtype->funct3 = (bin_inst >> 12) & 0b111;
    rtype->rs1 = (bin_inst >> 15) & 0b11111;
    rtype->rs2 = (bin_inst >> 20) & 0b11111;
    rtype->funct7 = (bin_inst >> 25) & 0b1111111;
    return *rtype;
}


void print_UType(Instruction utype) {
    printf("UType:\n rd: %s\n imm: %lx | %ld\n", reg_names[utype.rd], utype.imm, utype.imm);
}

void print_JType(Instruction jtype) {
    printf("JType:\n rd: %s\n imm: %lx | %ld\n", reg_names[jtype.rd], jtype.imm, jtype.imm);
}

void print_IType(Instruction itype) {
    printf("IType:\n rd: %s\n funct3: %d\n rs1: %s\n imm: %lx | %ld\n", reg_names[itype.rd], itype.funct3,
           reg_names[itype.rs1], itype.imm, itype.imm);
}

void print_SType(Instruction stype) {
    printf("SType:\n funct3: %d\n rs1: %s\n rs2: %s\n imm: %lx | %ld\n", stype.funct3, reg_names[stype.rs1],
           reg_names[stype.rs2], stype.imm, stype.imm);
}

void print_BType(Instruction btype) {
    printf("BType:\n funct3: %d\n rs1: %s\n rs2: %s\n imm: %lx | %ld\n", btype.funct3, reg_names[btype.rs1],
           reg_names[btype.rs2], btype.imm, btype.imm);
}

void print_RType(Instruction rtype) {
    printf("RType:\n rd: %s\n funct3: %d\n rs1: %s\n rs2: %s\n funct7: %d\n", reg_names[rtype.rd], rtype.funct3,
           reg_names[rtype.rs1], reg_names[rtype.rs2], rtype.funct7);
}


void print_Instruction(Instruction inst) {
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

void execute_lui(Cpu *cpu, Instruction inst) {
    // U-Type
    cpu->regs[inst.rd] = inst.imm;
}

void execute_auipc(Cpu *cpu, Instruction inst) {
    // U-Type
    cpu->regs[inst.rd] = cpu->pc + inst.imm;
}

void execute_jal(Cpu *cpu, Instruction inst) {
    // J-Type
    cpu->regs[inst.rd] = cpu->pc + 4;
    cpu->pc += inst.imm;
}

void execute_jalr(Cpu *cpu, Instruction inst) {
    // I-Type
    cpu->regs[inst.rd] = cpu->pc + 4;
    cpu->pc = cpu->regs[inst.rs1] + inst.imm;
}

void execute_beq(Cpu *cpu, Instruction inst) {
    // B-Type
    if (cpu->regs[inst.rs1] == cpu->regs[inst.rs2]) {
        cpu->pc += inst.imm;
    }
}

void execute_bne(Cpu *cpu, Instruction inst) {
    // B-Type
    if (cpu->regs[inst.rs1] != cpu->regs[inst.rs2]) {
        cpu->pc += inst.imm;
    }
}

void execute_blt(Cpu *cpu, Instruction inst) {
    // B-Type
    if (cpu->regs[inst.rs1] < cpu->regs[inst.rs2]) {
        cpu->pc += inst.imm;
    }
}

void execute_bge(Cpu *cpu, Instruction inst) {
    // B-Type
    if (cpu->regs[inst.rs1] >= cpu->regs[inst.rs2]) {
        cpu->pc += inst.imm;
    }
}

void execute_bltu(Cpu *cpu, Instruction inst) {
    // B-Type
    if ((uint64_t) cpu->regs[inst.rs1] < (uint64_t) cpu->regs[inst.rs2]) {
        cpu->pc += inst.imm;
    }
}

void execute_bgeu(Cpu *cpu, Instruction inst) {
    // B-Type
    if ((uint64_t) cpu->regs[inst.rs1] >= (uint64_t) cpu->regs[inst.rs2]) {
        cpu->pc += inst.imm;
    }
}

void execute_lb(Cpu *cpu, Instruction inst) {
    // I-Type
    cpu->regs[inst.rd] = sign_extend(cpu->mem[cpu->regs[inst.rs1] + inst.imm], 8);
}

void execute_lh(Cpu *cpu, Instruction inst) {
    // I-Type
    cpu->regs[inst.rd] = sign_extend(
            (cpu->mem[cpu->regs[inst.rs1] + inst.imm + 1] << 8) | cpu->mem[cpu->regs[inst.rs1] + inst.imm], 16);
}

void execute_lw(Cpu *cpu, Instruction inst) {
    // I-Type
    cpu->regs[inst.rd] = sign_extend(
            (cpu->mem[cpu->regs[inst.rs1] + inst.imm + 3] << 24) |
            (cpu->mem[cpu->regs[inst.rs1] + inst.imm + 2] << 16) |
            (cpu->mem[cpu->regs[inst.rs1] + inst.imm + 1] << 8) |
            cpu->mem[cpu->regs[inst.rs1] + inst.imm], 32);
}

void execute_lbu(Cpu *cpu, Instruction inst) {
    // I-Type
    //printf("lbu: 0x%lx\n", cpu->regs[inst.rs1] + inst.imm);
    cpu->regs[inst.rd] = cpu->mem[cpu->regs[inst.rs1] + inst.imm];
}

void execute_lhu(Cpu *cpu, Instruction inst) {
    // I-Type
    cpu->regs[inst.rd] = (cpu->mem[cpu->regs[inst.rs1] + inst.imm + 1] << 8) | cpu->mem[cpu->regs[inst.rs1] + inst.imm];
}

void execute_sb(Cpu *cpu, Instruction inst) {
    // S-Type
    // print imm and rs1
    //printf("imm: %lx\n", inst.imm);
    //printf("rs1: %lx\n", cpu->regs[inst.rs1]);
    //printf("SB: %lx\n", cpu->regs[inst.rs1] + inst.imm);
    cpu->mem[cpu->regs[inst.rs1] + inst.imm] = cpu->regs[inst.rs2] & 0xFF;
}

void execute_sh(Cpu *cpu, Instruction inst) {
    // S-Type
    cpu->mem[cpu->regs[inst.rs1] + inst.imm + 1] = (cpu->regs[inst.rs2] >> 8) & 0xFF;
    cpu->mem[cpu->regs[inst.rs1] + inst.imm] = cpu->regs[inst.rs2] & 0xFF;
}

void execute_sw(Cpu *cpu, Instruction inst) {
    // S-Type
    cpu->mem[cpu->regs[inst.rs1] + inst.imm + 3] = (cpu->regs[inst.rs2] >> 24) & 0xFF;
    cpu->mem[cpu->regs[inst.rs1] + inst.imm + 2] = (cpu->regs[inst.rs2] >> 16) & 0xFF;
    cpu->mem[cpu->regs[inst.rs1] + inst.imm + 1] = (cpu->regs[inst.rs2] >> 8) & 0xFF;
    cpu->mem[cpu->regs[inst.rs1] + inst.imm] = cpu->regs[inst.rs2] & 0xFF;
}

void execute_addi(Cpu *cpu, Instruction inst) {
    // I-Type
    cpu->regs[inst.rd] = cpu->regs[inst.rs1] + inst.imm;
}

void execute_slti(Cpu *cpu, Instruction inst) {
    // I-Type
    cpu->regs[inst.rd] = cpu->regs[inst.rs1] < inst.imm;
}

void execute_sltiu(Cpu *cpu, Instruction inst) {
    // I-Type
    cpu->regs[inst.rd] = (uint64_t) cpu->regs[inst.rs1] < (uint64_t) inst.imm;
}

void execute_xori(Cpu *cpu, Instruction inst) {
    // I-Type
    cpu->regs[inst.rd] = cpu->regs[inst.rs1] ^ inst.imm;
}

void execute_ori(Cpu *cpu, Instruction inst) {
    // I-Type
    cpu->regs[inst.rd] = cpu->regs[inst.rs1] | inst.imm;
}

void execute_andi(Cpu *cpu, Instruction inst) {
    // I-Type
    cpu->regs[inst.rd] = cpu->regs[inst.rs1] & inst.imm;
}

void execute_slli(Cpu *cpu, Instruction inst) {
    // I-Type
    cpu->regs[inst.rd] = cpu->regs[inst.rs1] << inst.imm;
}

void execute_srli(Cpu *cpu, Instruction inst) {
    // I-Type
    cpu->regs[inst.rd] = (uint64_t) cpu->regs[inst.rs1] >> inst.imm;
}

void execute_srai(Cpu *cpu, Instruction inst) {
    // I-Type
    cpu->regs[inst.rd] = cpu->regs[inst.rs1] >> inst.imm;
}

void execute_add(Cpu *cpu, Instruction inst) {
    // R-Type
    cpu->regs[inst.rd] = cpu->regs[inst.rs1] + cpu->regs[inst.rs2];
}

void execute_sub(Cpu *cpu, Instruction inst) {
    // R-Type
    cpu->regs[inst.rd] = cpu->regs[inst.rs1] - cpu->regs[inst.rs2];
}

void execute_sll(Cpu *cpu, Instruction inst) {
    // R-Type
    cpu->regs[inst.rd] = cpu->regs[inst.rs1] << (cpu->regs[inst.rs2] & 0b111111);
}

void execute_slt(Cpu *cpu, Instruction inst) {
    // R-Type
    cpu->regs[inst.rd] = cpu->regs[inst.rs1] < cpu->regs[inst.rs2];
}

void execute_sltu(Cpu *cpu, Instruction inst) {
    // R-Type
    cpu->regs[inst.rd] = (uint64_t) cpu->regs[inst.rs1] < (uint64_t) cpu->regs[inst.rs2];
}

void execute_xor(Cpu *cpu, Instruction inst) {
    // R-Type
    cpu->regs[inst.rd] = cpu->regs[inst.rs1] ^ cpu->regs[inst.rs2];
}

void execute_srl(Cpu *cpu, Instruction inst) {
    // R-Type
    cpu->regs[inst.rd] = (uint64_t) cpu->regs[inst.rs1] >> (cpu->regs[inst.rs2] & 0b111111);
}

void execute_sra(Cpu *cpu, Instruction inst) {
    // R-Type
    cpu->regs[inst.rd] = cpu->regs[inst.rs1] >> (cpu->regs[inst.rs2] & 0b111111);
}

void execute_or(Cpu *cpu, Instruction inst) {
    // R-Type
    cpu->regs[inst.rd] = cpu->regs[inst.rs1] | cpu->regs[inst.rs2];
}

void execute_and(Cpu *cpu, Instruction inst) {
    // R-Type
    cpu->regs[inst.rd] = cpu->regs[inst.rs1] & cpu->regs[inst.rs2];
}

void execute_fence(Cpu *cpu, Instruction inst) {
    // no-op
}

void execute_ecall(Cpu *cpu, Instruction inst) {
    do_syscall(cpu);
}

void execute_ebreak(Cpu *cpu, Instruction inst) {
    // no-op
}

void execute_lwu(Cpu *cpu, Instruction inst) {
    // I-Type
    cpu->regs[inst.rd] =  (cpu->mem[cpu->regs[inst.rs1] + inst.imm + 3] << 24) |
                          (cpu->mem[cpu->regs[inst.rs1] + inst.imm + 2] << 16) |
                          (cpu->mem[cpu->regs[inst.rs1] + inst.imm + 1] << 8) |
                          cpu->mem[cpu->regs[inst.rs1] + inst.imm];
}

void execute_ld(Cpu *cpu, Instruction inst) {
    /*
    printf("ld: 0x%lx\n", cpu->regs[inst.rs1] + inst.imm);
    // print the bytes as they are in memory
    printf("ld: 0x%x\n", cpu->mem[cpu->regs[inst.rs1] + inst.imm + 7]);
    printf("ld: 0x%x\n", cpu->mem[cpu->regs[inst.rs1] + inst.imm + 6]);
    printf("ld: 0x%x\n", cpu->mem[cpu->regs[inst.rs1] + inst.imm + 5]);
    printf("ld: 0x%x\n", cpu->mem[cpu->regs[inst.rs1] + inst.imm + 4]);
    printf("ld: 0x%x\n", cpu->mem[cpu->regs[inst.rs1] + inst.imm + 3]);
    printf("ld: 0x%x\n", cpu->mem[cpu->regs[inst.rs1] + inst.imm + 2]);
    printf("ld: 0x%x\n", cpu->mem[cpu->regs[inst.rs1] + inst.imm + 1]);
    printf("ld: 0x%x\n", cpu->mem[cpu->regs[inst.rs1] + inst.imm]);
    printf("\n\n");
    // print the shifted parts that are ored together
    printf("ld: 0x%lx\n", ((int64_t)cpu->mem[cpu->regs[inst.rs1] + inst.imm + 7] << 56));
    printf("ld: 0x%lx\n", ((uint64_t)cpu->mem[cpu->regs[inst.rs1] + inst.imm + 6] << 48));
    printf("ld: 0x%lx\n", ((uint64_t)cpu->mem[cpu->regs[inst.rs1] + inst.imm + 5] << 40));
    printf("ld: 0x%lx\n", ((uint64_t)cpu->mem[cpu->regs[inst.rs1] + inst.imm + 4] << 32));
    printf("ld: 0x%lx\n", (cpu->mem[cpu->regs[inst.rs1] + inst.imm + 3] << 24));
    printf("ld: 0x%lx\n", (cpu->mem[cpu->regs[inst.rs1] + inst.imm + 2] << 16));
    printf("ld: 0x%lx\n", (cpu->mem[cpu->regs[inst.rs1] + inst.imm + 1] << 8));
    printf("ld: 0x%lx\n", cpu->mem[cpu->regs[inst.rs1] + inst.imm]);*/


    // I-Type
    cpu->regs[inst.rd] = ((uint64_t)cpu->mem[cpu->regs[inst.rs1] + inst.imm + 7] << 56) |
                          ((uint64_t)cpu->mem[cpu->regs[inst.rs1] + inst.imm + 6] << 48) |
                          ((uint64_t)cpu->mem[cpu->regs[inst.rs1] + inst.imm + 5] << 40) |
                          ((uint64_t)cpu->mem[cpu->regs[inst.rs1] + inst.imm + 4] << 32) |
                          ((int64_t)cpu->mem[cpu->regs[inst.rs1] + inst.imm + 3] << 24) |
                          ((int64_t)cpu->mem[cpu->regs[inst.rs1] + inst.imm + 2] << 16) |
                          ((int64_t)cpu->mem[cpu->regs[inst.rs1] + inst.imm + 1] << 8) |
                            (int64_t)cpu->mem[cpu->regs[inst.rs1] + inst.imm];
}

void execute_sd(Cpu *cpu, Instruction inst) {
    // S-Type
    cpu->mem[cpu->regs[inst.rs1] + inst.imm + 7] = (cpu->regs[inst.rs2] >> 56) & 0xFF;
    cpu->mem[cpu->regs[inst.rs1] + inst.imm + 6] = (cpu->regs[inst.rs2] >> 48) & 0xFF;
    cpu->mem[cpu->regs[inst.rs1] + inst.imm + 5] = (cpu->regs[inst.rs2] >> 40) & 0xFF;
    cpu->mem[cpu->regs[inst.rs1] + inst.imm + 4] = (cpu->regs[inst.rs2] >> 32) & 0xFF;
    cpu->mem[cpu->regs[inst.rs1] + inst.imm + 3] = (cpu->regs[inst.rs2] >> 24) & 0xFF;
    cpu->mem[cpu->regs[inst.rs1] + inst.imm + 2] = (cpu->regs[inst.rs2] >> 16) & 0xFF;
    cpu->mem[cpu->regs[inst.rs1] + inst.imm + 1] = (cpu->regs[inst.rs2] >> 8) & 0xFF;
    cpu->mem[cpu->regs[inst.rs1] + inst.imm] = cpu->regs[inst.rs2] & 0xFF;
}

void execute_addiw(Cpu *cpu, Instruction inst) {
    // I-Type
    cpu->regs[inst.rd] = sign_extend((int32_t) cpu->regs[inst.rs1] + inst.imm, 32);
}

void execute_slliw(Cpu *cpu, Instruction inst) {
    // I-Type
    cpu->regs[inst.rd] = sign_extend((int32_t) cpu->regs[inst.rs1] << (inst.imm & 0b11111), 32);
}

void execute_srliw(Cpu *cpu, Instruction inst) {
    // I-Type
    cpu->regs[inst.rd] = sign_extend((uint32_t) cpu->regs[inst.rs1] >> (inst.imm & 0b11111), 32);
}

void execute_sraiw(Cpu *cpu, Instruction inst) {
    // I-Type
    cpu->regs[inst.rd] = sign_extend((int32_t) cpu->regs[inst.rs1] >> (inst.imm & 0b11111), 32);
}

void execute_addw(Cpu *cpu, Instruction inst) {
    // R-Type
    cpu->regs[inst.rd] = sign_extend((int32_t) cpu->regs[inst.rs1] + (int32_t) cpu->regs[inst.rs2], 32);
}

void execute_subw(Cpu *cpu, Instruction inst) {
    // R-Type
    cpu->regs[inst.rd] = sign_extend((int32_t) cpu->regs[inst.rs1] - (int32_t) cpu->regs[inst.rs2], 32);
}

void execute_sllw(Cpu *cpu, Instruction inst) {
    // R-Type
    cpu->regs[inst.rd] = sign_extend((int32_t) cpu->regs[inst.rs1] << (cpu->regs[inst.rs2] & 0b11111), 32);
}

void execute_srlw(Cpu *cpu, Instruction inst) {
    // R-Type
    cpu->regs[inst.rd] = sign_extend((uint32_t) cpu->regs[inst.rs1] >> (cpu->regs[inst.rs2] & 0b11111), 32);
}

void execute_sraw(Cpu *cpu, Instruction inst) {
    // R-Type
    cpu->regs[inst.rd] = sign_extend((int32_t) cpu->regs[inst.rs1] >> (cpu->regs[inst.rs2] & 0b11111), 32);
}
