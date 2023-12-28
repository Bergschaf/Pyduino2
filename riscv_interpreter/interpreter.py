def int_from_bin(a: int, word_size=64):
    # twos complement
    if a & (1 << (word_size - 1)):
        a = a - (1 << word_size)
    return a


def int_to_bin(a: int, word_size=64):
    # twos complement
    if a < 0:
        a = (1 << word_size) + a
    return a


def sign_extend(a: int, word_size=64):
    if a & (1 << (word_size - 1)):
        a = a - (1 << word_size)
    return a


class Registers:
    def __init__(self):
        self.registers = [0] * 32
        self.pc = 0

    def __getitem__(self, item):
        return self.registers[item]

    def __setitem__(self, key, value):
        if key == 0:
            return
        self.registers[key] = value

    def __repr__(self):
        return "\n".join(str(x) + ": " + str(hex(self[x])) for x in range(32) if self[x])


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
        return ("\n".join(x + ":" + str(bin(self.__dict__[x])) for x in vars if self.__dict__[x] is not None) +
                f"\nimm: {self.imm}") if self.imm is not None else ""


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


class Instruction:
    def __init__(self, values: InstructionType, func):
        self.values = values
        self.func = func

    def do(self, registers):
        self.func(self.values, registers)

    def __repr__(self):
        return str(self.values) + "\n" + self.func.__name__


class Instructions:
    @staticmethod
    def addi(inst: IType, registers: Registers):
        registers[inst.rd] = registers[inst.rs1] + sign_extend(inst.imm, 64)

    @staticmethod
    def slli(inst: IType, registers: Registers):
        registers[inst.rd] = registers[inst.rs1] << inst.imm

    @staticmethod
    def srli(inst: IType, registers: Registers):
        registers[inst.rd] = registers[inst.rs1] >> inst.imm

    @staticmethod
    def srai(inst: IType, registers: Registers):
        registers[inst.rd] = sign_extend(registers[inst.rs1] >> inst.imm, 64)

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
        pass

    @staticmethod
    def xxx(inst: IType, registers: Registers):
        pass

    @staticmethod
    def xxx(inst: IType, registers: Registers):
        pass

    @staticmethod
    def xxx(inst: IType, registers: Registers):
        pass

    @staticmethod
    def xxx(inst: IType, registers: Registers):
        pass

    @staticmethod
    def xxx(inst: IType, registers: Registers):
        pass

    @staticmethod
    def xxx(inst: IType, registers: Registers):
        pass

    @staticmethod
    def match(inst):
        opcode = inst & 0b111111
        print(bin(opcode))
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
                inst = SType(inst).decode()
                match inst.funct3:
                    case 0b000:
                        return Instruction(inst, Instructions.add)

            case 0b0110111:
                return Instruction(UType(inst).decode(), Instructions.lui)

            case 0b0010111:
                return Instruction(UType(inst).decode(), Instructions.auipc)

            case _:
                raise Exception("not gud")


#   ff650493  | 111111110110 01010 000 010010 010011        	add	s1,a0,10
# print(bin(0xff650493))
instruction_list = [0x00b50593, 0x00359593, 0x4d25f593,0x3944d537]
registers = Registers()
instructions = []
for i in instruction_list:
    instructions.append(Instructions.match(i))
print(instructions)
for i in instructions:
    i.do(registers)
print(registers)
