# include "timer.h"
# include "device.h"
# include "../console/console.h"
# include "../global.h"
# include "../interrupt/intr.h"
# include "../interrupt/apic.h" 

# define TIMER_BASE (0xFFFFFF0001100000)

#define GCAP_ID     (TIMER_BASE + 0x000)  // 000h ~ 007h
#define GEN_CONF    (TIMER_BASE + 0x010)  // 010h ~ 017h
#define GINTR_STA   (TIMER_BASE + 0x020)  // 020h ~ 027h
#define MAIN_CNT    (TIMER_BASE + 0x0F0)  // 0F0h ~ 0F7h

#define HPET_TIMER_CONFIG(n)    (TIMER_BASE + 0x100 + (n) * 0x20)
#define HPET_TIMER_COMP(n)    (TIMER_BASE + 0x108 + (n) * 0x20)

typedef struct {
    uint8_t trigger_mode     ; 
    uint8_t timer_type       ; 
    uint8_t irq_route        ; 
    uint8_t msg_enable       ;
} timer_config;


uint32_t time_precision ;


void timer_handler(uint8_t vector, uint64_t error_code){ 
    print("timer interrupt\n");
}

uint64_t cal_timer_ticks(uint32_t time, TimeUnit unit) {
    if (time_precision == 0) return 0;

    uint64_t desired_period_fs; // 将期望周期统一转换为飞秒

    switch (unit) {
        case TIME_UNIT_MS:
            // 1 ms = 1,000,000,000,000 fs
            desired_period_fs = (uint64_t)time * 1000000000000ULL;
            break;
        case TIME_UNIT_US:
            // 1 us = 1,000,000,000 fs
            desired_period_fs = (uint64_t)time * 1000000000ULL;
            break;
        case TIME_UNIT_NS:
            // 1 ns = 1,000,000 fs
            desired_period_fs = (uint64_t)time * 1000000ULL;
            break;
        default:
            return 0; // 不支持的单位
    }

    // 计算计数值 = 总的飞秒数 / 每个tick的飞秒数
    uint64_t ticks = desired_period_fs / time_precision;

    return ticks;
}

void init_timer_config(uint8_t timer_n , timer_config *config , uint32_t time , TimeUnit unit){

    uint32_t val = 0;

    val |= config->trigger_mode << 1;
    val |= 1 << 2;
    val |= config->timer_type << 3;

    if(config->timer_type) val |= 1 << 6;

    val |= config->irq_route << 9;
    val |= config->msg_enable << 14;

    uint64_t ticks = cal_timer_ticks(time, unit);
    print("ticks: %x\n", ticks);

    uint32_t *cnf = (uint32_t *)HPET_TIMER_CONFIG(timer_n);
   
    io_mfence();
    *cnf = val;
    io_mfence();

    uint64_t *cnt_reg = (uint64_t *)HPET_TIMER_COMP(timer_n);
   
    io_mfence();
    *cnt_reg = ticks;
    io_mfence();

}


void init_hpet(void){
    io_out32(0xcf8,0x8000f8f0);
    io_mfence();
	uint32_t x = io_in32(0xcfc);
    io_mfence();
    x = x & 0xffffc000;

    uint32_t *p = ( 0xFFFFFF0001000000 + (x + 0x3404UL) & 0x1FFFFF);

	io_mfence();
	*p = 0x80;
	io_mfence();

    time_precision = *(uint64_t *)(GCAP_ID) >> 32;
    print("time precision: %x\n", time_precision);

    // 初始化timer0
    timer_config timer0;
    timer0.trigger_mode = 0;
    timer0.timer_type = 1;
    timer0.msg_enable = 0;
    timer0.irq_route = 0;

    init_timer_config(0 , &timer0 , 1000 , TIME_UNIT_MS);

    // 启动主计数器
    uint64_t *main_cnt = (uint64_t *)MAIN_CNT;
    io_mfence();
    *main_cnt = 0;
    io_mfence();

    uint64_t *gen_conf = (uint64_t *)GEN_CONF;
    io_mfence();
    *gen_conf = 0x3;
    io_mfence();

}

void init_timer(void){

    init_hpet();
    print("init HPET\n");

    register_rte(PIO_HPET0_TIMER0 , TIMER , 0);

    register_handler(TIMER,timer_handler);
}


