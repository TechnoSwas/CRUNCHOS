#include <stdint.h>

#define MULTIBOOT_MAGIC    0x1BADB002
#define MULTIBOOT_FLAGS    0x00000007
#define MULTIBOOT_CHECKSUM -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

__attribute__((section(".multiboot"))) __attribute__((aligned(4)))
const unsigned int multiboot_header[] = {
    MULTIBOOT_MAGIC,
    MULTIBOOT_FLAGS,
    MULTIBOOT_CHECKSUM,
    0, 0, 0, 0, 0,
    1024, 768, 24
};

typedef struct {
    uint32_t flags;
    uint32_t mem_lower, mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count, mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length, mmap_addr;
    uint32_t drives_length, drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info, vbe_mode_info, vbe_mode;
    uint32_t vbe_interface_seg, vbe_interface_off, vbe_interface_len;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width, framebuffer_height;
    uint8_t  framebuffer_bpp;
} multiboot_info_t;

uint8_t* fb;
uint32_t pitch;
uint32_t W, H;

void put_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    uint8_t* p = fb + y * pitch + x * 3;
    p[0] = b; p[1] = g; p[2] = r;
}

void draw_rect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b) {
    for (int j = y; j < y+h; j++)
        for (int i = x; i < x+w; i++)
            put_pixel(i, j, r, g, b);
}

void kernel_main(uint32_t magic, multiboot_info_t* mbi) {
    fb    = (uint8_t*)(uint32_t)mbi->framebuffer_addr;
    pitch = mbi->framebuffer_pitch;
    W     = mbi->framebuffer_width;
    H     = mbi->framebuffer_height;

    draw_rect(0, 0, W, H, 15, 15, 25);
    draw_rect(0, 0, W, 48, 20, 20, 40);
    draw_rect(0, 48, W, 3, 0, 120, 255);
    draw_rect(W/2-250, 150, 500, 400, 30, 30, 50);
    draw_rect(W/2-250, 150, 500, 4, 0, 120, 255);
    draw_rect(W/2-150, 480, 300, 45, 0, 120, 255);
    draw_rect(0, H-40, W, 40, 20, 20, 40);
    draw_rect(0, H-40, W, 2, 0, 120, 255);

    while (1) {}
}
