mod c_string;
pub mod types;
pub use self::c_string::{CString};

use self::types::*;

extern {
    fn system_process_name_set(name: *const c_char) -> return_type;
    fn system_thread_name_set(name: *const c_char) -> return_type;
}

pub unsafe fn process_name_set(name: &str) -> return_type {
    let s = CString::new(name);
    return system_process_name_set(s.as_ptr());
}

pub unsafe fn thread_name_set(name: &str) -> return_type {
    let s = CString::new(name);
    return system_thread_name_set(s.as_ptr());
}
