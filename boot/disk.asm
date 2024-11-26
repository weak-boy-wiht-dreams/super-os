;功能：提供加载数据到内存的低级函数。
;内容：从磁盘读取内核到指定的内存位置。
;作用：辅助引导加载器加载内核
disk_load:
    pusha         ; 保存所有寄存器的状态，防止调用此函数后破坏寄存器值
    ;这个指令会将当前 CPU 中 16 位模式下的所有通用寄存器 (AX, BX, CX, DX, SP, BP, SI, DI) 入栈，方便在函数返回时恢复
    push dx       ; 保存传入的 `dx` 值，即读取的扇区数

    mov ah, 0x02 ; 读取
    mov al, dh   ; 赋值给al
    mov cl, 0x02 ; start from sector 2
                 ; ( sector（扇区） 1 is  boot sector)
    mov ch, 0x00 ; cylinder（柱面） 0
    mov dh, 0x00 ; head 0（0磁头）


    int 0x13      ; BIOS interrupt
    jc disk_error ; check carry bit for error
;int 0x13,它在实模式下（16位）运行，允许从磁盘读取数据并将其加载到指定内存位置
    pop dx     ; get back original number of sectors 
    cmp al, dh ; BIOS sets 'al' to the # of sectors actually read
               ; 比较实际读取的扇区数和预期的 `dh` 是否一致
    jne sectors_error
    popa
    ret

disk_error:
    mov bx, DISK_ERROR
    call print16
    call print16_nl
    mov dh, ah
    call print16_hex 
    jmp disk_loop

sectors_error:
    mov bx, SECTORS_ERROR
    call print16

disk_loop:
    jmp $

DISK_ERROR: db "Disk read error", 0
SECTORS_ERROR: db "Incorrect number of sectors read", 0
