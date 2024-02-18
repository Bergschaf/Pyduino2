use crate::cpu::Cpu;
use crate::emulator::Emulator;
use crate::kernel::do_syscall;

pub fn sign_extend(val: u64, from: u8) -> i64 {
    let shift = 64 - from;
    ((val << shift) as i64 >> shift)
}

fn crop_to_32_and_sign_extend(val: i64) -> i64 {
    sign_extend(val as u64, 32) as i64
}

#[derive(Debug)]
pub struct RType {
    rd: u8,
    funct3: u8,
    rs1: u8,
    rs2: u8,
    funct7: u8,
}

#[derive(Debug)]
pub struct IType {
    rd: u8,
    funct3: u8,
    rs1: u8,
    imm: i32,
}

#[derive(Debug)]
pub struct SType {
    imm: i32,
    funct3: u8,
    rs1: u8,
    rs2: u8,
}

#[derive(Debug)]
pub struct BType {
    imm: i32,
    funct3: u8,
    rs1: u8,
    rs2: u8,
}

#[derive(Debug)]
pub struct UType {
    rd: u8,
    imm: i32,
}

#[derive(Debug)]
pub struct JType {
    rd: u8,
    imm: i32,
}

pub fn parse_r_type(inst: u64) -> RType {
    RType {
        rd: ((inst >> 7) & 0b11111) as u8,
        funct3: ((inst >> 12) & 0b111) as u8,
        rs1: ((inst >> 15) & 0b11111) as u8,
        rs2: ((inst >> 20) & 0b11111) as u8,
        funct7: ((inst >> 25) & 0b1111111) as u8,
    }
}

pub fn parse_i_type(inst: u64) -> IType {
    IType {
        rd: ((inst >> 7) & 0b11111) as u8,
        funct3: ((inst >> 12) & 0b111) as u8,
        rs1: ((inst >> 15) & 0b11111) as u8,
        imm: sign_extend(((inst >> 20) & 0b111111111111), 12) as i32,
    }
}

pub fn parse_s_type(inst: u64) -> SType {
    SType {
        imm: sign_extend((((inst >> 7) & 0b11111) | ((inst >> 25) & 0b1111111) << 5), 12) as i32,
        funct3: ((inst >> 12) & 0b111) as u8,
        rs1: ((inst >> 15) & 0b11111) as u8,
        rs2: ((inst >> 20) & 0b11111) as u8,
    }
}

pub fn parse_b_type(inst: u64) -> BType {
    BType {
        imm: sign_extend((((inst >> 8) & 0b1111) | ((inst >> 25) & 0b111111) << 4 | ((inst >> 7) & 0b1) << 10 | ((inst >> 31) & 0b1) << 11) << 1, 13) as i32,
        funct3: ((inst >> 12) & 0b111) as u8,
        rs1: ((inst >> 15) & 0b11111) as u8,
        rs2: ((inst >> 20) & 0b11111) as u8,
    }
}

pub fn parse_u_type(inst: u64) -> UType {
    UType {
        rd: ((inst >> 7) & 0b11111) as u8,
        imm: sign_extend(((inst >> 12) & 0b11111111111111111111) << 12, 32) as i32,
    }
}

pub fn parse_j_type(inst: u64) -> JType {
    JType {
        rd: ((inst >> 7) & 0b11111) as u8,
        imm: sign_extend((((inst >> 21) & 0b1111111111) | ((inst >> 20) & 0b1) << 10 | ((inst >> 12) & 0b11111111) << 11 | ((inst >> 31) & 0b1) << 19) << 1, 21) as i32,
    }
}

#[derive(Debug)]
pub enum Instruction {
    NOP,
    LUI(UType),
    AUIPC(UType),
    JAL(JType),
    JALR(IType),
    BEQ(BType),
    BNE(BType),
    BLT(BType),
    BGE(BType),
    BLTU(BType),
    BGEU(BType),
    LB(IType),
    LH(IType),
    LW(IType),
    LD(IType),
    LWU(IType),
    LBU(IType),
    LHU(IType),
    SB(SType),
    SH(SType),
    SW(SType),
    SD(SType),
    ADDI(IType),
    SLTI(IType),
    SLTIU(IType),
    XORI(IType),
    ORI(IType),
    ANDI(IType),
    SLLI(IType),
    SRLI(IType),
    SRAI(IType),
    ADD(RType),
    SUB(RType),
    SLL(RType),
    SLT(RType),
    SLTU(RType),
    XOR(RType),
    SRL(RType),
    SRA(RType),
    OR(RType),
    AND(RType),
    FENCE,
    FENCE_I,
    ECALL,
    EBREAK,

    ADDIW(IType),
    SLLIW(IType),
    SRLIW(IType),
    SRAIW(IType),
    ADDW(RType),
    SUBW(RType),
    SLLW(RType),
    SRLW(RType),
    SRAW(RType),

    // weird instructions
    CSRRW(IType),
    CSRRS(IType),
    CSRRC(IType),
    CSRRWI(IType),
    CSRRSI(IType),
    CSRRCI(IType),
}

pub fn decode_CR(inst: u16) -> (u8, u8, u8) { // funct (last bit), rd/rs1, rs2
    let funct = (inst >> 12) & 0b1;
    let rd = (inst >> 7) & 0b11111;
    let rs2 = (inst >> 2) & 0b11111;
    (funct as u8, rd as u8, rs2 as u8)
}

