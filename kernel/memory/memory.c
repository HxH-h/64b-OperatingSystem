# include "memory.h"
# include "../console/console.h"
# include "../lib/stdtype.h"

// 取消结构体对齐
typedef struct __attribute__((packed)){
    uint64_t start;
    uint64_t size;
    uint32_t type;
} BIOS_memory;

uint64_t phy_memory_size = 0;

void init_memory(){
    BIOS_memory* mem = (BIOS_memory*)0xffff800000000800;

    while(mem->type >= 1 && mem->type <= 4 && mem->size != 0){
        print("Start: %x, Size: %x, Type: %x\n",mem->start, mem->size, mem->type);
        if (mem->type == 1) phy_memory_size += mem->size;
        mem++;
    }
    print("Total memory: %x\n", phy_memory_size);

}





