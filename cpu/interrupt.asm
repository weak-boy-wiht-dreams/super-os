[extern isr_handler]
[extern irq_handler]

; 这个文件的主要作用是实现中断服务例程（ISR）和硬件中断请求（IRQ）的通用处理流程
; 1. 中断服务例程（ISR）部分处理 CPU 异常，包括除零错误、非法指令等。
; 2. 硬件中断请求（IRQ）部分处理外部设备的中断，例如键盘和定时器等。

; 通用 ISR 处理代码
isr_common_stub:
    ; 1. 保存 CPU 的寄存器状态
    pusha            ; 将通用寄存器的值压入堆栈
    mov ax, ds       ; 保存当前数据段选择子
    push eax         ; 将数据段选择子也压入堆栈保存
    mov ax, 0x10     ; 加载内核数据段选择子（GDT 索引为 0x10）
    mov ds, ax       ; 设置数据段寄存器
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; 2. 调用 C 中断处理程序
    push esp         ; 将寄存器结构指针压入堆栈
    call isr_handler ; 调用外部定义的中断处理函数
    pop eax          ; 恢复堆栈，清除指针

    ; 3. 恢复 CPU 的寄存器状态
    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    popa             ; 恢复通用寄存器
    add esp, 8       ; 清理堆栈中压入的错误代码和中断号
    iret             ; 返回中断，恢复 CPU 的执行状态

; 通用 IRQ 处理代码，几乎与 ISR 相同，仅 `call` 和 `pop` 操作有所不同
irq_common_stub:
    ; 1. 保存 CPU 的寄存器状态
    pusha
    mov ax, ds
    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; 2. 调用 C 中断处理程序
    push esp
    call irq_handler ; 调用外部定义的 IRQ 处理函数
    pop ebx          ; 恢复堆栈，清除指针

    ; 3. 恢复 CPU 的寄存器状态
    pop ebx
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx
    popa
    add esp, 8
    iret

; 定义全局 ISR（中断服务例程）的标签，从 ISR 0 到 ISR 31
; 每个 ISR 对应一个 CPU 异常，常见的例如除零错误（ISR0）、非法指令（ISR6）等
global isr0
global isr1
global isr2
global isr3
global isr4
global isr5
global isr6
global isr7
global isr8
global isr9
global isr10
global isr11
global isr12
global isr13
global isr14
global isr15
global isr16
global isr17
global isr18
global isr19
global isr20
global isr21
global isr22
global isr23
global isr24
global isr25
global isr26
global isr27
global isr28
global isr29
global isr30
global isr31

; ISR 的具体实现：每个 ISR 会将中断号和错误代码压入堆栈，然后跳转到通用处理代码。
; 例如，ISR 0 是除零错误，ISR 1 是调试异常等。
; 错误代码为 0 的中断将其强制设置为 0。
isr0:
    push byte 0
    push byte 0
    jmp isr_common_stub

isr1:
    push byte 0
    push byte 1
    jmp isr_common_stub

isr2:
    push byte 0
    push byte 2
    jmp isr_common_stub

isr3:
    push byte 0
    push byte 3
    jmp isr_common_stub

isr4:
    push byte 0
    push byte 4
    jmp isr_common_stub

isr5:
    push byte 0
    push byte 5
    jmp isr_common_stub

isr6:
    push byte 0
    push byte 6
    jmp isr_common_stub

isr7:
    push byte 0
    push byte 7
    jmp isr_common_stub

isr8:
    push byte 8
    jmp isr_common_stub

isr9:
    push byte 0
    push byte 9
    jmp isr_common_stub

isr10:
    push byte 10
    jmp isr_common_stub

isr11:
    push byte 11
    jmp isr_common_stub

isr12:
    push byte 12
    jmp isr_common_stub

isr13:
    push byte 13
    jmp isr_common_stub

isr14:
    push byte 14
    jmp isr_common_stub

isr15:
    push byte 0
    push byte 15
    jmp isr_common_stub

isr16:
    push byte 0
    push byte 16
    jmp isr_common_stub

isr17:
    push byte 0
    push byte 17
    jmp isr_common_stub

isr18:
    push byte 0
    push byte 18
    jmp isr_common_stub

isr19:
    push byte 0
    push byte 19
    jmp isr_common_stub

isr20:
    push byte 0
    push byte 20
    jmp isr_common_stub

isr21:
    push byte 0
    push byte 21
    jmp isr_common_stub

isr22:
    push byte 0
    push byte 22
    jmp isr_common_stub

isr23:
    push byte 0
    push byte 23
    jmp isr_common_stub

isr24:
    push byte 0
    push byte 24
    jmp isr_common_stub

isr25:
    push byte 0
    push byte 25
    jmp isr_common_stub

isr26:
    push byte 0
    push byte 26
    jmp isr_common_stub

isr27:
    push byte 0
    push byte 27
    jmp isr_common_stub

isr28:
    push byte 0
    push byte 28
    jmp isr_common_stub

isr29:
    push byte 0
    push byte 29
    jmp isr_common_stub

isr30:
    push byte 0
    push byte 30
    jmp isr_common_stub

isr31:
    push byte 0
    push byte 31
    jmp isr_common_stub

; 定义全局 IRQ（硬件中断请求）的标签，从 IRQ 0 到 IRQ 15
; 每个 IRQ 对应一个外部设备的中断，例如定时器（IRQ0）、键盘（IRQ1）等
global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15

; IRQ 的具体实现：每个 IRQ 会将设备 ID 和中断号压入堆栈，然后跳转到通用处理代码。
irq0:
    push byte 0
    push byte 32
    jmp irq_common_stub

irq1:
    push byte 1
    push byte 33
    jmp irq_common_stub

irq2:
    push byte 2
    push byte 34
    jmp irq_common_stub

irq3:
    push byte 3
    push byte 35
    jmp irq_common_stub

irq4:
    push byte 4
    push byte 36
    jmp irq_common_stub

irq5:
    push byte 5
    push byte 37
    jmp irq_common_stub

irq6:
    push byte 6
    push byte 38
    jmp irq_common_stub

irq7:
    push byte 7
    push byte 39
    jmp irq_common_stub

irq8:
    push byte 8
    push byte 40
    jmp irq_common_stub

irq9:
    push byte 9
    push byte 41
    jmp irq_common_stub

irq10:
    push byte 10
    push byte 42
    jmp irq_common_stub

irq11:
    push byte 11
    push byte 43
    jmp irq_common_stub

irq12:
    push byte 12
    push byte 44
    jmp irq_common_stub

irq13:
    push byte 13
    push byte 45
    jmp irq_common_stub

irq14:
    push byte 14
    push byte 46
    jmp irq_common_stub

irq15:
    push byte 15
    push byte 47
    jmp irq_common_stub
