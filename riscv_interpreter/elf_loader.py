
class ELF_File:
    Instruction_Set = {
        0x00: "None",
        0x02: "SPARC",
        0x03: "x86",
        0x08: "MIPS",
        0x14: "PowerPC",
        0x28: "ARM",
        0x2A: "SuperH",
        0x32: "IA-64",
        0x3E: "x86-64",
        0xB7: "AArch64",
        0xF3: "RISC-V"
    }
    def __init__(self, filename):
        # https://wiki.osdev.org/ELF
        self.filename = filename
        self.arch = None # 32 or 64 bit
        self.endian = None # little or big endian
        self.type = None # 1 if relocatable, 2 if executable, 3 if shared object, 4 if core image
        self.instruction_set = None # See Instruction_Set
        self.entry_pos = None
        self.program_header_pos = None
        self.section_header_pos = None
        self.header_size = None
        self.program_header_entry_size = None
        self.program_header_entry_count = None
        self.section_header_entry_size = None
        self.section_header_entry_count = None
        self.section_header_names_index = None

        self.program_headers = []
        self.section_headers = []

    def get_program_data(self):
        # has to be called after load()
        if self.program_header_entry_count is None:
            raise Exception("ELF file not loaded yet")


        to_load = [] # (start_address, size, virtual_address)
        for i in range(self.program_header_entry_count):
            # all with type 1 have to be loaded
            if self.program_headers[i].type == 1:
                to_load.append((self.program_headers[i].offset, self.program_headers[i].p_filesz, self.program_headers[i].p_vaddr))

        return to_load

    def __repr__(self):
        # print all attributes
        return f"""ELF_File(
    filename={self.filename},
    arch={self.arch},
    endian={self.endian},
    type={self.type},
    instruction_set={self.instruction_set},
    entry_pos={self.entry_pos} | 0x{self.entry_pos:08X},
    program_header_pos={self.program_header_pos},
    section_header_pos={self.section_header_pos},
    header_size={self.header_size},
    program_header_entry_size={self.program_header_entry_size},
    program_header_entry_count={self.program_header_entry_count},
    section_header_entry_size={self.section_header_entry_size},
    section_header_entry_count={self.section_header_entry_count},
    section_header_names_index={self.section_header_names_index}
    
    program_headers={self.program_headers}
    
    section_headers={self.section_headers}
)"""


    def load(self):
        with open(self.filename, "rb") as f:
            f = f.read()
            # Check magic number
            if f[0:4] != b"\x7FELF":
                raise Exception("Invalid ELF file")

            # Check if 32-bit or 64-bit
            if f[4] == 1:
                self.arch = 32
                raise NotImplementedError("32-bit ELF files are not supported yet")
            elif f[4] == 2:
                self.arch = 64
            else:
                raise Exception("Invalid ELF file")

            # Check if little-endian or big-endian
            if f[5] == 1:
                self.endian = "little"
            elif f[5] == 2:
                self.endian = "big"
            else:
                raise Exception("Invalid ELF file")

            # Check ELF version
            # skip

            # Check OS ABI
            # skip

            # check type
            self.type = int.from_bytes(f[16:18], byteorder="little")
            if self.type != 2:
                raise Exception("Invalid ELF file")

            # check instruction set
            self.instruction_set = int.from_bytes(f[18:20], byteorder="little")
            if self.instruction_set not in self.Instruction_Set:
                raise Exception("Invalid ELF file")
            self.instruction_set = self.Instruction_Set[self.instruction_set]

            # check entry point
            self.entry_pos = int.from_bytes(f[24:32], byteorder="little")

            # check program header
            self.program_header_pos = int.from_bytes(f[32:40], byteorder="little")

            # check section header
            self.section_header_pos = int.from_bytes(f[40:48], byteorder="little")

            # check header size
            self.header_size = int.from_bytes(f[52:54], byteorder="little")

            # check program header entry size
            self.program_header_entry_size = int.from_bytes(f[54:56], byteorder="little")

            # check program header entry count
            self.program_header_entry_count = int.from_bytes(f[56:58], byteorder="little")

            # check section header entry size
            self.section_header_entry_size = int.from_bytes(f[58:60], byteorder="little")

            # check section header entry count
            self.section_header_entry_count = int.from_bytes(f[60:62], byteorder="little")

            # check section header names index
            self.section_header_names_index = int.from_bytes(f[62:64], byteorder="little")

            # load program headers
            for i in range(self.program_header_entry_count):
                self.program_headers.append(Program_Header().load(f[self.program_header_pos + i * self.program_header_entry_size: self.program_header_pos + (i + 1) * self.program_header_entry_size]))

            # load section headers
            for i in range(self.section_header_entry_count):
                self.section_headers.append(Section_Header().load(f[self.section_header_pos + i * self.section_header_entry_size: self.section_header_pos + (i + 1) * self.section_header_entry_size]))

            return self


