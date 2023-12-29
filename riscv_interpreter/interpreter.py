XLEN = 64


def int_from_bin(a: int, word_size=XLEN):
    # twos complement
    if a & (1 << (word_size - 1)):
        a = a - (1 << word_size)
    return a

def int_to_bin(a: int, word_size=XLEN):
    # twos complement
    if a < 0:
        a = (1 << word_size) + a
    return a


def sign_extend(a: int, word_size=XLEN):
    # twos complement
    if a & (1 << (word_size - 1)):
        a = a - (1 << word_size)
    return a


class Registers:

    def __init__(self):
        self.registers = [0] * 32
        self.pc = 0
        self.memory = None

    def __getitem__(self, item):
        return self.registers[item]

    def __setitem__(self, key, value):
        if key == 0:
            return
        value = value & pow(2, XLEN) - 1  # TODO Change to 64 bits
        self.registers[key] = value

    def __repr__(self):
        return "\n".join(str(x) + ": " + str(self[x]) for x in range(32) if self[x])


class InstructionType:
    def __init__(self, instruction: int):
        self.inst = instruction
        self.opcode = instruction & 0b111111
        self.rd = None
        self.rs1 = None
        self.rs2 = None
        self.imm = None
        self.imm_left = None  # The immediate of the S and B type (in the place of the rd value)
        self.funct3 = None
        self.funct7 = None

    def __repr__(self):
        vars = ("opcode", "rd", "rs1", "rs2", "funct3", "funct7")
        return "\n".join(
            x + ":" + str(self.__dict__[x]) for x in vars if self.__dict__[x] is not None) + "\nimm: " + str(self.imm)


class RType(InstructionType):
    def decode(self):
        self.rd = (self.inst >> 7) & 0b11111
        self.funct3 = (self.inst >> 12) & 0b111
        self.rs1 = (self.inst >> 15) & 0b11111
        self.rs2 = (self.inst >> 20) & 0b11111
        self.funct7 = self.inst >> 25
        return self


class SType(InstructionType):
    def decode(self):
        self.imm_left = (self.inst >> 7) & 0b11111
        self.funct3 = (self.inst >> 12) & 0b111
        self.rs1 = (self.inst >> 15) & 0b11111
        self.rs2 = (self.inst >> 20) & 0b11111
        self.imm = self.inst >> 25
        return self


class IType(InstructionType):
    def decode(self):
        self.rd = (self.inst >> 7) & 0b11111
        self.funct3 = (self.inst >> 12) & 0b111
        self.rs1 = (self.inst >> 15) & 0b11111
        self.imm = int_from_bin(self.inst >> 20, word_size=12)
        return self


class UType(InstructionType):
    def decode(self):
        self.rd = (self.inst >> 7) & 0b11111
        self.imm = self.inst >> 12
        return self

class JType(InstructionType):
    def decode(self):
        self.rd = (self.inst >> 7) & 0b11111
        self.imm = self.inst >> 12
        return self

class Instruction:
    def __init__(self, values: InstructionType, func):
        self.values = values
        self.func = func

    def do(self, registers):
        self.func(self.values, registers)

    def __repr__(self):
        return str(self.values) + "\n" + self.func.__name__ + "\n\n"


