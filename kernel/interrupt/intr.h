# pragma once 


typedef void (*intr_handler)(void);
typedef enum {
    TIMER = 0x20
} INTR_TYPE;

void init_intr();
void register_handler(uint8_t vec_num, intr_handler handler);