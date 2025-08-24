# include "apic.h"

# include "../global.h"

# include "../console/console.h"

# define IA32_APIC_BASE 0x1B
# define LAPIC_TPR 0x808
# define LAPIC 0x80F
# define LAPIC_EOI 0x80B

# define LVT_CMCI 0x82F
# define LVT_TIMER 0x832
# define LVT_THERMAL 0x833
# define LVT_PERFORMANCE 0x834
# define LVT_LINT0 0x835
# define LVT_LINT1 0x836
# define LVT_ERROR 0x837


# define APIC_ENABLE (1 << 11)
# define x2APIC_ENABLE (1 << 10)
#define LAPIC_ENABLE  (1 << 8)

# define IOAPIC_BASE (0xFFFFFF0001000000)
# define IOREGSEL     ((uint32_t *)(IOAPIC_BASE + 0x00))
# define IOWIN        ((uint32_t *)(IOAPIC_BASE + 0x10))
# define IO_EOI       ((uint32_t *)(IOAPIC_BASE + 0x40))


typedef struct {
    uint8_t vector;         // 中断向量号
    uint8_t delivery_mode;  // 000=Fixed...
    uint8_t dest_mode;      // 0=Physical, 1=Logical
    uint8_t polarity;       // 0=高有效, 1=低有效
    uint8_t trigger_mode;   // 0=Edge, 1=Level
    uint8_t mask;           // 0=enable, 1=mask
    uint8_t reserved_low;   // 保留
    uint8_t dest_lapic_id;  // 目标 LAPIC ID (0~255)
} ioapic_rte;


void init_apic(void) { 

    // 检查是否支持APIC
    uint32_t ecx , edx;
    cpuid(1, 0, NULL, NULL , &ecx, &edx);

    if (!(edx & (1 << 9))) print("APIC not supported\n");
    if (!(ecx & (1 << 21))) print("x2APIC not enabled\n");

    // 启用x2APIC
    uint64_t val = APIC_ENABLE | x2APIC_ENABLE;
    wrmsr(IA32_APIC_BASE, val);

    // 开启LAPIC
    val = LAPIC_ENABLE | 0xFF;
    wrmsr(LAPIC , val);

    wrmsr(LAPIC_TPR , 0);

    wrmsr(LAPIC_EOI , 0);

    // 禁用8295a
    outb(0x21 , 0xff);
    outb(0xa1 , 0xff);


    // 设置LVT
    wrmsr(LVT_CMCI , 0x10000);
    wrmsr(LVT_TIMER , 0x10000);
    wrmsr(LVT_THERMAL , 0x10000);
    wrmsr(LVT_PERFORMANCE , 0x10000);
    wrmsr(LVT_LINT0 , 0x10000);
    wrmsr(LVT_LINT1 , 0x10000);
    wrmsr(LVT_ERROR , 0x10000);

}


// 写 RTE
void ioapic_write_rte_struct(uint32_t irq, const ioapic_rte *rte) {
    uint32_t low  = 0, high = 0;

    // 构造低 32 位
    low |= rte->vector & 0xFF;
    low |= ((uint32_t)(rte->delivery_mode & 0x7)) << 8;
    low |= ((uint32_t)(rte->dest_mode & 0x1)) << 11;
    low |= ((uint32_t)(rte->polarity & 0x1)) << 13;
    low |= ((uint32_t)(rte->trigger_mode & 0x1)) << 15;
    low |= ((uint32_t)(rte->mask & 0x1)) << 16;

    // 高 32 位：只关心 bit56-63
    high = ((uint32_t)(rte->dest_lapic_id & 0xFF)) << 24;

    // 写高字
    *IOREGSEL = 0x10 + irq * 2 + 1;
    io_mfence();
    *IOWIN = high;
    io_mfence();

    // 写低字
    *IOREGSEL = 0x10 + irq * 2;
    io_mfence();
    *IOWIN = low;
    io_mfence();
}

// 读 RTE
void ioapic_read_rte_struct(unsigned int irq, ioapic_rte *rte) {
    uint32_t low, high;

    *IOREGSEL = 0x10 + irq * 2 + 1;
    io_mfence();
    high = *IOWIN;
    io_mfence();

    *IOREGSEL = 0x10 + irq * 2;
    io_mfence();
    low = *IOWIN;
    io_mfence();

    rte->vector       = low & 0xFF;
    rte->delivery_mode = (low >> 8) & 0x7;
    rte->dest_mode     = (low >> 11) & 0x1;
    rte->polarity      = (low >> 13) & 0x1;
    rte->trigger_mode  = (low >> 15) & 0x1;
    rte->mask          = (low >> 16) & 0x1;
    rte->dest_lapic_id = (high >> 24) & 0xFF;
}

void register_rte(intr_src src , uint8_t vector , uint8_t dest_lapic){
    ioapic_rte rte;
    rte.vector = vector;
    rte.delivery_mode = 0;
    rte.dest_mode = 0;
    rte.polarity = 0;
    rte.trigger_mode = 0;
    rte.mask = 0;
    rte.dest_lapic_id = dest_lapic;

    ioapic_write_rte_struct(src , &rte);

}

void init_ioapic(void) { 
    ioapic_rte rte;
    rte.vector = 0x20;
    rte.delivery_mode = 0;
    rte.dest_mode = 0;
    rte.polarity = 0;
    rte.trigger_mode = 0;
    rte.mask = 1;
    rte.dest_lapic_id = 0;

    uint8_t i = 0;
    for(; i < 24 ; i++ , rte.vector++) ioapic_write_rte_struct(i , &rte);

    // 开启IOAPIC
    io_out32(0xcf8,0x8000f8f0);
	uint32_t x = io_in32(0xcfc);

    x = x & 0xffffc000;

    uint32_t *p = ( 0xFFFFFF0001000000 + (x + (uint32_t)0x31fe) & 0x1FFFFF);

    x = (*p & 0xffffff00) | 0x100;
	io_mfence();
	*p = x;
	io_mfence();  
    
}

