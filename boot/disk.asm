; 功能：提供加载数据到内存的低级函数。
; 内容：从磁盘读取内核到指定的内存位置。
; 作用：辅助引导加载器加载内核
disk_load:
    pusha         ; 保存所有寄存器的状态，防止调用此函数后破坏寄存器值。
                   ; 该指令会将当前 CPU 中 16 位模式下的所有通用寄存器 (AX, BX, CX, DX, SP, BP, SI, DI) 压入堆栈，
                   ; 方便函数返回时恢复寄存器的值。
    push dx       ; 保存传入的 `dx` 值，即读取的扇区数。

    mov ah, 0x02  ; 设置功能号为 0x02，表示读取扇区。
    mov al, dh    ; 将 dh 中存储的扇区数赋值给 al。
    mov cl, 0x02  ; 从第 2 扇区开始读取数据（第 1 扇区为引导扇区）。
    mov ch, 0x00  ; 设置柱面号为 0（cylinder 0）。
    mov dh, 0x00  ; 设置磁头号为 0（head 0）。

    int 0x13      ; 调用 BIOS 中断 0x13，用于磁盘操作。
                   ; BIOS 提供的 int 0x13 在实模式下运行，允许从磁盘读取数据并加载到指定内存位置。
    jc disk_error ; 如果读取发生错误，进位标志 (CF) 被置位，跳转到 disk_error 处理错误。

    pop dx        ; 恢复原始的扇区数值。
    cmp al, dh    ; BIOS 会将实际读取的扇区数存入 al，将其与预期的 `dh` 扇区数进行比较。
    jne sectors_error ; 如果读取的扇区数不符合预期，跳转到 sectors_error。

    popa          ; 恢复所有寄存器的状态。
    ret           ; 返回调用点。

; 错误处理：磁盘读取错误
disk_error:
    mov bx, DISK_ERROR ; 将错误信息 "Disk read error" 的地址加载到 bx。
    call print16        ; 调用 16 位模式下的打印函数，显示错误信息。
    call print16_nl     ; 输出换行符。
    mov dh, ah          ; 将错误代码（存储在 ah 中）赋值给 dh。
    call print16_hex    ; 以十六进制打印错误代码。
    jmp disk_loop       ; 无限循环，表示无法继续操作。

; 错误处理：读取的扇区数错误
sectors_error:
    mov bx, SECTORS_ERROR ; 将错误信息 "Incorrect number of sectors read" 的地址加载到 bx。
    call print16           ; 调用打印函数显示错误信息。

disk_loop:
    jmp $  ; 无限循环，等待进一步指令。

; 错误信息字符串
DISK_ERROR: db "Disk read error", 0        ; 磁盘读取错误。
SECTORS_ERROR: db "Incorrect number of sectors read", 0 ; 读取的扇区数不正确。
