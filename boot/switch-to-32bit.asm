[bits 16]  
switch_to_32bit:  
    cli                     ; 1. 禁用中断，防止在切换模式过程中出现中断请求
    lgdt [gdt_descriptor]   ; 2. 加载 GDT 描述符到 GDTR 寄存器，准备好全局描述符表
  
    mov eax, cr0            ; 3. 将控制寄存器 CR0 的值加载到 EAX 寄存器
    or eax, 0x1             ; 4. 将 EAX 寄存器的第 0 位设置为 1，用于启用保护模式
    mov cr0, eax            ; 5. 将修改后的值写回 CR0，正式启用保护模式
  
    jmp CODE_SEG:init_32bit ; 6. 通过远跳转 (far jump) 进入 32 位代码段（段寄存器 CODE_SEG 中的基地址）

[bits 32]  
init_32bit:  
    mov ax, DATA_SEG        ; 7. 将数据段选择子加载到 AX 寄存器，更新数据段寄存器
    mov ds, ax              ; 将 DS 数据段寄存器指向 DATA_SEG，启用数据段
    mov ss, ax              ; 将 SS 堆栈段寄存器指向 DATA_SEG，启用堆栈段
    mov es, ax              ; 将 ES 附加段寄存器指向 DATA_SEG，启用附加段
    mov fs, ax              ; 将 FS 寄存器指向 DATA_SEG
    mov gs, ax              ; 将 GS 寄存器指向 DATA_SEG

    mov ebp, 0x90000        ; 8. 设置堆栈指针寄存器 EBP，将堆栈顶位置设置为地址 0x90000
    mov esp, ebp            ; 9. 同时将 ESP 设置为 EBP 指向的值，初始化栈指针

    call BEGIN_32BIT        ; 10. 调用 BEGIN_32BIT 标签，返回到 mbr.asm 中继续执行
