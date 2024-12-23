#pragma once

#include <stdint.h>

/* ISRs（中断服务例程）保留给 CPU 异常 */
extern void isr0();  /* 除零异常 */
extern void isr1();  /* 调试异常 */
extern void isr2();  /* 非屏蔽中断异常 */
extern void isr3();  /* 断点异常 */
extern void isr4();  /* 溢出异常 */
extern void isr5();  /* 越界异常 */
extern void isr6();  /* 无效操作码异常 */
extern void isr7();  /* 协处理器不可用异常 */
extern void isr8();  /* 双重错误异常 */
extern void isr9();  /* 协处理器段溢出异常 */
extern void isr10(); /* 无效任务状态段异常 */
extern void isr11(); /* 段不存在异常 */
extern void isr12(); /* 堆栈段错误异常 */
extern void isr13(); /* 常规保护异常 */
extern void isr14(); /* 页错误异常 */
extern void isr15(); /* 保留 */
extern void isr16(); /* 浮点异常 */
extern void isr17(); /* 对齐检查异常 */
extern void isr18(); /* 机器检查异常 */
extern void isr19(); /* 保留 */
extern void isr20(); /* 保留 */
extern void isr21(); /* 保留 */
extern void isr22(); /* 保留 */
extern void isr23(); /* 保留 */
extern void isr24(); /* 保留 */
extern void isr25(); /* 保留 */
extern void isr26(); /* 保留 */
extern void isr27(); /* 保留 */
extern void isr28(); /* 保留 */
extern void isr29(); /* 保留 */
extern void isr30(); /* 保留 */
extern void isr31(); /* 保留 */

/* IRQ 定义（硬件中断请求） */
extern void irq0();  /* 定时器中断 */
extern void irq1();  /* 键盘中断 */
extern void irq2();  /* 级联中断 */
extern void irq3();  /* 串口2中断 */
extern void irq4();  /* 串口1中断 */
extern void irq5();  /* 并口2中断 */
extern void irq6();  /* 软盘控制器中断 */
extern void irq7();  /* 并口1中断 */
extern void irq8();  /* CMOS 实时时钟中断 */
extern void irq9();  /* 可用中断 */
extern void irq10(); /* 可用中断 */
extern void irq11(); /* 可用中断 */
extern void irq12(); /* PS/2 鼠标中断 */
extern void irq13(); /* 协处理器中断 */
extern void irq14(); /* 主硬盘中断 */
extern void irq15(); /* 次硬盘中断 */

/* IRQ 中断号的常量定义 */
#define IRQ0 32  /* 定时器中断号 */
#define IRQ1 33  /* 键盘中断号 */
#define IRQ2 34  /* 级联中断号 */
#define IRQ3 35  /* 串口2中断号 */
#define IRQ4 36  /* 串口1中断号 */
#define IRQ5 37  /* 并口2中断号 */
#define IRQ6 38  /* 软盘中断号 */
#define IRQ7 39  /* 并口1中断号 */
#define IRQ8 40  /* 实时时钟中断号 */
#define IRQ9 41  /* 可用中断号 */
#define IRQ10 42 /* 可用中断号 */
#define IRQ11 43 /* 可用中断号 */
#define IRQ12 44 /* 鼠标中断号 */
#define IRQ13 45 /* 协处理器中断号 */
#define IRQ14 46 /* 主硬盘中断号 */
#define IRQ15 47 /* 次硬盘中断号 */

/* 定义结构体以聚合多个寄存器
 * 该结构体的顺序与 interrupt.asm 中 push 的顺序完全匹配：
 * - 最底部是由处理器自动压入的寄存器
 * - `push byte` 压入的错误代码和中断号
 * - `pusha` 压入的所有通用寄存器
 * - `push eax` 压入的数据段选择子
 */
typedef struct {
    uint32_t ds; /* 数据段选择子 */
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; /* 被 pusha 指令压入的通用寄存器 */
    uint32_t int_no, err_code; /* 中断号和错误代码（如果有） */
    uint32_t eip, cs, eflags, useresp, ss; /* 处理器自动压入的寄存器 */
} registers_t;

/* 初始化 ISR 的函数声明 */
void isr_install();

/* ISR 通用处理函数 */
void isr_handler(registers_t *r);

/* 中断处理函数指针类型 */
typedef void (*isr_t)(registers_t *);

/* 注册中断处理程序的函数 */
void register_interrupt_handler(uint8_t n, isr_t handler);
