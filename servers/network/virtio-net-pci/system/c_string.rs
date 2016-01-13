const MAX_LENGTH: usize = 1024;

pub struct CString {
    buffer: [i8; MAX_LENGTH],
}

impl CString {
    pub unsafe fn new(s: &str) -> CString {
        CString {
            buffer: CString::to_c_string(s)
        }
    }

    unsafe fn to_c_string(s: &str) -> [i8; MAX_LENGTH] {
        let mut buffer: [i8; MAX_LENGTH] = [0; MAX_LENGTH];
        let mut i = 0;

        // TODO: ignore the risk for buffer overruns for now. :)
        // TODO: likewise with UTF8; assume that we are ASCII-only.
        for c in s.chars() {
            *buffer.get_unchecked_mut(i) = c as i8;
            i = i + 1;
        }

        *buffer.get_unchecked_mut(s.len()) = '\0' as i8;
        buffer
    }

    pub fn as_ptr(&self) -> *const i8 {
        &self.buffer as *const i8
    }
}
