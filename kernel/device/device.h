# pragma once

# include "../lib/stdtype.h"
# include "../global.h"

# define DEVICE_BASE (0xFFFFFF0001000000)
# define DEVICE_MASK (0x1FFFFF)
# define DEVICE_PHY_BASE (0xFEC00000)

void init_device(void);

static inline uint64_t get_mmio(uint32_t phy_addr){
    return (uint64_t)(DEVICE_BASE + ((phy_addr) & DEVICE_MASK));
}

static inline uint32_t get_rcba(){
    io_out32(0xcf8,0x8000f8f0);
	uint32_t x = io_in32(0xcfc);

    x = x & 0xffffc000;
    return x;
}

static inline uint32_t *get_conf_reg(uint32_t offset){
    uint32_t x = get_rcba();
    return (uint32_t *)get_mmio(x + offset);
}