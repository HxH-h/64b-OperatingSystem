# pragma once 
# include "../lib/stdtype.h"

typedef void (*intr_handler)(uint8_t vector, uint64_t error_code);
typedef enum {
    TIMER = 0x20
} INTR_TYPE;

// 中断状态
typedef enum{
    INTR_OFF,
    INTR_ON
} intr_status;

void init_intr();
void register_handler(uint8_t vec_num, intr_handler handler);
void isr_dispatch(uint8_t vector, uint64_t error_code);

intr_status enable_intr();
intr_status disable_intr();