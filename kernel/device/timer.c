# include "timer.h"
# include "../console/console.h"
# include "../global.h"
# include "../interrupt/intr.h"

# define CNT_REG 0x838
# define FREQ_REG 0x83E

void get_cpu_freq(uint64_t *base , uint64_t *max , uint64_t *bus){ 
    cpuid(0x16, 0 , base,max,bus,NULL);
}

void timer_handler(uint8_t vector, uint64_t error_code){ 
    print("timer interrupt\n");
}

void init_timer(void){

    wrmsr(CNT_REG,0xffff);
    wrmsr(FREQ_REG,0xA);

    wrmsr(0x832 , 0x20020);

    register_handler(TIMER,timer_handler);


}


