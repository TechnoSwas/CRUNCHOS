volatile unsigned short* vga = (unsigned short*)0xB8000;

void print(const char* str, unsigned char color) {
    int i = 0;

    while (str[i]) {
        vga[i] = (color << 8) | str[i];
        i++;
    }
}

void kernel_main() {
    print("Kernel Loaded!", 0x0A);

    while (1) {
        __asm__("hlt");
    }
}

void _start() {
    kernel_main();
}
