#include "./random.h"

// random.c
#include "random.h"
#include "../drivers/ports.h"  // 用于读取CMOS时钟

// 线性同余生成器的参数
#define LCG_A 1664525
#define LCG_C 1013904223
#define LCG_M 0xFFFFFFFF

static uint32_t seed;

// 从CMOS时钟获取当前时间
static uint32_t get_rtc_seconds() {
    // 选择RTC寄存器 0x00 (秒)
    port_byte_out(0x70, 0x00);
    // 读取数据
    return port_byte_in(0x71);
}

// 初始化随机数种子
void init_random() {
    // 使用RTC时钟的秒数作为初始种子
    seed = get_rtc_seconds();
    // 可以混合其他时间值来增加随机性
    port_byte_out(0x70, 0x02); // 分钟
    seed ^= port_byte_in(0x71) << 8;
    port_byte_out(0x70, 0x04); // 小时
    seed ^= port_byte_in(0x71) << 16;
}

// 生成下一个随机数
static uint32_t next_random() {
    seed = (LCG_A * seed + LCG_C) & LCG_M;
    return seed;
}

// 获取0到max-1之间的随机数
uint32_t get_random(uint32_t max) {
    if (max == 0) return 0;
    return next_random() % max;
}

