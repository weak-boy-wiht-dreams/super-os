; 没啥作用，用来测试启动内核之前的功能。
print16:
    pusha                  ; 将所有通用寄存器（如 eax, ebx 等）压入堆栈保存当前 CPU 状态

; 开始循环输出字符串
print16_loop:
    mov al, [bx]           ; 将字符串的当前字符（由 bx 指向）加载到 al
    cmp al, 0              ; 检查该字符是否为字符串结束符（0）
    je print16_done        ; 若为结束符，跳到 print16_done 结束函数

    ; 显示当前字符
    mov ah, 0x0e           ; 设置功能号 0x0e（TTY 模式）以显示字符
    int 0x10               ; BIOS 中断调用，显示字符（字符在 al 中）

    ; 继续下一个字符
    add bx, 1              ; 指针 bx 前移，指向下一个字符
    jmp print16_loop       ; 跳回循环继续处理下一个字符

print16_done:
    popa                   ; 恢复寄存器状态
    ret                    ; 返回

; 输出换行
print16_nl:
    pusha                  ; 保存当前寄存器状态

    mov ah, 0x0e           ; 设置 TTY 显示功能
    mov al, 0x0a           ; 新行字符（换行）
    int 0x10               ; BIOS 显示中断
    mov al, 0x0d           ; 回车字符
    int 0x10               ; BIOS 显示中断

    popa                   ; 恢复寄存器状态
    ret                    ; 返回

; 清屏函数
print16_cls:
    pusha                  ; 保存当前寄存器状态

    mov ah, 0x00           ; BIOS 设置视频模式
    mov al, 0x03           ; 模式 0x03，80x25 文本模式，16 色
    int 0x10               ; BIOS 调用以切换到文本模式并清屏

    popa                   ; 恢复寄存器状态
    ret                    ; 返回

; 以十六进制格式打印 dx 中的值
; 传入值存储在 dx 中
print16_hex:
    pusha                  ; 保存当前寄存器状态

    mov cx, 0              ; 初始化索引变量 cx

; 获取 dx 中每个十六进制字符并转换为 ASCII
print16_hex_loop:
    cmp cx, 4              ; 循环 4 次，每次处理一个 4 位的十六进制字符
    je print16_hex_end     ; 完成 4 次循环后跳转到结束

    mov ax, dx             ; 使用 ax 作为工作寄存器
    and ax, 0x000f         ; 仅保留最后 4 位，例如 0x1234 -> 0x0004
    add al, 0x30           ; 转换为 ASCII，0-9 变成 '0'-'9'
    cmp al, 0x39           ; 检查是否在字符 '9' 之后
    jle print16_hex_step2  ; 如果小于等于 '9' 则跳过
    add al, 7              ; 若大于 9，则转换为 'A'-'F'，例如 'A' 是 0x41

print16_hex_step2:
    mov bx, PRINT16_HEX_OUT + 5 ; 获取十六进制字符串的基址 + 末尾位置
    sub bx, cx                  ; 计算当前字符的位置
    mov [bx], al                ; 将当前字符存入字符串

    ror dx, 4              ; 右移 4 位，处理下一个 4 位字符

    add cx, 1              ; 索引加 1
    jmp print16_hex_loop   ; 跳回循环继续处理

print16_hex_end:
    mov bx, PRINT16_HEX_OUT  ; 将十六进制字符串地址存入 bx
    call print16              ; 调用 print16 打印十六进制字符串

    popa                      ; 恢复寄存器状态
    ret                       ; 返回

; 字符串存储区
PRINT16_HEX_OUT:
    db '0x0000', 0            ; 预留内存空间，初始化十六进制字符串为 '0x0000'

