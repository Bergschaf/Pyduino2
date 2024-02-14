pub struct Memory {
    data: Vec<u8>,
}
pub const MEMORY_SIZE: i64 = 1024 * 1024 * 1024; // 1GB TODO may be slow
pub const STACK_SIZE: i64 = 1024 * 1024; // 1MB
impl Memory {
    pub fn new() -> Memory {
        Memory {
            data: vec![0; MEMORY_SIZE as usize],
        }
    }
    pub fn write(&mut self, address: usize, data: &[u8]) {
        self.data[address..address + data.len()].copy_from_slice(data);
    }
    pub fn read(&self, address: usize, size: usize) -> Vec<u8> {
        self.data[address..address + size].to_vec()
    }
    pub fn write_u64(&mut self, address: usize, data: u64) {
        self.write(address, &data.to_le_bytes());
    }
    pub fn read_u64(&self, address: usize) -> u64 {
        let mut data = [0; 8];
        data.copy_from_slice(&self.data[address..address + 8]);
        u64::from_le_bytes(data)
    }
    pub fn write_string(&mut self, address: usize, string: &str) {
        self.write(address, string.as_bytes());
    }
    pub fn read_string(&self, address: usize) -> String {
        let mut size = 0;
        while self.data[address + size] != 0 {
            size += 1;
        }
        String::from_utf8(self.data[address..address + size].to_vec()).unwrap()
    }
}