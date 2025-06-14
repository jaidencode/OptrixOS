// pmm.c - Simple physical memory manager (frame allocator)
#include <stdint.h>
#include <stddef.h>

#define FRAME_SIZE 4096        // 4 KiB
#define TOTAL_MEMORY (16 * 1024 * 1024)  // 16 MiB
#define FRAME_COUNT (TOTAL_MEMORY / FRAME_SIZE)

static uint8_t frame_bitmap[FRAME_COUNT / 8]; // 512 bytes

// Set or clear a bit in the bitmap
static void set_frame(uint32_t frame) {
    frame_bitmap[frame / 8] |= (1 << (frame % 8));
}
static void clear_frame(uint32_t frame) {
    frame_bitmap[frame / 8] &= ~(1 << (frame % 8));
}
static int test_frame(uint32_t frame) {
    return frame_bitmap[frame / 8] & (1 << (frame % 8));
}

// Initialize: all frames used except ones above kernel (simple, safe)
void init_pmm(uint32_t kernel_end_addr) {
    for (uint32_t i = 0; i < FRAME_COUNT / 8; ++i)
        frame_bitmap[i] = 0xFF; // all used

    // Mark frames after kernel as free
    uint32_t first_free = (kernel_end_addr + FRAME_SIZE - 1) / FRAME_SIZE;
    for (uint32_t i = first_free; i < FRAME_COUNT; ++i)
        clear_frame(i);
}

// Allocate a free frame, return address or 0 if none
uint32_t alloc_frame() {
    for (uint32_t i = 0; i < FRAME_COUNT; ++i) {
        if (!test_frame(i)) {
            set_frame(i);
            return i * FRAME_SIZE;
        }
    }
    return 0;
}

// Free a frame at given physical address
void free_frame(uint32_t addr) {
    uint32_t frame = addr / FRAME_SIZE;
    clear_frame(frame);
}
