use crate::memory::Memory;
use std::fs;

pub struct ElfFile {
    name: String,
    data: Vec<u8>,
    pub entry_point: usize,
    program_header_offset: usize,
    program_header_count: u16,
    program_headers: Vec<ProgramHeader>,
    // ignore the rest
}

#[derive(Debug)]
struct ProgramHeader {
    p_type: u32,
    p_offset: usize,
    p_vaddr: usize,
    p_filesz: usize,
    p_memsz: usize,
    p_flags: u32,
    p_align: usize,
}

pub fn load_elf_file(name: &str) -> ElfFile {
    let data = fs::read(name).unwrap();
    let mut elf = ElfFile {
        name: name.to_string(),
        data,
        entry_point: 0,
        program_header_offset: 0,
        program_header_count: 0,
        program_headers: Vec::new(),
    };

    // parse the ELF header

    // check the architecture
    if (elf.data[4] != 2) || (elf.data[5] != 1) {
        panic!("Unsupported architecture");
    };

    // check instruction set
    if elf.data[18] != 0xF3 {
        panic!("Unsupported instruction set");
    }

    // Entry point
    elf.entry_point = u64::from_le_bytes([elf.data[24], elf.data[25], elf.data[26], elf.data[27], elf.data[28], elf.data[29], elf.data[30], elf.data[31]]) as usize;
    // print the entry point as hex

    // Program header offset
    elf.program_header_offset = u64::from_le_bytes([elf.data[32], elf.data[33], elf.data[34], elf.data[35], elf.data[36], elf.data[37], elf.data[38], elf.data[39]]) as usize;
    // print the program header offset as hex

    // Program header count
    elf.program_header_count = u16::from_le_bytes([elf.data[56], elf.data[57]]) as u16;
    // print the program header count as hex

    // parse the program headers
    for i in 0..elf.program_header_count {
        let offset = elf.program_header_offset + (i as usize * 56);
        let p_type = u32::from_le_bytes([elf.data[offset], elf.data[offset + 1], elf.data[offset + 2], elf.data[offset + 3]]);
        let p_flags = u32::from_le_bytes([elf.data[offset + 4], elf.data[offset + 5], elf.data[offset + 6], elf.data[offset + 7]]);
        let p_offset = u64::from_le_bytes([elf.data[offset + 8], elf.data[offset + 9], elf.data[offset + 10], elf.data[offset + 11], elf.data[offset + 12], elf.data[offset + 13], elf.data[offset + 14], elf.data[offset + 15]]) as usize;
        let p_vaddr = u64::from_le_bytes([elf.data[offset + 16], elf.data[offset + 17], elf.data[offset + 18], elf.data[offset + 19], elf.data[offset + 20], elf.data[offset + 21], elf.data[offset + 22], elf.data[offset + 23]]) as usize;
        let p_filesz = u64::from_le_bytes([elf.data[offset + 32], elf.data[offset + 33], elf.data[offset + 34], elf.data[offset + 35], elf.data[offset + 36], elf.data[offset + 37], elf.data[offset + 38], elf.data[offset + 39]]) as usize;
        let p_memsz = u64::from_le_bytes([elf.data[offset + 40], elf.data[offset + 41], elf.data[offset + 42], elf.data[offset + 43], elf.data[offset + 44], elf.data[offset + 45], elf.data[offset + 46], elf.data[offset + 47]]) as usize;
        let p_align = u64::from_le_bytes([elf.data[offset + 48], elf.data[offset + 49], elf.data[offset + 50], elf.data[offset + 51], elf.data[offset + 52], elf.data[offset + 53], elf.data[offset + 54], elf.data[offset + 55]]) as usize;

        elf.program_headers.push(ProgramHeader {
            p_type,
            p_offset,
            p_vaddr,
            p_filesz,
            p_memsz,
            p_flags,
            p_align,
        });
    }

    elf
}

impl ElfFile {
    pub fn load_into_memory(&self, memory: &mut Memory){
        for i in 0..self.program_header_count {
            let header = &self.program_headers[i as usize];
            if header.p_type == 1 {
                memory.write(header.p_vaddr, &self.data[header.p_offset..header.p_offset + header.p_filesz]);
            }
        }
    }
}