use std::fs;
use crate::memory::Memory;

pub struct pe32File {
    name: String,
    data: Vec<u8>,
    COFF_header: COFF_Header,
    pub optional_header: Optional_header,
    section_headers: Vec<Section_header>,
}

#[derive(Debug)]
struct COFF_Header {
    machine: u16,
    number_of_sections: u16,
    time_date_stamp: u32,
    pointer_to_symbol_table: u32,
    number_of_symbols: u32,
    size_of_optional_header: u16,
    characteristics: u16,
}

#[derive(Debug)]
pub struct Optional_header {
    // required for image files (we assume its pe32+)
    magic: u16,
    major_linker_version: u8,
    minor_linker_version: u8,
    size_of_code: u32,
    size_of_initialized_data: u32,
    size_of_uninitialized_data: u32,
    pub address_of_entry_point: u32,
    base_of_code: u32,
}

#[derive(Debug)]
struct Section_header {
    name: String,
    virtual_size: u32,
    virtual_address: u32,
    size_of_raw_data: u32,
    pointer_to_raw_data: u32,
    characteristics: u32,
}

pub fn load_p32_file(name: &str) -> pe32File {
    let data = fs::read(name.to_string()).unwrap();
    let mut pe32 = pe32File {
        name: name.to_string(),
        data,
        COFF_header: COFF_Header {
            machine: 0,
            number_of_sections: 0,
            time_date_stamp: 0,
            pointer_to_symbol_table: 0,
            number_of_symbols: 0,
            size_of_optional_header: 0,
            characteristics: 0,
        },
        optional_header: Optional_header {
            magic: 0,
            major_linker_version: 0,
            minor_linker_version: 0,
            size_of_code: 0,
            size_of_initialized_data: 0,
            size_of_uninitialized_data: 0,
            address_of_entry_point: 0,
            base_of_code: 0,
        },
        section_headers: Vec::new(),
    };


    // read the file offset (specified at location 0x3c)
    let file_offset = u32::from_le_bytes([pe32.data[0x3c], pe32.data[0x3d], pe32.data[0x3e], pe32.data[0x3f]]) as usize;
    print!("File offset: {:x?}\n", file_offset);


    // read the signature (at the file offset)
    let signature = String::from_utf8(pe32.data[file_offset..file_offset + 4].to_vec()).unwrap();
    print!("Signature: {:x?}\n", signature);
    // check the signature
    if signature != "PE\0\0" {
        panic!("Invalid signature");
    }

    // parse the COFF header(starts after the signature)
    pe32.COFF_header.machine = u16::from_le_bytes([pe32.data[file_offset + 4], pe32.data[file_offset + 5]]) as u16;
    pe32.COFF_header.number_of_sections = u16::from_le_bytes([pe32.data[file_offset + 6], pe32.data[file_offset + 7]]) as u16;
    pe32.COFF_header.time_date_stamp = u32::from_le_bytes([pe32.data[file_offset + 8], pe32.data[file_offset + 9], pe32.data[file_offset + 10], pe32.data[file_offset + 11]]) as u32;
    pe32.COFF_header.pointer_to_symbol_table = u32::from_le_bytes([pe32.data[file_offset + 12], pe32.data[file_offset + 13], pe32.data[file_offset + 14], pe32.data[file_offset + 15]]) as u32;
    pe32.COFF_header.number_of_symbols = u32::from_le_bytes([pe32.data[file_offset + 16], pe32.data[file_offset + 17], pe32.data[file_offset + 18], pe32.data[file_offset + 19]]) as u32;
    pe32.COFF_header.size_of_optional_header = u16::from_le_bytes([pe32.data[file_offset + 20], pe32.data[file_offset + 21]]) as u16;
    pe32.COFF_header.characteristics = u16::from_le_bytes([pe32.data[file_offset + 22], pe32.data[file_offset + 23]]) as u16;
    print!("COFF header: {:x?}\n", pe32.COFF_header);

    // parse the optional header
    let magic_number = u16::from_le_bytes([pe32.data[file_offset + 24], pe32.data[file_offset + 25]]) as u16;
    // check if the magic number valid -> 0x20b for pe32+ (64-bit)
    if magic_number != 0x20b {
        panic!("Invalid magic number");
    }
    pe32.optional_header.magic = magic_number;
    pe32.optional_header.major_linker_version = pe32.data[file_offset + 26];
    pe32.optional_header.minor_linker_version = pe32.data[file_offset + 27];
    pe32.optional_header.size_of_code = u32::from_le_bytes([pe32.data[file_offset + 28], pe32.data[file_offset + 29], pe32.data[file_offset + 30], pe32.data[file_offset + 31]]) as u32;
    pe32.optional_header.size_of_initialized_data = u32::from_le_bytes([pe32.data[file_offset + 32], pe32.data[file_offset + 33], pe32.data[file_offset + 34], pe32.data[file_offset + 35]]) as u32;
    pe32.optional_header.size_of_uninitialized_data = u32::from_le_bytes([pe32.data[file_offset + 36], pe32.data[file_offset + 37], pe32.data[file_offset + 38], pe32.data[file_offset + 39]]) as u32;
    pe32.optional_header.address_of_entry_point = u32::from_le_bytes([pe32.data[file_offset + 40], pe32.data[file_offset + 41], pe32.data[file_offset + 42], pe32.data[file_offset + 43]]) as u32;
    pe32.optional_header.base_of_code = u32::from_le_bytes([pe32.data[file_offset + 44], pe32.data[file_offset + 45], pe32.data[file_offset + 46], pe32.data[file_offset + 47]]) as u32;
    print!("Optional header: {:x?}\n", pe32.optional_header);

    // parse the section headers (directly after the optional header)
    let section_header_offset = file_offset + 24 + pe32.COFF_header.size_of_optional_header as usize;
    for i in 0..pe32.COFF_header.number_of_sections {
        let offset = section_header_offset + (i as usize * 40);
        let name = String::from_utf8(pe32.data[offset..offset + 8].to_vec()).unwrap();
        let virtual_size = u32::from_le_bytes([pe32.data[offset + 8], pe32.data[offset + 9], pe32.data[offset + 10], pe32.data[offset + 11]]) as u32;
        let virtual_address = u32::from_le_bytes([pe32.data[offset + 12], pe32.data[offset + 13], pe32.data[offset + 14], pe32.data[offset + 15]]) as u32;
        let size_of_raw_data = u32::from_le_bytes([pe32.data[offset + 16], pe32.data[offset + 17], pe32.data[offset + 18], pe32.data[offset + 19]]) as u32;
        let pointer_to_raw_data = u32::from_le_bytes([pe32.data[offset + 20], pe32.data[offset + 21], pe32.data[offset + 22], pe32.data[offset + 23]]) as u32;
        let characteristics = u32::from_le_bytes([pe32.data[offset + 24], pe32.data[offset + 25], pe32.data[offset + 26], pe32.data[offset + 27]]) as u32;
        pe32.section_headers.push(Section_header {
            name,
            virtual_size,
            virtual_address,
            size_of_raw_data,
            pointer_to_raw_data,
            characteristics,
        });
        print!("Section header: {:x?}\n", pe32.section_headers[i as usize]);

    };
    return pe32;
}

impl pe32File {
    pub fn load_into_memory(&self, memory: &mut Memory) {
        // load the sections into memory
        for i in 0..self.COFF_header.number_of_sections {
            let section = &self.section_headers[i as usize];
            let offset = section.pointer_to_raw_data as usize;
            let size = section.size_of_raw_data as usize;
            memory.create_mapping(section.virtual_size as i64, section.virtual_address as i64);
            memory.write(offset, &self.data[offset..offset + size]);
        }
    }
}

