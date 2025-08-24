# pragma once

# include "./lib/stdtype.h"

# define DIV_ROUND_UP(a, b) (((a) + (b) - 1) / (b))


// 获取cpuid
static inline void cpuid(uint32_t leaf, uint32_t subleaf,
                         uint32_t *eax, uint32_t *ebx,
                         uint32_t *ecx, uint32_t *edx)
{
    uint32_t a, b, c, d;
    __asm__ volatile("cpuid"
                     : "=a"(a), "=b"(b), "=c"(c), "=d"(d)
                     : "a"(leaf), "c"(subleaf)
                     : "memory");
    if (eax) *eax = a;
    if (ebx) *ebx = b;
    if (ecx) *ecx = c;
    if (edx) *edx = d;
}

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

// 指定端口输出一个字节
static inline void outb(uint16_t port, uint8_t data){
    asm volatile ("outb %b0, %w1" : : "a"(data), "Nd"(port));
}

// 指定端口读取一个字节
static inline uint8_t inb(uint16_t port){
    uint8_t data;
    asm volatile ("inb %w1, %b0" : "=a"(data) : "Nd"(port));
    return data;
}

static inline void io_out32(uint16_t port, uint32_t val) {
    __asm__ volatile ("outl %0, %1" :: "a"(val), "Nd"(port));
}

static inline uint32_t io_in32(uint16_t port) {
    uint32_t val;
    __asm__ volatile ("inl %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

// 从指定地址输出多个字节到指定端口
static inline void outsw(uint16_t port, const void* addr, uint32_t word_cnt){
    asm volatile ("cld; rep outsw" : "+S"(addr), "+c"(word_cnt) : "d"(port));
}
// 从指定端口读取多个字节到指定内存空间
static inline void insw(uint16_t port, void* addr, uint32_t word_cnt){
    asm volatile ("cld; rep insw" : "+D"(addr), "+c"(word_cnt) : "d"(port) : "memory");
}

static inline void io_mfence(void) {
    __asm__ volatile("mfence" ::: "memory");
}
