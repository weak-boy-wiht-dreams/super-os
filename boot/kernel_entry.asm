global _start;
[bits 32]

_start:
    [extern start_kernel] 
    call start_kernel ;启动内核
    jmp $