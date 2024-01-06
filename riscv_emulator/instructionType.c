//
// Created by bergschaf on 1/5/24.
//

#include <stdio.h>
#include <stdlib.h>
#include "util.c"

struct RType{
    uint8_t rd;
    uint8_t funct3;
    uint8_t rs1;
    uint8_t rs2;
    uint8_t funct7;
};
typedef struct RType RType;

struct SType {
    uint8_t funct3;
    uint8_t rs1;
    uint8_t rs2;
    uint64_t imm;
};
typedef struct SType SType;

struct IType {
    uint8_t rd;
    uint8_t funct3;
    uint8_t rs1;
    int64_t imm;
};
typedef struct IType IType;

struct UType {
    uint8_t rd;
    int64_t imm;
};
typedef struct UType UType;

struct JType {
    uint8_t rd;
    int64_t imm;
};
typedef struct JType JType;

UType decode_UType(int64_t inst){
    struct UType *utype = malloc(sizeof(struct UType));
    utype->rd = (inst >> 7) & 0b11111;
    utype->imm = inst >> 12;
    return *utype;
}

void print_UType(UType utype){
    printf("UType:\n rd:");
    print_bin(utype.rd);
    printf("\n imm:");
    print_bin(utype.imm);
    printf("\n");
}

struct BType {
    uint8_t funct3;
    uint8_t rs1;
    uint8_t rs2;
    int64_t imm;
};
typedef struct BType BType;


