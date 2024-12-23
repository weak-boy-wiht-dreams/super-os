[bits 32] ; 使用 32 位保护模式

; 定义常量
VIDEO_MEMORY equ 0xb8000 ; 视频内存的起始地址，VGA 文本模式的屏幕缓冲区
WHITE_ON_BLACK equ 0x0f  ; 字符颜色：白色字符，黑色背景

; 功能：在屏幕上打印字符串（32 位保护模式）
print32:
    pusha                 ; 保存所有通用寄存器的值，保护上下文
    mov edx, VIDEO_MEMORY ; 将视频内存起始地址加载到 edx

print32_loop:
    mov al, [ebx]         ; 从 ebx 指向的地址中加载字符到 al（字符串的当前字符）
    mov ah, WHITE_ON_BLACK; 设置字符的颜色属性

    cmp al, 0             ; 检查当前字符是否是字符串结束符（ASCII 码 0）
    je print32_done       ; 如果是结束符，跳转到结束标志

    mov [edx], ax         ; 将字符（al）和颜色属性（ah）写入视频内存（显存）
    add ebx, 1            ; 指针 ebx 指向字符串的下一个字符
    add edx, 2            ; 视频内存地址向后移动 2 字节（1 字符 = 2 字节）

    jmp print32_loop      ; 返回循环，处理下一个字符

print32_done:
    popa                  ; 恢复所有通用寄存器的值
    ret                   ; 返回调用点
