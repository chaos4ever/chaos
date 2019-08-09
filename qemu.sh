qemu-system-i386 \
    -s \
    -m 128 \
    -net nic,model=e1000 \
    -soundhw sb16 \
    -cdrom chaos.iso
