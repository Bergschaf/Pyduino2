use std::collections::HashMap;

pub struct Filesystem {
    files: Vec<File>,
    open_files: HashMap<i32, File>, // file descriptor -> file
}

pub struct File {
    name: String,
    data: Vec<u8>,
    offset: usize,
}

impl Filesystem {
    pub fn new() -> Filesystem {
        Filesystem {
            files: Vec::new(),
            open_files: HashMap::new(),
        }
    }
}