pub fn decode_CI(inst: u16) -> (u8, i32) { // rd, imm
    let rd = (inst >> 7) & 0b11111;
    let imm_5 = (inst >> 12) & 0b1;
    let imm_4_0 = (inst >> 2) & 0b11111;
    let imm = (imm_5 << 5) | imm_4_0;
    (rd as u8, imm as i32)
}

pub fn decode_CSS(inst: u16) -> (u8, i32) { // rs2, imm
    let rs2 = (inst >> 2) & 0b11111;
    let imm = (inst >> 7) & 0b111111;
    (rs2 as u8, imm as i32)
}

pub fn decode_CIW(inst: u16) -> (u8, i32) { // rd', imm
    let rd_ = (inst >> 2) & 0b111;
    let imm_3 = (inst >> 5) & 0b1;
    let imm_2 = (inst >> 6) & 0b1;
    let imm_9_6 = (inst >> 7) & 0b1111;
    let imm_5_4 = (inst >> 10) & 0b11;
    let imm = (imm_9_6 << 6) | (imm_5_4 << 4) | (imm_3 << 3) | (imm_2 << 2);
    (rd_ as u8, imm as i32)
}

pub fn decode_CL_CS(inst: u16) -> (u8, u8, i32) { // rs1', rd'/rs2', imm
    let rd_ = (inst >> 2) & 0b111;
    let rs1_ = (inst >> 7) & 0b111;
    let imm_1_2 = (inst >> 5) & 0b11;
    let imm_rest = (inst >> 10) & 0b111;
    let imm = (imm_rest << 2) | imm_1_2;
    (rs1_ as u8, rd_ as u8, imm as i32)
}

pub fn decode_CB(inst: u16) -> (u8, i32) { // rs1', offset
    let rs1_ = (inst >> 7) & 0b111;
    let offset_start = (inst >> 2) & 0b11111;
    let offset_end = (inst >> 10) & 0b111;
    let offset = (offset_end << 5) | offset_start;
    (rs1_ as u8, offset as i32)
}

pub fn decode_CJ(inst: u16) -> (i32) { // jump target
    let inst = inst >> 2;
    let imm_5 = inst & 0b1;
    let imm_3_1 = (inst >> 1) & 0b111;
    let imm_7 = (inst >> 4) & 0b1;
    let imm_6 = (inst >> 5) & 0b1;
    let imm_10 = (inst >> 6) & 0b1;
    let imm_9_8 = (inst >> 7) & 0b11;
    let imm_4 = (inst >> 9) & 0b1;
    let imm_11 = (inst >> 10) & 0b1;
    let imm = (imm_11 << 11) | (imm_10 << 10) | (imm_9_8 << 8) | (imm_7 << 7) | (imm_6 << 6) | (imm_5 << 5) | (imm_4 << 4) | (imm_3_1 << 1);
    let imm = sign_extend(imm as u64, 12) as i32;
    imm
}

pub fn map_rvc_register(reg: u8) -> u8 {
    match reg {
        0b000 => 8,
        0b001 => 9,
        0b010 => 10,
        0b011 => 11,
        0b100 => 12,
        0b101 => 13,
        0b110 => 14,
        0b111 => 15,
        _ => panic!("Unknown register mapping"),
    }
}

