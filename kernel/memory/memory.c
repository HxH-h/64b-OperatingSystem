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
} Pool;

typedef enum{
    MEM_4K, USER_2M, KERNEL_2M 
} Pool_type;

// 内核结束地址
extern char _end;
// 物理内存池
Pool pool_4k , pool_user_2m , pool_core_2m;

// 内核虚拟内存池
Pool pool_virt;


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


    // 初始化虚拟内存池
    uint64_t bitmap_virt = bitmap_kernel + bitmap_size_kernel;
    bitmap_init(&pool_virt.bitmap, (uint8_t*)bitmap_virt, bitmap_size_kernel);

    pool_virt.pool_size = total_pages_kernel;
    pool_virt.addr_start = ALIGN_UP_2MB(end);

    print("Kernel Virt pool: size %x, start %x\n", pool_virt.pool_size, pool_virt.addr_start);
}

void memset(void *_dst , uint8_t value, uint32_t size){
    uint8_t* dst = _dst;
    while (size--) *(dst++) = value;
}
void memcpy(void *_dst , void *_src, uint32_t size){
    uint8_t *src = _src;
    uint8_t *dst = _dst;
    while (size--) *(dst++) = *(src++);
}

// bitmap 分配
uint64_t bitmap_alloc(Bitmap* bitmap , uint32_t count){

    int index = bitmap_scan(bitmap, count);

    if(index == -1) return NULL;
    // 设置已分配
    bitmap_con_set(bitmap, index, count, 1);

    return index;
}

// 分配物理页
// 一次只分配一页
// 返回物理地址
void* palloc(Pool_type type){
    Pool* pool = NULL;
    uint32_t p_size = PAGE_SIZE;

    if(type == MEM_4K){
        pool = &pool_4k;
        p_size = PAGE_SIZE_4K;
    }else if(type == USER_2M) pool = &pool_user_2m;
    else if(type == KERNEL_2M) pool = &pool_core_2m;
    else return NULL;

    uint64_t idx = bitmap_alloc(&pool->bitmap, 1);

    return (void*)(pool->addr_start + idx * p_size);
}

// 释放物理页
bool pfree(Pool_type type, void* addr){ 
    Pool* pool = NULL;
    uint32_t p_size = PAGE_SIZE;
    if(type == MEM_4K){
        pool = &pool_4k;
        p_size = PAGE_SIZE_4K;
    }else if(type == USER_2M) pool = &pool_user_2m;
    else if(type == KERNEL_2M) pool = &pool_core_2m;
    else return false;

    if(addr < pool->addr_start || addr >= pool->addr_start + pool->pool_size * p_size) return false;

    uint64_t bit_idx = ((uint64_t)addr - pool->addr_start) / p_size;

    bitmap_set(&pool->bitmap, bit_idx, 0);

    return true;

}

// 分配虚拟页
// 返回虚拟地址
void* valloc(Pool_type type , uint32_t pg_cnt){
    Pool* pool = NULL;
    if(type == KERNEL_2M) pool = &pool_virt;
    else if (type == USER_2M){
    
    }
    else return NULL;

    uint64_t idx = bitmap_alloc(&pool->bitmap, pg_cnt);
    return (void*)(pool->addr_start + idx * PAGE_SIZE);
}

// 释放虚拟页
bool vfree(Pool_type type, void* addr , uint32_t pg_cnt){ 
    Pool* pool = NULL;
    if(type == KERNEL_2M) pool = &pool_virt;
    else if (type == USER_2M){
    
    }
    else return false;

    uint64_t bit_idx = ((uint64_t)addr - pool->addr_start) / PAGE_SIZE;
    bitmap_con_set(&pool->bitmap, bit_idx, pg_cnt, 0);
    return true;
}


bool page_table_add(void* _vaddr, void* _paddr){

    uint64_t* pml4e = GET_PML4E((uint64_t)_vaddr);
    uint64_t* pdpte = GET_PDPTE((uint64_t)_vaddr);
    uint64_t* pde = GET_PDE((uint64_t)_vaddr);

    // 检查各级页表项是否存在
    if(!(*pml4e & PG_EXIST)){
        uint64_t addr = (uint64_t)palloc(MEM_4K);
        *pml4e = addr | PG_USER | PG_RW | PG_EXIST;
        memset((void *)((uint64_t)pdpte & 0xfffffffffffff000), 0, PAGE_SIZE_4K);
    }
    if(!(*pdpte & PG_EXIST)){
        uint64_t addr = (uint64_t)palloc(MEM_4K);
        *pdpte = addr | PG_USER | PG_RW | PG_EXIST;
        memset((void *)((uint64_t)pde & 0xfffffffffffff000), 0, PAGE_SIZE_4K);
    }

    *pde = (uint64_t)_paddr | PG_USER | PG_RW | PG_EXIST;
    
    return true;
}

bool page_table_remove(void* vaddr){ 
    uint64_t* pde = GET_PDE((uint64_t)vaddr);
    *pde &= ~PG_EXIST;
    asm volatile("invlpg %0"::"m"(vaddr):"memory");
}

// 分配页
void* page_alloc(Pool_type type, uint32_t pg_cnt){
    void* vaddr = valloc(type, pg_cnt);

    if(!vaddr){
        print("page_alloc: valloc failed\n");
        return NULL;
    }

    uint64_t vaddr_start = (uint64_t)vaddr;
    while(pg_cnt--){
        void* paddr = palloc(type);
        if(!paddr){
            print("page_alloc: palloc failed\n");
            return NULL;
        }
        page_table_add((void*)vaddr_start, paddr);
        vaddr_start += PAGE_SIZE;
    }


    return vaddr;
}

// 释放页
bool page_free(Pool_type type, void* _vaddr, uint32_t pg_cnt){ 

    uint64_t vaddr = (uint64_t)_vaddr;
    vfree(type, _vaddr, pg_cnt);

    while(pg_cnt--){
        uint64_t paddr = V2P_2MB(vaddr);
        pfree(type, (void*)paddr);

        page_table_remove((void*)vaddr);

        vaddr += PAGE_SIZE;
    }
    return true;
}


