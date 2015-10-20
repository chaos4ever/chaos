#![feature(core_slice_ext)]
#![feature(core_str_ext)]
#![feature(lang_items)]
#![feature(no_std)]
#![feature(start)]

#![no_std]

mod system;

#[no_mangle]
pub unsafe extern fn main() -> i32 {
    system::process_name_set("virtio-net-pci");
    system::thread_name_set("Initialising");

    loop {}
}
