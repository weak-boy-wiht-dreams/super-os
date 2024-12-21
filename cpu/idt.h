#pragma once

#include <stdint.h>

/*用于指定中断处理程序所在的段。0x08 表示使用 GDT 中内核代码段的索引*/
#define KERNEL_CS 0x08

/*中断门数组，定义 IDT 中每个条目的格式，用于存储一个中断或异常处理程序的地址和权限信息。*/
typedef struct {
    uint16_t low_offset;
    uint16_t sel; 
    uint8_t always0;
    uint8_t flags;
    uint16_t high_offset; 
} __attribute__((packed)) idt_gate_t;

/*low_offset 和 high_offset：分别表示中断处理函数的低 16 位和高 16 位的地址，合起来形成 32 位的地址，指向具体的处理函数。
sel：段选择子，通常指向内核代码段，以保证中断发生时切换到内核代码段。
flags：表示中断门的属性（如特权级、是否有效、是否为 32 位中断门等）*/

//idt的描述符，将 IDT 的地址和大小传递给 CPU，使 CPU 能够找到 IDT 的位置
typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_register_t;

/*一共256个捏*/
#define IDT_ENTRIES 256

void set_idt_gate(int n, uint32_t handler);

void load_idt();