pub fn decode_compressed_instruction(inst: u64) -> Option<Instruction> {
    let opcode = inst & 0b11;
    let funct3 = (inst >> 13) & 0b111;
    match opcode {
        0b11 => None,
        0b10 => {
            match funct3 {
                0b000 => {
                    // C.SLL
                    let (rd, imm) = decode_CI(inst as u16);
                    Some(Instruction::SLLI(IType {
                        rd: rd,
                        funct3: 0b001,
                        rs1: rd,
                        imm,
                    }))
                }
                0b001 => {
                    panic!("Floating point instructions not supported")
                }
                0b010 => {
                    // C.LWSP
                    let (rd, imm) = decode_CI(inst as u16);
                    let imm_7_6 = imm & 0b11;
                    let imm_4_2 = (imm >> 2) & 0b111;
                    let imm_5 = (imm >> 5) & 0b1;
                    let imm = (imm_7_6 << 6) | (imm_5 << 5) | (imm_4_2 << 2);
                    Some(Instruction::LW(IType {
                        rd: rd,
                        funct3: 0b10,
                        rs1: 2,
                        imm,
                    }))
                }
                0b011 => {
                    // C.LDSP
                    let (rd, imm) = decode_CI(inst as u16);
                    let imm_8_6 = imm & 0b111;
                    let imm_4_3 = (imm >> 3) & 0b11;
                    let imm_5 = (imm >> 5) & 0b1;
                    let imm = (imm_8_6 << 6) | (imm_5 << 5) | (imm_4_3 << 3);
                    Some(Instruction::LD(IType {
                        rd: rd,
                        funct3: 0b11,
                        rs1: 2,
                        imm,
                    }))
                }
                0b100 => {
                    let (funct_4, rsd, rs2) = decode_CR(inst as u16);
                    if (funct_4 == 0) {
                        if (rs2 == 0) {
                            // C.JR
                            Some(Instruction::JALR(IType {
                                rd: 0,
                                funct3: 0b000,
                                rs1: rsd,
                                imm: 0,
                            }))
                        } else {
                            // C.MV
                            Some(Instruction::ADD / RType {
                                rd: rsd,
                                funct3: 0b000,
                                rs1: rs2,
                                rs2: 0,
                                funct7: 0b0000000,
                            })
                        }
                    } else {
                        if (rsd == 0 && rs2 == 0) {
                            // C.EBREAK
                            Some(Instruction::EBREAK)
                        } else if (rs2 == 0) {
                            // C.JALR
                            Some(Instruction::JALR(IType {
                                rd: rsd,
                                funct3: 0b000,
                                rs1: rsd,
                                imm: 0,
                            }))
                        } else {
                            // C.ADD
                            Some(Instruction::ADD / RType {
                                rd: rsd,
                                funct3: 0b000,
                                rs1: rsd,
                                rs2: rs2,
                                funct7: 0b0000000,
                            })
                        }
                    }
                }
                0b101 => {
                    panic!("Floating point instructions not supported")
                }
                0b110 => {
                    // C.SWSP
                    let (rs1, imm) = decode_CSS(inst as u16);
                    let imm_7_6 = imm & 0b11;
                    let imm_5_2 = (imm >> 2) & 0b1111;
                    let imm = (imm_7_6 << 6) | (imm_5_2 << 2);
                    Some(Instruction::SW(SType {
                        rs1: 2,
                        rs2: rs1,
                        imm,
                        funct3: 0b010,
                    }))
                }
                0b111 => {
                    // C.SDSP
                    let (rs1, imm) = decode_CSS(inst as u16);
                    let imm_8_6 = imm & 0b111;
                    let imm_5_3 = (imm >> 3) & 0b111;
                    let imm = (imm_8_6 << 6) | (imm_5_3 << 3);
                    Some(Instruction::SD(SType {
                        rs1: 2,
                        rs2: rs1,
                        imm,
                        funct3: 0b011,
                    }))
                }
                _ => {
                    panic!("Unknown funct3 for compressed instruction")
                }
            }
        }
        0b01 => {
            match funct3 {
                0b000 => {
                    // C.ADDI or C.NOP
                    let (rd, imm) = decode_CI(inst as u16);
                    if rd == 0 && imm == 0 {
                        return Some(Instruction::NOP);
                    };
                    Some(Instruction::ADDI(IType {
                        rd: rd,
                        funct3: 0b000,
                        rs1: rd,
                        imm,
                    }))
                }
                0b001 => {
                    // C.ADDIW
                    let (rd, imm) = decode_CI(inst as u16);
                    Some(Instruction::ADDIW(IType {
                        rd: rd,
                        funct3: 0b000,
                        rs1: rd,
                        imm,
                    }))
                }
                0b010 => {
                    // C.LI
                    let (rd, imm) = decode_CI(inst as u16);
                    // sign extend imm
                    let imm = sign_extend(imm as u64, 6) as i32;
                    Some(Instruction::ADDI(IType {
                        rd: rd,
                        funct3: 0b000,
                        rs1: 0,
                        imm,
                    }))
                }
                0b011 => {
                    // C.LUI or C.ADDI16SP
                    let (rd, imm) = decode_CI(inst as u16);
                    if (rd == 2) {
                        // C.ADDI16SP
                        let imm_5 = imm & 0b1;
                        let imm_8_7 = (imm >> 1) & 0b11;
                        let imm_6 = (imm >> 3) & 0b1;
                        let imm_4 = (imm >> 4) & 0b1;
                        let imm_9 = (imm >> 5) & 0b1;
                        let imm = (imm_9 << 9) | (imm_8_7 << 7) | (imm_6 << 6) | (imm_5 << 5) | (imm_4 << 4);
                        // TODO immediate decoding may be false
                        Some(Instruction::ADDI(IType {
                            rd: 2,
                            funct3: 0b000,
                            rs1: 2,
                            imm,
                        }))
                    } else {
                        let imm = imm << 12;
                        // TODO immiadiate decoding may be false
                        Some(Instruction::LUI(UType {
                            rd: rd,
                            imm,
                        }))
                    }
                }
                0b100 => {
                    // Some stuff
                    let funct_2 = ((inst >> 10) & 0b11) as u8;
                    let inst_12 = ((inst >> 12) & 0b1) as u8;
                    let rsd = ((inst >> 7) & 0b111) as u8;
                    let imm_start = (inst >> 2) & 0b11111;
                    match funct_2 {
                        0b00 => {
                            // C.SRLI
                            let imm = (inst_12 << 5) | imm_start;
                            Some(Instruction::SRLI(IType {
                                rd: map_rvc_register(rsd),
                                funct3: 0b101,
                                rs1: map_rvc_register(rsd),
                                imm: imm as i32,
                            }))
                        }
                        0b01 => {
                            // C.SRAI
                            let imm = (inst_12 << 5) | imm_start;
                            Some(Instruction::SRAI(IType {
                                rd: map_rvc_register(rsd),
                                funct3: 0b101,
                                rs1: map_rvc_register(rsd),
                                imm: imm as i32,
                            }))
                        }
                        0b10 => {
                            // C.ANDI
                            let imm = (inst_12 << 5) | imm_start;
                            let imm = sign_extend(imm as u64, 6) as i32;
                            Some(Instruction::ANDI(IType {
                                rd: map_rvc_register(rsd),
                                funct3: 0b111,
                                rs1: map_rvc_register(rsd),
                                imm: imm,
                            }))
                        }
                        0b11 => {
                            let another_funct_2 = (imm_start >> 3) & 0b11;
                            let rs2 = (imm_start & 0b111) as u8;
                            match another_funct_2 {
                                0b00 => {
                                    // C.SUB or C.SUBW
                                    if (inst_12) {
                                        Some(Instruction::SUBW(RType {
                                            rd: map_rvc_register(rsd),
                                            funct3: 0b000,
                                            rs1: map_rvc_register(rsd),
                                            rs2: map_rvc_register(rs2),
                                            funct7: 0b0100000,
                                        }))
                                    } else {
                                        Some(Instruction::SUB(RType {
                                            rd: map_rvc_register(rsd),
                                            funct3: 0b000,
                                            rs1: map_rvc_register(rsd),
                                            rs2: map_rvc_register(rs2),
                                            funct7: 0b0100000,
                                        }))
                                    }
                                }
                                0b01 => {
                                    // C.XOR or C.ADDW
                                    if (inst_12) {
                                        Some(Instruction::ADDW(RType {
                                            rd: map_rvc_register(rsd),
                                            funct3: 0b100,
                                            rs1: map_rvc_register(rsd),
                                            rs2: map_rvc_register(rs2),
                                            funct7: 0b0000000,
                                        }))
                                    } else {
                                        Some(Instruction::XOR(RType {
                                            rd: map_rvc_register(rsd),
                                            funct3: 0b100,
                                            rs1: map_rvc_register(rsd),
                                            rs2: map_rvc_register(rs2),
                                            funct7: 0b0000000,
                                        }))
                                    }
                                }
                                0b10 => {
                                    // C.OR or reserved
                                    if (inst_12) {
                                        panic!("reserved")
                                    } else {
                                        Some(Instruction::OR(RType {
                                            rd: map_rvc_register(rsd),
                                            funct3: 0b110,
                                            rs1: map_rvc_register(rsd),
                                            rs2: map_rvc_register(rs2),
                                            funct7: 0b0000000,
                                        }))
                                    }
                                }
                                0b11 => {
                                    // C.AND or reserved
                                    if (inst_12) {
                                        panic!("reserved")
                                    } else {
                                        Some(Instruction::AND(RType {
                                            rd: map_rvc_register(rsd),
                                            funct3: 0b111,
                                            rs1: map_rvc_register(rsd),
                                            rs2: map_rvc_register(rs2),
                                            funct7: 0b0000000,
                                        }))
                                    }
                                }
                                _ => {
                                    panic!("Unknown funct_2 for compressed instruction")
                                }
                            }
                        }
                        _ => { panic!("Unknown funct_2 for compressed instruction") }
                    }
                }
                0b101 => {
                    // C.J
                    let imm = decode_CJ(inst as u16);
                    Some(Instruction::JAL(JType {
                        rd: 0,
                        imm,
                    }))
                }
                0b110 | 0b111 => {
                    // C.BEQZ
                    let (rs1, imm) = decode_CB(inst as u16);
                    let imm_5 = imm & 0b1;
                    let imm_2_1 = (imm >> 1) & 0b11;
                    let imm_7 = (imm >> 3) & 0b1;
                    let imm_6 = (imm >> 4) & 0b1;
                    let imm_10 = (imm >> 5) & 0b1;
                    let imm_9_8 = (imm >> 6) & 0b11;
                    let imm_4 = (imm >> 8) & 0b1;
                    let imm_11 = (imm >> 9) & 0b1;
                    let imm = (imm_11 << 11) | (imm_10 << 10) | (imm_9_8 << 8) | (imm_7 << 7) | (imm_6 << 6) | (imm_5 << 5) | (imm_4 << 4) | (imm_2_1 << 1);
                    let imm = sign_extend(imm as u64, 12) as i32;
                    if (funct3 == 0b110) {
                        Some(Instruction::BEQ(BType {
                            rs1: map_rvc_register(rs1),
                            rs2: 0,
                            imm,
                            funct3: 0b000,
                        }))
                    } else {
                        Some(Instruction::BNE(BType {
                            rs1: map_rvc_register(rs1),
                            rs2: 0,
                            imm,
                            funct3: 0b001,
                        }))
                    }
                }
                _ => { panic!("Unknown funct3 for compressed instruction") }
            }
        }
        0b00 => {
            match funct3 {
                0b000 => {
                    let (rd, imm) = decode_CIW(inst as u16);
                    Some(Instruction::ADDI(IType {
                        rd: map_rvc_register(rd),
                        funct3: 0b000,
                        rs1: 2,
                        imm,
                    }))
                }
                0b001 => {
                    panic!("Floating point instructions not supported")
                }
                0b010 => {
                    let (rs1, rd, imm) = decode_CL_CS(inst as u16);
                    let imm_6 = imm & 0b1;
                    let imm_2 = (imm >> 1) & 0b1;
                    let imm_5_3 = (imm >> 2) & 0b111;
                    let imm = (imm_6 << 6) | (imm_5_3 << 3) | (imm_2 << 2);
                    Some(Instruction::LW(IType {
                        rd: map_rvc_register(rd),
                        funct3: 0b10,
                        rs1: map_rvc_register(rs1),
                        imm,
                    }))
                }
                0b011 => {
                    let (rs1, rd, imm) = decode_CL_CS(inst as u16);
                    let imm_6_7 = imm & 0b11;
                    let imm_5_3 = (imm >> 2) & 0b111;
                    let imm = (imm_6_7 << 6) | (imm_5_3 << 3);
                    Some(Instruction::LD(IType {
                        rd: map_rvc_register(rd),
                        funct3: 0b11,
                        rs1: map_rvc_register(rs1),
                        imm,
                    }))
                }
                0b100 => {
                    panic!("reserved")
                }
                0b101 => {
                    //
                    panic!("No floating point instructions")
                }
                0b110 => {
                    // C.SW
                    let (rs1, rs2, imm) = decode_CL_CS(inst as u16);
                    let imm_6 = imm & 0b1;
                    let imm_2 = (imm >> 1) & 0b1;
                    let imm_5_3 = (imm >> 2) & 0b111;
                    let imm = (imm_6 << 6) | (imm_5_3 << 3) | (imm_2 << 2);
                    Some(Instruction::SW(SType {
                        rs1: map_rvc_register(rs1),
                        funct3: 0b10,
                        rs2: map_rvc_register(rs2),
                        imm,
                    }))
                }
                0b111 => {
                    // C.SD
                    let (rs1, rs2, imm) = decode_CL_CS(inst as u16);
                    let imm_6_7 = imm & 0b11;
                    let imm_5_3 = (imm >> 2) & 0b111;
                    let imm = (imm_6_7 << 6) | (imm_5_3 << 3);
                    Some(Instruction::SD(SType {
                        rs1: map_rvc_register(rs1),
                        funct3: 0b11,
                        rs2: map_rvc_register(rs2),
                        imm,
                    }))
                }
                _ => { panic!("Unknown funct3 for compressed instruction") }
            }
        }
        _ => panic!("Unknown opcode for compressed instruction")
    }
}

