# pragma once
# include "../lib/stdtype.h"

typedef enum{
    PIO_8259A ,          // 8259A主芯片
    PIO_KEYBOARD ,              // 键盘
    PIO_HPET0_TIMER0 ,          // HPET #0、8254 定时器0
    PIO_SERIAL2_4 ,             // 串行接口2&4
    PIO_SERIAL1_3 ,             // 串行接口1&3
    PIO_PARALLEL2 ,             // 并行接口2
    PIO_FLOPPY_DRIVE ,          // 软盘驱动器
    PIO_PARALLEL1 ,             // 并行接口1
    PIO_RTC_HPET1 ,             // RTC/HPET #1
    PIO_RESERVED9 ,             // 无
    PIO_RESERVED10 ,           // 无
    PIO_HPET2 ,                // HPET #2
    PIO_HPET3_PS2_MOUSE ,      // HPET #3、鼠标（PS/2接口）
    PIO_DMA ,             // FERR#、DMA
    PIO_SATA_MASTER ,          // 主SATA接口
    PIO_SATA_SLAVE ,           // 从SATA接口
    PIO_PIRQA ,                // PIRQA#
    PIO_PIRQB ,                // PIRQB#
    PIO_PIRQC ,                // PIRQC#
    PIO_PIRQD ,                // PIRQD#
    PIO_PIRQE ,                // PIRQE#
    PIO_PIRQF ,                // PIRQF#
    PIO_PIRQG ,                // PIRQG#
    PIO_PIRQH                  // PIRQH#  
} intr_src;

void init_apic(void);
void init_ioapic(void);

void register_rte(intr_src src , uint8_t vector , uint8_t dest_lapic);











