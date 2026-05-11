void kernel_main() {
    volatile unsigned short* vga = (unsigned short*)0xB8000;

    vga[0] = (0x0A << 8) | 'K';
    vga[1] = (0x0A << 8) | 'O';
    vga[2] = (0x0A << 8) | 'S';

    while (1) {}
}

void _start() {
    kernel_main();
}
