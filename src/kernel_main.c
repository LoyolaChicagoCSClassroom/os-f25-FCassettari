
#include <stdint.h>

#define MULTIBOOT2_HEADER_MAGIC         0xe85250d6

#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VIDEO_MEMORY 0xB8000

const unsigned int multiboot_header[]  __attribute__((section(".multiboot"))) = {MULTIBOOT2_HEADER_MAGIC, 0, 16, -(16+MULTIBOOT2_HEADER_MAGIC), 0, 12};

uint8_t inb (uint16_t _port) {
    uint8_t rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

int offset = 0; //where the next character goes

void scroll() {
    volatile uint16_t* vram = (uint16_t*)VIDEO_MEMORY;
    for (int row = 1; row < VGA_HEIGHT; row++) {
        for (int col = 0; col < VGA_WIDTH; col++) {
            vram[(row - 1) * VGA_WIDTH + col] = vram[row * VGA_WIDTH + col];
        }
    }
    //clear bottom line
    for (int col = 0; col < VGA_WIDTH; col++) {
        vram[(VGA_HEIGHT - 1) * VGA_WIDTH + col] = (' ' | (0x07 << 8));
    }
    offset -= VGA_WIDTH;
}

void putc(int data) {
    volatile uint16_t* vram = (uint16_t*)VIDEO_MEMORY;
    if (data == '\n') {
        offset += VGA_WIDTH - (offset % VGA_WIDTH); //next line
    } else {
        vram[offset] = (data & 0xFF) | (0x07 << 8); // char + color
        offset++;
    }
    if (offset >= VGA_WIDTH * VGA_HEIGHT) {
        scroll();
    }
}

void print_hex(uint32_t num) {
    const char *hex = "0123456789ABCDEF";
    for (int i = 28; i >= 0; i -= 4) {
        putc(hex[(num >> i) & 0xF]);
    }
}

void main() {
    putc('H'); putc('e'); putc('l'); putc('l'); putc('o'); putc('\n');

    for (int i = 1008; i > 0; i--) { putc('j'); }

    uint32_t esp_val;
    __asm__ __volatile__("mov %%esp, %0" : "=r"(esp_val));

    putc('E'); putc('S'); putc('P'); putc(':'); putc(' ');
    print_hex(esp_val);
    putc('\n');

    while (1);
}
