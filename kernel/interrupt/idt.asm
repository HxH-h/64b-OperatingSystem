section .text
bits 64

%define ERROR_CODE nop
%define NO_CODE push 0

; 1. 生成256个中断入口（与之前相同）
%macro IRQ_handler 2
isr%1:
    %2
    push qword %1

    jmp isr_common_handler
%endmacro

IRQ_handler 0, NO_CODE
IRQ_handler 1, NO_CODE
IRQ_handler 2, NO_CODE
IRQ_handler 3, NO_CODE
IRQ_handler 4, NO_CODE
IRQ_handler 5, NO_CODE
IRQ_handler 6, NO_CODE
IRQ_handler 7, NO_CODE
IRQ_handler 8, ERROR_CODE
IRQ_handler 9, NO_CODE
IRQ_handler 10, ERROR_CODE
IRQ_handler 11, ERROR_CODE
IRQ_handler 12, ERROR_CODE
IRQ_handler 13, ERROR_CODE
IRQ_handler 14, ERROR_CODE
IRQ_handler 15, NO_CODE
IRQ_handler 16, NO_CODE
IRQ_handler 17, ERROR_CODE

%assign i 18
%rep 238
    IRQ_handler i , NO_CODE
%assign i i+1
%endrep

extern isr_dispatch    
isr_common_handler:

    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov rax, es
    push rax
    mov rax, ds
    push rax
    mov rax, fs
    push rax
    mov rax, gs
    push rax

    ; 调用C语言分发函数
    mov rdi, [rsp + 19*8 ]  ; vector
    mov rsi, [rsp + 19*8 + 8] ; error_code

    call isr_dispatch

    ; 恢复寄存器
    pop rax
    mov gs, rax
    pop rax
    mov fs, rax
    pop rax
    mov ds, rax
    pop rax
    mov es, rax

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    add rsp, 16
iretq

section .data
global intr_entry_addr  ; 导出数组供C语言使用
intr_entry_addr:
%assign i 0
%rep 256
    dq isr%+i  ; 每个元素是isr_i的地址（64位）
%assign i i+1
%endrep