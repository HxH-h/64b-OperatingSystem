# pragma once

typedef enum {
    TIME_UNIT_MS, // 毫秒 (ms)
    TIME_UNIT_US, // 微秒 (us)
    TIME_UNIT_NS   // 纳秒 (ns)
} TimeUnit;



void init_timer(void);