pub fn decode_instruction(inst: u64) -> Instruction {
    let opcode = inst & 0b1111111;
    match opcode {
        0b0110111 => Instruction::LUI(parse_u_type(inst)),
        0b0010111 => Instruction::AUIPC(parse_u_type(inst)),
        0b1101111 => Instruction::JAL(parse_j_type(inst)),
        0b1100111 => Instruction::JALR(parse_i_type(inst)),
        0b1100011 => {
            let inst = parse_b_type(inst);
            match inst.funct3 {
                0b000 => Instruction::BEQ(inst),
                0b001 => Instruction::BNE(inst),
                0b100 => Instruction::BLT(inst),
                0b101 => Instruction::BGE(inst),
                0b110 => Instruction::BLTU(inst),
                0b111 => Instruction::BGEU(inst),
                _ => panic!("Unknown funct3 for B-type instruction"),
            }
        }
        0b0000011 => {
            let inst = parse_i_type(inst);
            match inst.funct3 {
                0b000 => Instruction::LB(inst),
                0b001 => Instruction::LH(inst),
                0b010 => Instruction::LW(inst),
                0b011 => Instruction::LD(inst),
                0b100 => Instruction::LBU(inst),
                0b101 => Instruction::LHU(inst),
                0b110 => Instruction::LWU(inst),
                _ => panic!("Unknown funct3 for I-type instruction"),
            }
        }
        0b0100011 => {
            let inst = parse_s_type(inst);
            match inst.funct3 {
                0b000 => Instruction::SB(inst),
                0b001 => Instruction::SH(inst),
                0b010 => Instruction::SW(inst),
                0b011 => Instruction::SD(inst),
                _ => panic!("Unknown funct3 for S-type instruction"),
            }
        }
        0b0010011 => {
            let inst = parse_i_type(inst);
            match inst.funct3 {
                0b000 => Instruction::ADDI(inst),
                0b010 => Instruction::SLTI(inst),
                0b011 => Instruction::SLTIU(inst),
                0b100 => Instruction::XORI(inst),
                0b101 => {
                    if (inst.imm >> 5) & 0b1111111 == 0 {
                        Instruction::SRLI(inst)
                    } else {
                        Instruction::SRAI(inst)
                    }
                }
                0b110 => Instruction::ORI(inst),
                0b001 => Instruction::SLLI(inst),
                0b111 => Instruction::ANDI(inst),
                _ => panic!("Unknown funct3 for I-type instruction"),
            }
        }
        0b0110011 => {
            let inst = parse_r_type(inst);
            match inst.funct3 {
                0b000 => {
                    if inst.funct7 == 0 {
                        Instruction::ADD(inst)
                    } else {
                        Instruction::SUB(inst)
                    }
                }
                0b001 => Instruction::SLL(inst),
                0b010 => Instruction::SLT(inst),
                0b011 => Instruction::SLTU(inst),
                0b100 => Instruction::XOR(inst),
                0b101 => {
                    if inst.funct7 == 0 {
                        Instruction::SRL(inst)
                    } else {
                        Instruction::SRA(inst)
                    }
                }
                0b110 => Instruction::OR(inst),
                0b111 => Instruction::AND(inst),
                _ => panic!("Unknown funct3 for R-type instruction"),
            }
        }
        0b0001111 => Instruction::FENCE,
        0b1110011 => {
            // TODO everything is ecall now
            if (inst >> 20) & 0xfff == 0 {
                Instruction::ECALL
            } else {
                let i_type = parse_i_type(inst);
                match i_type.funct3 {
                    0 => Instruction::EBREAK,
                    1 => Instruction::CSRRW(i_type),
                    2 => Instruction::CSRRS(i_type),
                    3 => Instruction::CSRRC(i_type),
                    5 => Instruction::CSRRWI(i_type),
                    6 => Instruction::CSRRSI(i_type),
                    7 => Instruction::CSRRCI(i_type),
                    _ => panic!("Unknown funct3 for I-type instruction"),
                }
            }
        }
        0b0011011 => {
            let inst = parse_i_type(inst);
            match inst.funct3 {
                0b000 => Instruction::ADDIW(inst),
                0b001 => Instruction::SLLIW(inst),
                0b101 => {
                    if (inst.imm >> 5) & 0b1111111 == 0 {
                        Instruction::SRLIW(inst)
                    } else {
                        Instruction::SRAIW(inst)
                    }
                }
                _ => panic!("Unknown funct3 for I-type instruction"),
            }
        }
        0b0111011 => {
            let inst = parse_r_type(inst);
            match inst.funct3 {
                0b000 => {
                    if inst.funct7 == 0 {
                        Instruction::ADDW(inst)
                    } else {
                        Instruction::SUBW(inst)
                    }
                }
                0b001 => Instruction::SLLW(inst),
                0b101 => {
                    if inst.funct7 == 0 {
                        Instruction::SRLW(inst)
                    } else {
                        Instruction::SRAW(inst)
                    }
                }
                _ => panic!("Unknown funct3 for R-type instruction"),
            }
        }
        _ => panic!("Unknown opcode: {:b}", opcode),
    }
}

