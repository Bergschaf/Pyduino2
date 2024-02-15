use std::collections::HashMap;
use syscall::flag as syscall_flag;

pub struct Filesystem {
    files: Vec<File>,
    open_files: HashMap<i32, (i32, AccessMode, FileStatusFlags)>, // file descriptor -> (index in files, ..., ...)
}

pub struct File {
    name: String,
    data: Vec<u8>,
    offset: usize,
}

#[derive(Debug)]
pub enum AccessMode {
    Read,
    Write,
    ReadWrite,
}

pub struct FileCreationFlags {
    pub create: bool,
}

pub struct FileStatusFlags {
    pub append: bool,
}

impl Filesystem {
    pub fn new() -> Filesystem {
        let mut fs = Filesystem {
            files: Vec::new(),
            open_files: HashMap::new(),
        };
        fs.create_file("test.txt", b"Hello, World!\n".to_vec());
        fs
    }
    pub fn create_file(&mut self, name: &str, data: Vec<u8>) {
        self.files.push(File {
            name: name.to_string(),
            data,
            offset: 0,
        });
    }

    fn get_next_fd(&self) -> i32 {
        let mut fd = 2;
        while self.open_files.contains_key(&fd) {
            fd += 1;
        };
        fd
    }

    fn search_file(&self, name: &str) -> Option<usize> {
        for (i, file) in self.files.iter().enumerate() {
            if file.name == name {
                return Some(i);
            }
        }
        None
    }

    pub fn open_file(&mut self, name: &str, access_mode: AccessMode, creation_flags: FileCreationFlags, status_flags: FileStatusFlags) -> i32 {
        match access_mode {
            AccessMode::Read => {
                if let Some(index) = self.search_file(name) {
                    let fd = self.get_next_fd();
                    self.open_files.insert(fd, (index as i32, access_mode, status_flags));
                    fd
                } else {
                    -1
                }
            }
            AccessMode::Write => {
                if let Some(index) = self.search_file(name) {
                    let fd = self.get_next_fd();
                    self.open_files.insert(fd, (index as i32, access_mode, status_flags));
                    fd
                } else {
                    if creation_flags.create {
                        self.create_file(name, Vec::new());
                        let fd = self.get_next_fd();
                        self.open_files.insert(fd, (self.files.len() as i32 - 1, access_mode, status_flags));
                        fd
                    } else {
                        -1
                    }
                }
            }
            AccessMode::ReadWrite => {
                if let Some(index) = self.search_file(name) {
                    let fd = self.get_next_fd();
                    self.open_files.insert(fd, (index as i32, access_mode, status_flags));
                    fd
                } else {
                    if creation_flags.create {
                        self.create_file(name, Vec::new());
                        let fd = self.get_next_fd();
                        self.open_files.insert(fd, (self.files.len() as i32 - 1, access_mode, status_flags));
                        fd
                    } else {
                        -1
                    }
                }
            }
        }
    }

    pub fn close_file(&mut self, fd: i32) {
        self.open_files.remove(&fd);
    }


}