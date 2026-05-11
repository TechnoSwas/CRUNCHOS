#include <stdint.h>

void kernel_main(void) {
    volatile uint16_t* vga = (uint16_t*)0xB8000;

    vga[0] = 0x0F00 | 'C';
    vga[1] = 0x0F00 | 'R';
    vga[2] = 0x0F00 | 'U';
    vga[3] = 0x0F00 | 'N';
    vga[4] = 0x0F00 | 'C';

    while (1) {}
}