pub fn execute_instruction(inst: Instruction, emulator: &mut Emulator) {
    let cpu = &mut emulator.cpu;
    let memory = &mut emulator.memory;
    match inst {
        Instruction::AUIPC(u_type) => {
            cpu.registers[u_type.rd as usize] = cpu.pc + u_type.imm as i64;
        }
        Instruction::LUI(u_type) => {
            cpu.registers[u_type.rd as usize] = u_type.imm as i64;
        }
        Instruction::ADDI(i_type) => {
            cpu.registers[i_type.rd as usize] = cpu.registers[i_type.rs1 as usize] + i_type.imm as i64;
        }
        Instruction::ADDIW(i_type) => {
            cpu.registers[i_type.rd as usize] = crop_to_32_and_sign_extend(cpu.registers[i_type.rs1 as usize] + i_type.imm as i64);
        }
        Instruction::ADD(r_type) => {
            cpu.registers[r_type.rd as usize] = cpu.registers[r_type.rs1 as usize] + cpu.registers[r_type.rs2 as usize];
        }
        Instruction::ADDW(r_type) => {
            cpu.registers[r_type.rd as usize] = crop_to_32_and_sign_extend(cpu.registers[r_type.rs1 as usize] + cpu.registers[r_type.rs2 as usize]);
        }
        Instruction::SUB(r_type) => {
            cpu.registers[r_type.rd as usize] = cpu.registers[r_type.rs1 as usize] - cpu.registers[r_type.rs2 as usize];
        }
        Instruction::SUBW(r_type) => {
            cpu.registers[r_type.rd as usize] = crop_to_32_and_sign_extend(cpu.registers[r_type.rs1 as usize] - cpu.registers[r_type.rs2 as usize]);
        }
        Instruction::SLLI(i_type) => {
            cpu.registers[i_type.rd as usize] = cpu.registers[i_type.rs1 as usize] << (i_type.imm & 0b111111);
        }
        Instruction::SLLIW(i_type) => {
            cpu.registers[i_type.rd as usize] = crop_to_32_and_sign_extend(cpu.registers[i_type.rs1 as usize] << (i_type.imm & 0b111111));
        }
        Instruction::SLL(r_type) => {
            cpu.registers[r_type.rd as usize] = cpu.registers[r_type.rs1 as usize] << (cpu.registers[r_type.rs2 as usize] & 0b111111);
        }
        Instruction::SLLW(r_type) => {
            cpu.registers[r_type.rd as usize] = crop_to_32_and_sign_extend(cpu.registers[r_type.rs1 as usize] << (cpu.registers[r_type.rs2 as usize] & 0b111111));
        }
        Instruction::SRLI(i_type) => {
            cpu.registers[i_type.rd as usize] = (cpu.registers[i_type.rs1 as usize] as u64 >> (i_type.imm & 0b111111)) as i64;
        }
        Instruction::SRAI(i_type) => {
            cpu.registers[i_type.rd as usize] = cpu.registers[i_type.rs1 as usize] >> (i_type.imm & 0b111111);
        }
        Instruction::SRL(r_type) => {
            cpu.registers[r_type.rd as usize] = (cpu.registers[r_type.rs1 as usize] as u64 >> (cpu.registers[r_type.rs2 as usize] & 0b111111)) as i64;
        }
        Instruction::SRLW(r_type) => {
            cpu.registers[r_type.rd as usize] = crop_to_32_and_sign_extend((cpu.registers[r_type.rs1 as usize] as u64 >> (cpu.registers[r_type.rs2 as usize] & 0b111111)) as i64);
        }
        Instruction::SRA(r_type) => {
            cpu.registers[r_type.rd as usize] = cpu.registers[r_type.rs1 as usize] >> (cpu.registers[r_type.rs2 as usize] & 0b111111);
        }
        Instruction::SRAW(r_type) => {
            cpu.registers[r_type.rd as usize] = crop_to_32_and_sign_extend(cpu.registers[r_type.rs1 as usize] >> (cpu.registers[r_type.rs2 as usize] & 0b111111));
        }
        Instruction::SLTI(i_type) => {
            cpu.registers[i_type.rd as usize] = if cpu.registers[i_type.rs1 as usize] < i_type.imm as i64 { 1 } else { 0 };
        }
        Instruction::SLTIU(i_type) => {
            cpu.registers[i_type.rd as usize] = if (cpu.registers[i_type.rs1 as usize] as u64) < i_type.imm as u64 { 1 } else { 0 };
        }
        Instruction::XORI(i_type) => {
            cpu.registers[i_type.rd as usize] = cpu.registers[i_type.rs1 as usize] ^ (i_type.imm as i64);
        }
        Instruction::ORI(i_type) => {
            cpu.registers[i_type.rd as usize] = cpu.registers[i_type.rs1 as usize] | (i_type.imm as i64);
        }
        Instruction::ANDI(i_type) => {
            cpu.registers[i_type.rd as usize] = cpu.registers[i_type.rs1 as usize] & (i_type.imm as i64);
        }
        Instruction::SLT(r_type) => {
            cpu.registers[r_type.rd as usize] = if cpu.registers[r_type.rs1 as usize] < cpu.registers[r_type.rs2 as usize] { 1 } else { 0 };
        }
        Instruction::SLTU(r_type) => {
            cpu.registers[r_type.rd as usize] = if (cpu.registers[r_type.rs1 as usize] as u64) < cpu.registers[r_type.rs2 as usize] as u64 { 1 } else { 0 };
        }
        Instruction::XOR(r_type) => {
            cpu.registers[r_type.rd as usize] = cpu.registers[r_type.rs1 as usize] ^ cpu.registers[r_type.rs2 as usize];
        }
        Instruction::OR(r_type) => {
            cpu.registers[r_type.rd as usize] = cpu.registers[r_type.rs1 as usize] | cpu.registers[r_type.rs2 as usize];
        }
        Instruction::AND(r_type) => {
            cpu.registers[r_type.rd as usize] = cpu.registers[r_type.rs1 as usize] & cpu.registers[r_type.rs2 as usize];
        }
        Instruction::FENCE => {}
        Instruction::FENCE_I => {}
        Instruction::ECALL => {
            let syscall_number = cpu.registers[17];
            let args = [cpu.registers[10], cpu.registers[11], cpu.registers[12], cpu.registers[13], cpu.registers[14], cpu.registers[15]];
            do_syscall(emulator, syscall_number, args);
        }
        Instruction::EBREAK => {
            panic!("EBREAK");
        }
        Instruction::JAL(j_type) => {
            cpu.registers[j_type.rd as usize] = cpu.pc + 4;
            cpu.pc += j_type.imm as i64;
        }
        Instruction::JALR(i_type) => {
            cpu.registers[i_type.rd as usize] = cpu.pc + 4;
            cpu.pc = (cpu.registers[i_type.rs1 as usize] + i_type.imm as i64) & !1;
        }
        Instruction::BEQ(b_type) => {
            if cpu.registers[b_type.rs1 as usize] == cpu.registers[b_type.rs2 as usize] {
                cpu.pc += b_type.imm as i64;
            }
        }
        Instruction::BNE(b_type) => {
            if cpu.registers[b_type.rs1 as usize] != cpu.registers[b_type.rs2 as usize] {
                cpu.pc += b_type.imm as i64;
            }
        }
        Instruction::BLT(b_type) => {
            if cpu.registers[b_type.rs1 as usize] < cpu.registers[b_type.rs2 as usize] {
                cpu.pc += b_type.imm as i64;
            }
        }
        Instruction::BGE(b_type) => {
            if cpu.registers[b_type.rs1 as usize] >= cpu.registers[b_type.rs2 as usize] {
                cpu.pc += b_type.imm as i64;
            }
        }
        Instruction::BLTU(b_type) => {
            if (cpu.registers[b_type.rs1 as usize] as u64) < cpu.registers[b_type.rs2 as usize] as u64 {
                cpu.pc += b_type.imm as i64;
            }
        }
        Instruction::BGEU(b_type) => {
            if (cpu.registers[b_type.rs1 as usize] as u64) >= cpu.registers[b_type.rs2 as usize] as u64 {
                cpu.pc += b_type.imm as i64;
            }
        }

        Instruction::LB(i_type) => {
            let address = (cpu.registers[i_type.rs1 as usize] + i_type.imm as i64);
            let data = memory.read(address.try_into().unwrap(), 1);
            cpu.registers[i_type.rd as usize] = sign_extend(data[0] as u64, 8) as i64;
        }
        Instruction::LH(i_type) => {
            let address = (cpu.registers[i_type.rs1 as usize] + i_type.imm as i64);
            let data = memory.read(address.try_into().unwrap(), 2);
            cpu.registers[i_type.rd as usize] = sign_extend(u16::from_le_bytes([data[0], data[1]]) as u64, 16);
        }
        Instruction::LW(i_type) => {
            let address = (cpu.registers[i_type.rs1 as usize] + i_type.imm as i64);
            let data = memory.read(address.try_into().unwrap(), 4);
            cpu.registers[i_type.rd as usize] = sign_extend(u32::from_le_bytes([data[0], data[1], data[2], data[3]]) as u64, 32) as i64;
        }
        Instruction::LD(i_type) => {
            let address = (cpu.registers[i_type.rs1 as usize] + i_type.imm as i64);
            let data = memory.read(address.try_into().unwrap(), 8);
            cpu.registers[i_type.rd as usize] = u64::from_le_bytes([data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]]) as i64;
        }
        Instruction::LBU(i_type) => {
            let address = (cpu.registers[i_type.rs1 as usize] + i_type.imm as i64);
            let data = memory.read(address.try_into().unwrap(), 1);
            cpu.registers[i_type.rd as usize] = u64::from(data[0]) as i64;
        }
        Instruction::LHU(i_type) => {
            let address = (cpu.registers[i_type.rs1 as usize] + i_type.imm as i64);
            let data = memory.read(address.try_into().unwrap(), 2);
            cpu.registers[i_type.rd as usize] = u16::from_le_bytes([data[0], data[1]]) as i64;
        }
        Instruction::LWU(i_type) => {
            let address = (cpu.registers[i_type.rs1 as usize] + i_type.imm as i64);
            let data = memory.read(address.try_into().unwrap(), 4);
            cpu.registers[i_type.rd as usize] = u32::from_le_bytes([data[0], data[1], data[2], data[3]]) as i64;
        }

        Instruction::SB(s_type) => {
            let address = (cpu.registers[s_type.rs1 as usize] + s_type.imm as i64);
            let data = cpu.registers[s_type.rs2 as usize] as u8;
            memory.write(address.try_into().unwrap(), &[data]);
        }
        Instruction::SH(s_type) => {
            let address = (cpu.registers[s_type.rs1 as usize] + s_type.imm as i64);
            let data = cpu.registers[s_type.rs2 as usize] as u16;
            memory.write(address.try_into().unwrap(), &data.to_le_bytes());
        }
        Instruction::SW(s_type) => {
            let address = (cpu.registers[s_type.rs1 as usize] + s_type.imm as i64);
            let data = cpu.registers[s_type.rs2 as usize] as u32;
            memory.write(address.try_into().unwrap(), &data.to_le_bytes());
        }
        Instruction::SD(s_type) => {
            let address = (cpu.registers[s_type.rs1 as usize] + s_type.imm as i64);
            let data = cpu.registers[s_type.rs2 as usize] as u64;
            memory.write(address as usize, &data.to_le_bytes());
        }

        Instruction::SRLIW(i_type) => {
            cpu.registers[i_type.rd as usize] = crop_to_32_and_sign_extend(((cpu.registers[i_type.rs1 as usize] as u64) >> (i_type.imm & 0b111111)) as i64);
        }
        Instruction::SRAIW(i_type) => {
            cpu.registers[i_type.rd as usize] = crop_to_32_and_sign_extend((cpu.registers[i_type.rs1 as usize] >> (i_type.imm & 0b111111)) as i64);
        }

        // weird Instructions
        Instruction::CSRRW(i_type) => {
            let address = i_type.imm as usize;
            let old_value = cpu.csr[address];
            cpu.csr[address] = cpu.registers[i_type.rs1 as usize];
            cpu.registers[i_type.rd as usize] = old_value;
        }
        Instruction::CSRRS(i_type) => {
            let address = i_type.imm as usize;
            let old_value = cpu.csr[address];
            cpu.csr[address] = cpu.csr[address] | cpu.registers[i_type.rs1 as usize];
            cpu.registers[i_type.rd as usize] = old_value;
        }
        Instruction::CSRRC(i_type) => {
            let address = i_type.imm as usize;
            let old_value = cpu.csr[address];
            cpu.csr[address] = cpu.csr[address] & !cpu.registers[i_type.rs1 as usize];
            cpu.registers[i_type.rd as usize] = old_value;
        }
        Instruction::CSRRWI(i_type) => {
            let address = i_type.imm as usize;
            let old_value = cpu.csr[address];
            cpu.csr[address] = i_type.rs1 as i64;
            cpu.registers[i_type.rd as usize] = old_value;
        }
        Instruction::CSRRSI(i_type) => {
            let address = i_type.imm as usize;
            let old_value = cpu.csr[address];
            cpu.csr[address] = cpu.csr[address] | i_type.rs1 as i64;
            cpu.registers[i_type.rd as usize] = old_value;
        }
        Instruction::CSRRCI(i_type) => {
            let address = i_type.imm as usize;
            let old_value = cpu.csr[address];
            cpu.csr[address] = cpu.csr[address] & !(i_type.rs1 as i64);
            cpu.registers[i_type.rd as usize] = old_value;
        }
    }
}
