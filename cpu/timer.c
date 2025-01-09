#include "timer.h"
#include "../drivers/display.h"  // 用于屏幕输出
#include "../drivers/ports.h"    // 用于与硬件端口通信
#include "../kernel/util.h"      // 常用的内核辅助函数
#include "isr.h"                 // 中断服务例程

// tick 是一个全局变量，用于记录定时器的“滴答次数”，即中断发生的次数
static volatile uint32_t tick = 0;

/**
 * 每当定时器中断（PIT，Programmable Interval Timer）发生时，timer_callback 函数会被调用。
 * 该函数会增加 tick 计数，并将当前的滴答次数打印到屏幕上。
 * 
 * @param regs 中断处理时保存的寄存器状态（未使用，但需要与 ISR 的签名保持一致）。
 */
static void timer_callback(registers_t *regs) {
    tick++;  // 增加滴答计数
}

/**
 * 初始化定时器（PIT）。
 * 该函数根据指定的频率（freq）设置 PIT，计算并设置定时器的分频器值，以便定时器以指定频率触发中断。
 * 
 * @param freq 指定的频率，单位为赫兹（Hz）。
 */
void init_timer(uint32_t freq) {
    /* 将 timer_callback 函数注册为 IRQ0 的中断处理程序 */
    register_interrupt_handler(IRQ0, timer_callback);

    /* 计算分频器值。PIT 的硬件时钟频率固定为 1193180 Hz */
    uint32_t divisor = PIT_FREQUENCY / freq;
    uint8_t low  = (uint8_t)(divisor & 0xFF);      // 分频器值的低 8 位
    uint8_t high = (uint8_t)((divisor >> 8) & 0xFF); // 分频器值的高 8 位

    /* 发送命令字到 PIT 控制端口（0x43） */
    port_byte_out(0x43, 0x36); /* 设置为模式 3（方波生成器），访问模式为低字节/高字节 */

    /* 将分频器值的低 8 位发送到通道 0 的数据端口（0x40） */
    port_byte_out(0x40, low);

    /* 将分频器值的高 8 位发送到通道 0 的数据端口（0x40） */
    port_byte_out(0x40, high);
}
//计时器中断的应用：延时函数

// 移除get_ticks中的测试代码
uint32_t get_ticks() {
    return tick;
}

// 分离测试函数
void test_timer() {
    static uint32_t last_tick = 0;  // 静态变量记录上次的tick值
    uint32_t current_tick = get_ticks();
    
    // 只在tick值变化时打印
    if (current_tick != last_tick) {
        print_string("Tick: ");
        char tick_ascii[256];
        int_to_string(current_tick, tick_ascii);
        print_string(tick_ascii);
        print_nl();
        
        last_tick = current_tick;
    }
}

void delay(int milliseconds) {
    uint32_t start = get_ticks();
    uint32_t ticks_to_wait = milliseconds * TIMER_FREQ / 1000;
    
    while (get_ticks() - start < ticks_to_wait) {
        __asm__ volatile("hlt");
    }
}


