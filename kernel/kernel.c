#include <stdint.h>

#define MULTIBOOT_MAGIC    0x1BADB002
#define MULTIBOOT_FLAGS    0x00
#define MULTIBOOT_CHECKSUM -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

__attribute__((section(".multiboot"))) __attribute__((aligned(4)))
unsigned int multiboot_header[] = {
    MULTIBOOT_MAGIC,
    MULTIBOOT_FLAGS,
    MULTIBOOT_CHECKSUM
};

#define VGA ((volatile uint16_t*)0xB8000)

void kernel_main(void) {
    const char* msg = "CrunchOS is alive!";
    for (int i = 0; msg[i]; i++)
        VGA[i] = 0x1F00 | msg[i];
    while(1);
}
