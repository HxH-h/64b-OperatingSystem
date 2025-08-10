# include "memory.h"
# include "../console/console.h"
# include "../lib/stdtype.h"
# include "../lib/bitmap.h"

// 取消结构体对齐
typedef struct __attribute__((packed)){
    uint64_t start;
    uint64_t size;
    uint32_t type;
} BIOS_memory;

typedef struct {
    Bitmap bitmap;
    uint64_t pool_size;
    uint64_t addr_start;
} pool;

pool pool_4k , pool_user_2m , pool_core_2m;

// 内核结束地址
extern char _end;
void init_memory(){
    BIOS_memory* mem = (BIOS_memory*)0xffff800000000800;
    uint64_t phy_memory_size = 0;

    while(mem->type >= 1 && mem->type <= 4 && mem->size != 0){
        // print("Start: %x, Size: %x, Type: %x\n",mem->start, mem->size, mem->type);
        if (mem->type == 1) phy_memory_size += mem->size;
        mem++;
    }
    print("Total memory: %x\n", phy_memory_size);
    print("Kernel end: %x\n", &_end);

    // 计算总页数
    uint64_t end = (uint64_t)&_end & 0x00007fffffffffff;
    uint64_t total_pages = (phy_memory_size - ALIGN_UP_2MB(end)) >> PAGE_SHIFT; 
    print("Total pages: %x\n", total_pages);

    //4KB 和 2MB 1 ：3
    uint64_t total_pages_4K = (total_pages / 4) * (PAGE_SIZE / PAGE_SIZE_4K);
    uint64_t total_pages_2M = total_pages - (total_pages / 4) - 1;
    
    // user 和 kernel 1 ：1
    uint64_t total_pages_user = total_pages_2M / 2;
    uint64_t total_pages_kernel = total_pages_2M - total_pages_user;

    print("4K pages: %x, user pages: %x kernel pages: %x\n" , total_pages_4K, total_pages_user, total_pages_kernel);
    // 计算位图大小
    uint32_t bitmap_size_4k = total_pages_4K / 8;
    uint32_t bitmap_size_user = total_pages_user / 8;
    uint32_t bitmap_size_kernel = total_pages_kernel / 8;

    // 计算物理起始分配地址
    uint64_t phy_user_2m = ALIGN_UP_2MB(end);
    uint64_t phy_core_2m = phy_user_2m + total_pages_user * PAGE_SIZE;

    uint64_t phy_4k = phy_core_2m + total_pages_kernel * PAGE_SIZE;

    // 计算位图起始地址
    uint64_t bitmap_user = BITMAP_4K_ADDR + bitmap_size_4k;
    uint64_t bitmap_kernel = bitmap_user + bitmap_size_user;

    print("Bitmap_4K: start %x, size %x\n", BITMAP_4K_ADDR, bitmap_size_4k);
    print("Bitmap_User: start %x, size %x\n", bitmap_user, bitmap_size_user);
    print("Bitmap_Kernel: start %x, size %x\n", bitmap_kernel, bitmap_size_kernel);

    

    // 初始化内存池
    bitmap_init(&pool_4k.bitmap, (uint8_t*)BITMAP_4K_ADDR, bitmap_size_4k);
    bitmap_init(&pool_user_2m.bitmap, (uint8_t*)bitmap_user, bitmap_size_user);
    bitmap_init(&pool_core_2m.bitmap, (uint8_t*)bitmap_kernel, bitmap_size_kernel);

    pool_4k.pool_size = total_pages_4K;
    pool_4k.addr_start = phy_4k;

    pool_user_2m.pool_size = total_pages_user;
    pool_user_2m.addr_start = phy_user_2m;

    pool_core_2m.pool_size = total_pages_kernel;
    pool_core_2m.addr_start = phy_core_2m;

    print("4K pool: size %x, start %x\n", pool_4k.pool_size, pool_4k.addr_start);
    print("User pool: size %x, start %x\n", pool_user_2m.pool_size, pool_user_2m.addr_start);
    print("Core pool: size %x, start %x\n", pool_core_2m.pool_size, pool_core_2m.addr_start);

}





