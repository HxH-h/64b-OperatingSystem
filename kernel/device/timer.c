# include "timer.h"
# include "device.h"
# include "../console/console.h"
# include "../global.h"
# include "../interrupt/intr.h"
# include "../interrupt/apic.h" 

// 8254定时器端口定义
# define TIMER0_PORT 0x40 
# define TIMER_CTRL_PORT 0x43
# define IRQ_FREQ 100
# define TIMER_FREQ 1193180
# define TIMER_COUNT (TIMER_FREQ / IRQ_FREQ)
# define TIMER_MODE 0x34

void timer_handler(uint8_t vector, uint64_t error_code){ 
    print("timer interrupt\n");
}

// 初始化8254定时器
void init_8254() {
    outb(TIMER_CTRL_PORT, TIMER_MODE);
    outb(TIMER0_PORT, (uint8_t)TIMER_COUNT);
    outb(TIMER0_PORT, (uint8_t)(TIMER_COUNT >> 8));
    register_handler(0x20, timer_handler); 
}

void init_timer(void){

    init_8254();
    print("init 8254 timer");

    register_rte(PIO_HPET0_TIMER0 , TIMER , 0);

    register_handler(TIMER,timer_handler);
}