class Program_Header:
    def __init__(self):
        self.type = None
        self.flags = None
        self.offset = None
        self.p_paddr = None
        self.p_vaddr = None
        self.p_filesz = None
        self.p_memsz = None
        self.align = None

    def __repr__(self):
        return f"""\nProgram_Header(
    type={self.type} | 0x{self.type:08X},
    flags={self.flags} | 0x{self.flags:08X},
    offset={self.offset} | 0x{self.offset:08X},
    p_vaddr={self.p_vaddr}  | 0x{self.p_vaddr:08X},
    p_filesz={self.p_filesz} | 0x{self.p_filesz:08X},
    p_memsz={self.p_memsz} | 0x{self.p_memsz:08X},
    align={self.align} | 0x{self.align:08X}
)"""

    def load(self, f):
        self.type = int.from_bytes(f[0:4], byteorder="little")
        self.flags = int.from_bytes(f[4:8], byteorder="little")
        self.offset = int.from_bytes(f[8:16], byteorder="little")
        self.p_vaddr = int.from_bytes(f[16:24], byteorder="little")
        self.p_paddr = int.from_bytes(f[24:32], byteorder="little")
        self.p_filesz = int.from_bytes(f[32:40], byteorder="little")
        self.p_memsz = int.from_bytes(f[40:48], byteorder="little")
        self.align = int.from_bytes(f[48:56], byteorder="little")

        return self

class Section_Header:
    #https://en.wikipedia.org/wiki/Executable_and_Linkable_Format
    def __init__(self):
        self.sh_name = None
        self.sh_type = None
        self.sh_flags = None
        self.sh_addr = None
        self.sh_offset = None
        self.sh_size = None
        self.sh_link = None
        self.sh_info = None
        self.sh_addralign = None
        self.sh_entsize = None

    def __repr__(self):
        return f"""\nSection_Header(
    sh_name={self.sh_name} | 0x{self.sh_name:08X},
    sh_type={self.sh_type} | 0x{self.sh_type:08X},
    sh_flags={self.sh_flags} | 0x{self.sh_flags:08X},
    sh_addr={self.sh_addr} | 0x{self.sh_addr:08X},
    sh_offset={self.sh_offset} | 0x{self.sh_offset:08X},
    sh_size={self.sh_size} | 0x{self.sh_size:08X},
    sh_link={self.sh_link} | 0x{self.sh_link:08X},
    sh_info={self.sh_info} | 0x{self.sh_info:08X},
    sh_addralign={self.sh_addralign} | 0x{self.sh_addralign:08X},
    sh_entsize={self.sh_entsize} | 0x{self.sh_entsize:08X}
)"""

    def load(self, f):
        self.sh_name = int.from_bytes(f[0:4], byteorder="little")
        self.sh_type = int.from_bytes(f[4:8], byteorder="little")
        self.sh_flags = int.from_bytes(f[8:16], byteorder="little")
        self.sh_addr = int.from_bytes(f[16:24], byteorder="little")
        self.sh_offset = int.from_bytes(f[24:32], byteorder="little")
        self.sh_size = int.from_bytes(f[32:40], byteorder="little")
        self.sh_link = int.from_bytes(f[40:44], byteorder="little")
        self.sh_info = int.from_bytes(f[44:48], byteorder="little")
        self.sh_addralign = int.from_bytes(f[48:56], byteorder="little")
        self.sh_entsize = int.from_bytes(f[56:64], byteorder="little")

        return self



if __name__ == '__main__':
    f = ELF_File("test").load()
    print(f)