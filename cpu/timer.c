#include "timer.h"
#include "../drivers/display.h"
#include "../drivers/ports.h"
#include "../kernel/util.h"
#include "isr.h"
//tick 是一个全局变量，用于记录定时器的“滴答次数”，即中断发生的次数
uint32_t tick = 0;
/*每当定时器中断（PIT）发生时，timer_callback 函数会被调用，增加 tick 计数并输出当前的滴答次数。
init_timer 函数根据给定的频率 freq 初始化 PIT，计算并设置定时器的分频器，以便以特定的频率触发中断*/
//当定时器中断（IRQ0）触发时调用这个回调函数
static void timer_callback(registers_t *regs) {
    tick++;
    print_string("Tick: ");

    char tick_ascii[256];
    int_to_string(tick, tick_ascii);
    print_string(tick_ascii);
    print_nl();
}
//将timer_callback注册为IRQ0
void init_timer(uint32_t freq) {
    /* Install the function we just wrote */
    register_interrupt_handler(IRQ0, timer_callback);

    /* Get the PIT value: hardware clock at 1193180 Hz */
    uint32_t divisor = 1193180 / freq;
    uint8_t low  = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)( (divisor >> 8) & 0xFF);
    /* Send the command */
    port_byte_out(0x43, 0x36); /* Command port */
    port_byte_out(0x40, low);
    port_byte_out(0x40, high);
}