//
// Created by bergschaf on 1/6/24.
//

#ifndef RISCV_EMULATOR_INSTRUCTIONS_H
#define RISCV_EMULATOR_INSTRUCTIONS_H

#include <stdint.h>
#include "config.h"
#include "cpu.h"
#include "kernel.h"

static const char *reg_names[32] = {
        "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
        "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
        "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
        "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

struct Instruction {
    uint8_t rd;
    uint8_t funct3;
    uint8_t rs1;
    uint8_t rs2;
    uint8_t funct7;
    int64_t imm;
    uint8_t type; // 0 = RType, 1 = IType, 2 = SType, 3 = BType, 4 = UType, 5 = JType
};
typedef struct Instruction Instruction;

Instruction *decode_UType(uint32_t bin_inst);

Instruction *decode_JType(uint32_t bin_inst);

Instruction *decode_IType(uint32_t bin_inst);

Instruction *decode_SType(uint32_t bin_inst);

Instruction *decode_BType(uint32_t bin_inst);

Instruction *decode_RType(uint32_t bin_inst);


void print_Instruction(Instruction inst);

typedef void (*execute_instruction)(Cpu *cpu, Instruction inst);

void execute_lui(Cpu *cpu, Instruction inst);

void execute_auipc(Cpu *cpu, Instruction inst);

void execute_jal(Cpu *cpu, Instruction inst);

void execute_jalr(Cpu *cpu, Instruction inst);

void execute_beq(Cpu *cpu, Instruction inst);

void execute_bne(Cpu *cpu, Instruction inst);

void execute_blt(Cpu *cpu, Instruction inst);

void execute_bge(Cpu *cpu, Instruction inst);

void execute_bltu(Cpu *cpu, Instruction inst);

void execute_bgeu(Cpu *cpu, Instruction inst);

void execute_lb(Cpu *cpu, Instruction inst);

void execute_lh(Cpu *cpu, Instruction inst);

void execute_lw(Cpu *cpu, Instruction inst);

void execute_lbu(Cpu *cpu, Instruction inst);

void execute_lhu(Cpu *cpu, Instruction inst);

void execute_sb(Cpu *cpu, Instruction inst);

void execute_sh(Cpu *cpu, Instruction inst);

void execute_sw(Cpu *cpu, Instruction inst);

void execute_addi(Cpu *cpu, Instruction inst);

void execute_slti(Cpu *cpu, Instruction inst);

void execute_sltiu(Cpu *cpu, Instruction inst);

void execute_xori(Cpu *cpu, Instruction inst);

void execute_ori(Cpu *cpu, Instruction inst);

void execute_andi(Cpu *cpu, Instruction inst);

void execute_slli(Cpu *cpu, Instruction inst);

void execute_srli(Cpu *cpu, Instruction inst);

void execute_srai(Cpu *cpu, Instruction inst);

void execute_add(Cpu *cpu, Instruction inst);

void execute_sub(Cpu *cpu, Instruction inst);

void execute_sll(Cpu *cpu, Instruction inst);

void execute_slt(Cpu *cpu, Instruction inst);

void execute_sltu(Cpu *cpu, Instruction inst);

void execute_xor(Cpu *cpu, Instruction inst);

void execute_srl(Cpu *cpu, Instruction inst);

void execute_sra(Cpu *cpu, Instruction inst);

void execute_or(Cpu *cpu, Instruction inst);

void execute_and(Cpu *cpu, Instruction inst);

void execute_fence(Cpu *cpu, Instruction inst);

void execute_ecall(Cpu *cpu, Instruction inst);

void execute_ebreak(Cpu *cpu, Instruction inst);

void execute_lwu(Cpu *cpu, Instruction inst);

void execute_ld(Cpu *cpu, Instruction inst);

void execute_sd(Cpu *cpu, Instruction inst);

void execute_addiw(Cpu *cpu, Instruction inst);

void execute_slliw(Cpu *cpu, Instruction inst);

void execute_srliw(Cpu *cpu, Instruction inst);

void execute_sraiw(Cpu *cpu, Instruction inst);

void execute_addw(Cpu *cpu, Instruction inst);

void execute_subw(Cpu *cpu, Instruction inst);

void execute_sllw(Cpu *cpu, Instruction inst);

void execute_srlw(Cpu *cpu, Instruction inst);

void execute_sraw(Cpu *cpu, Instruction inst);

#endif //RISCV_EMULATOR_INSTRUCTIONS_H
