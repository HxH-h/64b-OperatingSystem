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

