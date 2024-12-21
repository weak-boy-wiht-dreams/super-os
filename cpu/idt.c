#include "idt.h"
#include "../kernel/util.h"

// 定义中断描述符表（IDT）和 IDT 寄存器（IDT 注册结构）
idt_gate_t idt[IDT_ENTRIES];   // 中断描述符表，包含所有中断门
idt_register_t idt_reg;        // IDT 描述符结构，包含 IDT 的地址和大小

// 设置 IDT 门函数。可以认为是一种赋值
void set_idt_gate(int n, uint32_t handler) {
    idt[n].low_offset = low_16(handler); // 设置处理程序的低 16 位地址
    idt[n].sel = KERNEL_CS;              // 设置内核代码段选择子
    idt[n].always0 = 0;                  // 始终为 0，用于填充
    idt[n].flags = 0x8E;                 // 设置标志位：P=1（存在），DPL=00（内核级），类型位表瑟提——示中断门
    idt[n].high_offset = high_16(handler); // 设置处理程序的高 16 位地址
}

// 加载 IDT 到 CPU 中的 lidt 函数，可以认为是一种赋值
void load_idt() {
    // 设置 idt_reg 的基地址和大小
    idt_reg.base = (uint32_t) &idt; // 将 IDT 表的基地址赋给 idt_reg.base
    idt_reg.limit = IDT_ENTRIES * sizeof(idt_gate_t) - 1; // 设置 IDT 的大小

    // 加载 IDT 寄存器，使用 IDT 描述符结构地址加载到 CPU 的 IDT
    asm volatile("lidt (%0)" : : "r" (&idt_reg)); 
}
