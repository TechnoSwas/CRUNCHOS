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
#define W 80
#define H 25

void set(int x, int y, char c, uint8_t color) {
    VGA[y * W + x] = (color << 8) | c;
}

void print(const char* s, int x, int y, uint8_t color) {
    for (int i = 0; s[i]; i++)
        set(x + i, y, s[i], color);
}

void fill_row(int y, uint8_t color) {
    for (int x = 0; x < W; x++)
        set(x, y, ' ', color);
}

void fill_screen(uint8_t color) {
    for (int y = 0; y < H; y++)
        fill_row(y, color);
}

void kernel_main(void) {
    fill_screen(0x1F);

    // top bar
    fill_row(0, 0x70);
    print("  CrunchOS Setup                                                              v0.1", 0, 0, 0x70);

    // title
    print("Welcome to CrunchOS!", 30, 5, 0x1F);
    print("The OS built different.", 28, 6, 0x1E);

    // divider
    print("--------------------------------------------------------------------------------", 0, 8, 0x1B);

    // options
    print("  [1] Install CrunchOS", 20, 11, 0x1F);
    print("  [2] Boot from Live Disk", 20, 13, 0x1F);
    print("  [3] Repair Installation", 20, 15, 0x1F);

    // divider
    print("--------------------------------------------------------------------------------", 0, 17, 0x1B);

    // bottom bar
    fill_row(24, 0x70);
    print("  Use number keys to select an option                     CrunchOS 2025", 0, 24, 0x70);

    while(1);
}
