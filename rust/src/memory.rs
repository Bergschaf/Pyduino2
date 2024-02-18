pub struct Memory {
    mappings: Vec<Mapping>,
}

struct Mapping {
    data: Vec<u8>,
    size: i64,
    virtual_address: i64,
}

impl Mapping {
    fn expand(&mut self, size: i64) {
        self.size += size;
        self.data.resize(self.size as usize, 0);
    }
}


pub const MEMORY_SIZE: i64 = 1024 * 1024 * 1024;
// 1GB TODO may be slow
pub const STACK_SIZE: i64 = 1024 * 1024;

// 1MB
impl Memory {
    pub fn new() -> Memory {
        Memory {
            mappings: Vec::new(),
        }
    }
    pub fn create_mapping(&mut self, size: i64, virtual_address: i64) {
        // check if the mapping overlaps with another mapping
        for mapping in &self.mappings {
            if virtual_address < mapping.virtual_address + mapping.size && virtual_address + size > mapping.virtual_address {
                panic!("Mapping overlaps with another mapping");
            }
        }
        self.mappings.push(Mapping {
            data: vec![0; size as usize],
            size,
            virtual_address,
        });
    }

    pub fn write(&mut self, address: usize, data: &[u8]) {
        for mapping in &mut self.mappings {
            if address as i64 >= mapping.virtual_address && (address as i64) < mapping.virtual_address + mapping.size {
                let offset = address as i64 - mapping.virtual_address;
                if offset + data.len() as i64 > mapping.size {
                    mapping.expand(offset + data.len() as i64 - mapping.size);
                }
                for i in 0..data.len() {
                    mapping.data[(offset as usize + i) as usize] = data[i];
                }
                return;
            }
        }
        panic!("No mapping found for address: {:x}", address);
    }
    pub fn read(&self, address: usize, size: usize) -> Vec<u8> {
        for mapping in &self.mappings {
            if address as i64 >= mapping.virtual_address && (address as i64) < mapping.virtual_address + mapping.size {
                let offset = address as i64 - mapping.virtual_address;
                if offset + size as i64 > mapping.size {
                    panic!("Read out of bounds");
                }
                return mapping.data[offset as usize..(offset + size as i64) as usize].to_vec();
            }
        }
        panic!("No mapping found for address: {:x}", address);
    }
    pub fn write_u64(&mut self, address: usize, data: u64) {
        self.write(address, &data.to_le_bytes());
    }
    pub fn read_u64(&self, address: usize) -> u64 {
        let mut data = [0; 8];
        data.copy_from_slice(&self.read(address, 8));
        u64::from_le_bytes(data)
    }
    pub fn write_string(&mut self, address: usize, string: &str) {
        self.write(address, string.as_bytes());
    }
    pub fn read_string(&self, address: usize, count: usize) -> String {
        let mut size = 0;
        let mut c = 0;
        while c < count {
            if self.read(address + c, 1)[0] == 0 {
                break;
            }
            c += 1;
        }
        size = c;
        String::from_utf8(self.read(address, size)).unwrap()
    }
}