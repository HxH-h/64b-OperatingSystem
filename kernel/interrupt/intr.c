# include "intr.h"
# include "apic.h"
# include "../console/console.h"

typedef struct __attribute__((packed)) {
    uint16_t offset_low;     // handler offset[15:0]
    uint16_t selector;       // 代码段选择子（一般是内核CS）
    uint8_t  ist;            // 仅低3位有效：IST 索引；其余必须为0
    uint8_t  type_attr;      // [7]P | [6:5]DPL | [4]=0 | [3:0]Type(0xE/0xF)
    uint16_t offset_mid;     // handler offset[31:16]
    uint32_t offset_high;    // handler offset[63:32]
    uint32_t zero;           // 保留，必须为0
} IDT_gate;

# define IDT_BASE ((IDT_gate *)0xFFFF800000010000)
# define IDT_CNT  256
# define IDT_LIMIT (sizeof(IDT_gate) * IDT_CNT - 1)

# define SELECTOR_CODE (0x01 << 3) 

# define IDT_TYPE_INTR   0xE
# define IDT_PRESENT     (1 << 7)  
# define IDT_DPL_KERNEL   0
# define IDT_DPL_USER    3

#define IDT_DPL(n)      (((n) & 0x3) << 5)

#define IDT_IST(n)      ((uint8_t)((n) & 0x7))

// 中断服务函数入口
extern void *intr_entry_addr[IDT_CNT];

// 中断处理函数
intr_handler IRQ_handle_table[IDT_CNT];

void idt_set_gate(IDT_gate *gate,
                                void *handler,
                                uint16_t selector,
                                uint8_t ist,   
                                uint8_t dpl)
{
    uint64_t addr = (uint64_t)handler;

    gate->offset_low  = (uint16_t)(addr & 0xFFFF);
    gate->selector    = selector;
    gate->ist         = IDT_IST(ist);              
    gate->type_attr   = (uint8_t)(IDT_PRESENT | IDT_DPL(dpl) | IDT_TYPE_INTR);
    gate->offset_mid  = (uint16_t)((addr >> 16) & 0xFFFF);
    gate->offset_high = (uint32_t)((addr >> 32) & 0xFFFFFFFF);
    gate->zero        = 0;
}

void init_idt(){
    uint16_t i = 0;
    for (; i < IDT_CNT; i++) {
        idt_set_gate(&IDT_BASE[i], intr_entry_addr[i], SELECTOR_CODE, 0, IDT_DPL_KERNEL);
    }
}

void init_intr(){

    // 初始化APIC硬件
    init_apic();
    print("init apic finish\n");

    // 初始化IDT
    init_idt();
    print("init idt finish\n");

}



