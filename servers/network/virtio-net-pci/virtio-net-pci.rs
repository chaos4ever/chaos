// Abstract: Entry point for the virtio-net-pci network server.
// Authors: Per Lundberg <per@halleluja.nu>
//
// © Copyright 2015 chaos development.

#![feature(core_slice_ext)]
#![feature(core_str_ext)]
#![feature(lang_items)]
#![feature(no_std)]
#![feature(start)]

#![no_std]

mod log;
mod pci;
mod system;

#[no_mangle]
pub unsafe extern fn main() -> i32 {
    system::process_name_set("virtio-net-pci");
    system::thread_name_set("Initialising");

    let log_client = log::Client.new("virtio-net-pci").init();

    let pci_client = pci::Client.new();
    if (pci_client.init() != pci::ReturnValues::Success) {
        log_client.print(log::Urgencies::Emergency, "Couldn't create connection to PCI service.");
        return -1;
    }

    0
}
