#include <stdint.h>

#define MULTIBOOT_MAGIC 0x1BADB002
#define MULTIBOOT_FLAGS 0x00
#define MULTIBOOT_CHECKSUM -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

__attribute__((section(".multiboot")))
const unsigned int multiboot_header[] = {
    MULTIBOOT_MAGIC,
    MULTIBOOT_FLAGS,
    MULTIBOOT_CHECKSUM
};

void serial_write(char c) {
    __asm__ volatile ("outb %0, %1" : : "a"(c), "Nd"(0x3F8));
}

void serial_print(const char* msg) {
    while (*msg) serial_write(*msg++);
}

void kernel_main(void) {
    serial_print("Welcome to CrunchOS!\n");

    volatile uint16_t* vga = (uint16_t*)0xB8000;
    const char* msg = "Welcome to CrunchOS!";
    int i = 0;
    while (msg[i]) {
        vga[i] = 0x1F00 | msg[i];
        i++;
    }
    while (1) {}
}
