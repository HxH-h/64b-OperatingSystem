# pragma once

# define PAGE_SIZE 0x200000
# define PAGE_SHIFT 21
# define PAGE_SIZE_4K 0x1000
# define PAGE_SHIFT_4K 12
# define PAGE_MASK (PAGE_SIZE - 1)

// 向上对齐
#define ALIGN_UP_2MB(addr) \
    ( ( (uint64_t)(addr) + PAGE_MASK ) & ~PAGE_MASK )

// bitmap起始地址
# define BITMAP_4K_ADDR 0x8000


void init_memory();