class Instructions:
    @staticmethod
    def addi(inst: IType, registers: Registers):
        registers[inst.rd] = registers[inst.rs1] + sign_extend(inst.imm)

    @staticmethod
    def slli(inst: IType, registers: Registers):
        registers[inst.rd] = registers[inst.rs1] << inst.imm

    @staticmethod
    def srli(inst: IType, registers: Registers):
        registers[inst.rd] = registers[inst.rs1] >> inst.imm

    @staticmethod
    def srai(inst: IType, registers: Registers):
        sham = inst.imm & 0b11111
        if registers[inst.rs1] & 2 ** (XLEN - 1) != 0:  # MSB is 1, i.e. x is negative
            filler = int('1' * sham + '0' * (XLEN - sham), 2)
            registers[inst.rd] = (registers[inst.rs1] >> sham) | filler  # fill in 0's with 1's
        else:
            registers[inst.rd] = registers[inst.rs1] >> sham

    @staticmethod
    def slti(inst: IType, registers: Registers):
        if int_from_bin(registers[inst.rs1]) < int_to_bin(sign_extend(inst.imm)):
            registers[inst.rd] = 1
        else:
            registers[inst.rd] = 0

    @staticmethod
    def sltiu(inst: IType, registers: Registers):
        if registers[inst.rs1] < sign_extend(inst.imm):
            registers[inst.rd] = 1
        else:
            registers[inst.rd] = 0

    @staticmethod
    def andi(inst: IType, registers: Registers):
        registers[inst.rd] = registers[inst.rs1] & sign_extend(inst.imm)

    @staticmethod
    def ori(inst: IType, registers: Registers):
        registers[inst.rd] = registers[inst.rs1] | sign_extend(inst.imm)

    @staticmethod
    def xori(inst: IType, registers: Registers):
        registers[inst.rd] = registers[inst.rs1] ^ sign_extend(inst.imm)

    @staticmethod
    def lui(inst: IType, registers: Registers):
        registers[inst.rd] = inst.imm << 12

    @staticmethod
    def auipc(inst: IType, registers: Registers):
        registers[inst.rd] = registers.pc + (inst.imm << 12)

    @staticmethod
    def add(inst: IType, registers: Registers):
        registers[inst.rd] = registers[inst.rs1] + registers[inst.rs2]

    @staticmethod
    def sub(inst: IType, registers: Registers):
        registers[inst.rd] = registers[inst.rs1] - registers[inst.rs2]
        # maybe other way round

    @staticmethod
    def slt(inst: IType, registers: Registers):
        if int_from_bin(registers[inst.rs1]) < int_from_bin(registers[inst.rs2]):
            registers[inst.rd] = 1
        else:
            registers[inst.rd] = 0

    @staticmethod
    def sltu(inst: IType, registers: Registers):
        if registers[inst.rs1] < registers[inst.rs2]:
            registers[inst.rd] = 1
        else:
            registers[inst.rd] = 0

    @staticmethod
    def _or(inst: IType, registers: Registers):
        # underscore weil sonst python mault
        registers[inst.rd] = registers[inst.rs1] | registers[inst.rs2]

    @staticmethod
    def _and(inst: IType, registers: Registers):
        registers[inst.rd] = registers[inst.rs1] & registers[inst.rs2]

    @staticmethod
    def xor(inst: IType, registers: Registers):
        registers[inst.rd] = registers[inst.rs1] ^ registers[inst.rs2]

    @staticmethod
    def sll(inst: IType, registers: Registers):
        registers[inst.rd] = registers[inst.rs1] << (registers[inst.rs2] & 0b11111)

    @staticmethod
    def srl(inst: IType, registers: Registers):
        registers[inst.rd] = registers[inst.rs1] >> (registers[inst.rs2] & 0b11111)

    @staticmethod
    def sra(inst: IType, registers: Registers):
        # https://stackoverflow.com/questions/64963170/how-to-do-arithmetic-right-shift-in-python-for-signed-and-unsigned-values

        sham = registers[inst.rs2] & 0b11111
        if registers[inst.rs1] & 2 ** (XLEN - 1) != 0:  # MSB is 1, i.e. x is negative
            filler = int('1' * sham + '0' * (XLEN - sham), 2)
            registers[inst.rd] = (registers[inst.rs1] >> sham) | filler  # fill in 0's with 1's
        else:
            registers[inst.rd] =  registers[inst.rs1] >> sham


    @staticmethod
    def jal(inst: IType, registers: Registers):
        inst.imm = inst.imm >> 1 << 1
        print("imm", inst.imm, hex(inst.imm))
        target_adress = inst.imm + registers.pc
        print("target", target_adress, hex(target_adress))
        registers[inst.rd] = registers.pc + 4
        registers.pc = target_adress

    @staticmethod
    def xxx(inst: IType, registers: Registers):
        pass

    @staticmethod
    def xxx(inst: IType, registers: Registers):
        pass

    @staticmethod
    def match(inst):
        opcode = inst & 0b1111111
        match opcode:
            case 0b0010011:
                inst = IType(inst).decode()
                match inst.funct3:
                    case 0:
                        return Instruction(inst, Instructions.addi)
                    case 0b001:
                        return Instruction(inst, Instructions.slli)
                    case 0b010:
                        return Instruction(inst, Instructions.slti)
                    case 0b011:
                        return Instruction(inst, Instructions.sltiu)
                    case 0b100:
                        return Instruction(inst, Instructions.xori)
                    case 0b110:
                        return Instruction(inst, Instructions.ori)
                    case 0b111:
                        return Instruction(inst, Instructions.andi)

                    case 0b101:
                        if inst.imm >> 10 == 1:
                            return Instruction(inst, Instructions.srai)
                        elif inst.imm >> 10 == 0:
                            return Instruction(inst, Instructions.srli)
                        else:
                            raise Exception("not gud")
                    case _:
                        raise Exception(f"Wrong func3 ({bin(inst.funct3)}) for opcode: ", bin(opcode))

            case 0b0110011:
                inst = RType(inst).decode()
                match inst.funct3:
                    case 0b000:
                        if inst.funct7 >> 5 == 0:
                            return Instruction(inst, Instructions.add)
                        elif inst.funct7 >> 5 == 1:
                            return Instruction(inst, Instructions.sub)
                        else:
                            raise Exception("Wrong func7")

                    case 0b001:
                        return Instruction(inst, Instructions.sll)

                    case 0b010:
                        return Instruction(inst, Instructions.slt)

                    case 0b011:
                        return Instruction(inst, Instructions.sltu)

                    case 0b100:
                        return Instruction(inst, Instructions.xor)

                    case 0b101:
                        if inst.funct7 >> 5 == 0:
                            return Instruction(inst, Instructions.srl)
                        elif inst.funct7 >> 5 == 1:
                            return Instruction(inst, Instructions.sra)
                        else:
                            raise Exception("Wrong func7")

                    case 0b110:
                        return Instruction(inst, Instructions._or)

                    case 0b111:
                        return Instruction(inst, Instructions._and)

            case 0b0110111:
                return Instruction(UType(inst).decode(), Instructions.lui)

            case 0b0010111:
                return Instruction(UType(inst).decode(), Instructions.auipc)
                # not tested

            case 0b1101111:
                return Instruction(JType(inst).decode(), Instructions.jal)

            case _:
                print(f"Opcode: {bin(opcode)}")
                raise Exception("not gud")

if __name__ == '__main__':

    #   ff650493  | 111111110110 01010 000 010010 010011        	add	s1,a0,10
    # print(bin(0xff650493))
    instruction_list = [0x00b50593, 0x00359593, 0x4d25f593, 0x3944d537, 0x4d350513, 0x4dd57593, 0x00a58633, 0x40c586b3,
                        0x01965613, 0x00c69733, 0x00e6a7b3, 0x00072833, 0x00e038b3, 0x00e74733, 0x00060733, 0xfec70713,
                        0x00e6d4b3, 0x40e6d433, 0x40b55393, 0x00d686b3]
    registers = Registers()
    instructions = []
    for i in instruction_list:
        instructions.append(Instructions.match(i))

    for i in instructions:
        i.do(registers)
    print(registers)

    # Test oveflow -> sollte passen
    """addi x11, x10, 11
    slli x11, x11, 3
    andi x11, x11, 1234
    lui x10, 234573
    addi x10, x10, 1235
    andi x11, x10, 1245
    add x12, x11, x10
    sub x13, x11, x12
    srli x12, x12, 25
    sll x14, x13, x12
    slt x15, x13, x14
    slt x16, x14, x0
    sltu x17, x0, x14
    xor x14, x14, x14
    add x14, x12, x0"""
