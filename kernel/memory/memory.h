# pragma once

# include "../lib/stdtype.h"

# define PAGE_SIZE 0x200000
# define PAGE_SHIFT 21
# define PAGE_SIZE_4K 0x1000
# define PAGE_SHIFT_4K 12
# define PAGE_MASK (PAGE_SIZE - 1)

# define SLAB_CACHE_SIZE 16
# define SLAB_START 32

# define PG_EXIST 0x1

# define PG_RW (0x1 << 1)
# define PG_R (0x0 << 1)

# define PG_USER (0x1 << 2)
# define PG_CORE (0x0 << 2)

# define PG_2MB_BASE (0x1 << 7)

// 向上对齐
#define ALIGN_UP_2MB(addr) \
    ( ( (uint64_t)(addr) + PAGE_MASK ) & ~PAGE_MASK )

// bitmap起始地址
# define BITMAP_4K_ADDR 0x8000


# define PML4_MASK (0xfffffffffffff000)
# define PDPT_MASK (0xffffffffffe00000)
# define PDE_MASK (0xffffffffc0000000)
# define PDE_ITEM_MASK (0x000FFFFFFFFFE00000)


#define GET_PML4E(vAddr) ((uint64_t *)(((vAddr >> 39) << 3) | PML4_MASK))
#define GET_PDPTE(vAddr) ((uint64_t *)(((vAddr >> 30) << 3) | PDPT_MASK))
#define GET_PDE(vAddr)   ((uint64_t *)(((vAddr >> 21) << 3) | PDE_MASK))

#define V2P_2MB(vAddr) \
    ( ((uint64_t)(*GET_PDE(vAddr)) & PDE_ITEM_MASK) | (vAddr & PAGE_MASK) )

typedef enum{
    MEM_4K, USER_2M, KERNEL_2M 
} Pool_type;


void init_memory();

void memset(void *_dst , uint8_t value, uint32_t size);
void memcpy(void *_dst , void *_src, uint32_t size);

void* page_alloc(Pool_type type, uint32_t pg_cnt);
bool page_free(Pool_type type, void* _vaddr, uint32_t pg_cnt);

void* kmalloc(uint32_t size);
void kfree(void* _vaddr);
