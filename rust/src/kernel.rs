use crate::filesystem::Filesystem;
pub struct Kernel {
    filesystem: Filesystem,
}

impl Kernel {
    pub fn new() -> Kernel {
        Kernel {
            filesystem: Filesystem::new(),
        }
    }
}
