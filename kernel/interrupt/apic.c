# include "apic.h"
# include "global.h"
# include "../console/console.h"

# define IA32_APIC_BASE 0x1B
# define LAPIC_TPR 0x808
# define LAPIC 0x80F
# define LAPIC_EOI 0x80B


# define APIC_ENABLE (1 << 11)
# define x2APIC_ENABLE (1 << 10)
#define LAPIC_ENABLE  (1 << 8)

// 读写MSR
static inline uint64_t rdmsr(uint32_t msr) {
    uint32_t lo, hi;
    __asm__ volatile ("rdmsr" : "=a"(lo), "=d"(hi) : "c"(msr));
    return ((uint64_t)hi << 32) | lo;
}

static inline void wrmsr(uint32_t msr, uint64_t val) {
    uint32_t lo = val & 0xFFFFFFFF;
    uint32_t hi = val >> 32;
    __asm__ volatile ("wrmsr" : : "c"(msr), "a"(lo), "d"(hi));
}


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

}

