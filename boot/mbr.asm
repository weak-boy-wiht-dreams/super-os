[org 0x7c00]  ; 指定代码的起始地址为0x7C00，这是BIOS加载MBR的位置。
;0x7c00梦开始的地方！！！
KERNEL_OFFSET equ 0x1000 ; 内核加载的偏移地址，与链接内核时使用的地址一致

; 保存引导驱动器号到内存，BIOS在启动时将引导驱动器号存储在 'dl' 中
mov [BOOT_DRIVE], dl 
mov bp, 0x9000 ; 设置栈基地址为0x9000
mov sp, bp     ; 设置栈指针为bp

; 打印“16位模式启动”消息
mov bx, MSG_16BIT_MODE
call print16
call print16_nl

; 加载内核到内存
call load_kernel
; 切换到32位保护模式
call switch_to_32bit
; 无限循环，防止意外执行
jmp $ ; 此代码不会执行到这里

; 包含必要的外部文件
%include "boot/print-16bit.asm" ; 打印16位模式消息的实现
%include "boot/print-32bit.asm" ; 打印32位模式消息的实现
%include "boot/disk.asm"        ; 磁盘操作的实现
%include "boot/gdt.asm"         ; 全局描述符表(GDT)的实现
%include "boot/switch-to-32bit.asm" ; 切换到32位保护模式的实现

; 使用16位模式实现的内核加载逻辑
[bits 16]
load_kernel:
    ; 打印“正在加载内核”消息
    mov bx, MSG_LOAD_KERNEL
    call print16
    call print16_nl

    ; 将内核从磁盘加载到内存地址0x1000
    mov bx, KERNEL_OFFSET ; 内核加载地址
    mov dh, 31            ; 假定内核大小为31个扇区
    mov dl, [BOOT_DRIVE]  ; 使用保存的引导驱动器号
    call disk_load        ; 调用磁盘加载函数
    ret                   ; 返回

; 切换到32位保护模式后开始执行的代码
[bits 32]
BEGIN_32BIT:
    ; 打印“进入32位保护模式”消息
    mov ebx, MSG_32BIT_MODE
    call print32
    ; 将控制权移交给内核入口点
    call KERNEL_OFFSET 
    ; 如果内核返回控制权，则停留在这里
    jmp $

; 数据段
BOOT_DRIVE db 0 ; 保存引导驱动器号
MSG_16BIT_MODE db "Started in 16-bit Real Mode", 0 ; 16位模式启动消息
MSG_32BIT_MODE db "Landed in 32-bit Protected Mode", 0 ; 32位模式消息
MSG_LOAD_KERNEL db "Loading kernel into memory", 0 ; 加载内核消息

; 填充，确保引导扇区大小为512字节
times 510 - ($-$$) db 0
dw 0xaa55 ; MBR签名，标志引导扇区的末尾

