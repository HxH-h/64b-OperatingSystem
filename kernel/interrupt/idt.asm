; 统一处理有和没有ERROR_CODE时的栈
%define ERROR_CODE nop
%define NO_CODE push 0

extern IRQ_handle_table

SECTION .data

; 存储每个程序起始地址
global intr_entry_addr
intr_entry_addr:

; 中断处理程序宏定义
%macro IRQ_handler 2
intr%1entry:
        %2  ; 根据第二个参数决定是否压入0

        ; 保存上下文环境
        movq rax, es
        push rax
        movq rax, ds
        push rax
        movq rax, fs
        push rax
        movq rax, gs
        push rax
        
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

        ; 发送EOI信号，准备处理下一个中断
        movq rcx , 0x80B
        xor rax , rax
        xor rdx , rdx
        wrmsr 

        ; 调用对应的中断处理函数
        push %1
        call [rel IRQ_handle_table + %1 * 8]  ; 64位系统使用8字节指针
        add rsp, 8  ; 清理栈上的参数
        jmp intr_exit

    section .data
        dq intr%1entry  ; 存储中断入口地址

    %endmacro

intr_exit:
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
    
    pop rax
    movq gs, rax
    pop rax
    movq fs, rax
    pop rax
    movq ds, rax
    pop rax
    movq es, rax

    add rsp, 8  ; 清理栈上的错误码或0
    iretq       ; 64位中断返回

IRQ_handler 0x00, NO_CODE
IRQ_handler 0x01, NO_CODE
IRQ_handler 0x02, NO_CODE
IRQ_handler 0x03, NO_CODE
IRQ_handler 0x04, NO_CODE
IRQ_handler 0x05, NO_CODE
IRQ_handler 0x06, NO_CODE
IRQ_handler 0x07, NO_CODE
IRQ_handler 0x08, ERROR_CODE
IRQ_handler 0x09, NO_CODE
IRQ_handler 0x0A, ERROR_CODE
IRQ_handler 0x0B, ERROR_CODE
IRQ_handler 0x0C, ERROR_CODE
IRQ_handler 0x0D, ERROR_CODE
IRQ_handler 0x0E, ERROR_CODE
IRQ_handler 0x0F, NO_CODE
IRQ_handler 0x10, NO_CODE
IRQ_handler 0x11, ERROR_CODE
IRQ_handler 0x12, NO_CODE
IRQ_handler 0x13, NO_CODE
IRQ_handler 0x14, NO_CODE
IRQ_handler 0x15, NO_CODE
IRQ_handler 0x16, NO_CODE
IRQ_handler 0x17, NO_CODE
IRQ_handler 0x18, NO_CODE
IRQ_handler 0x19, NO_CODE
IRQ_handler 0x1A, NO_CODE
IRQ_handler 0x1B, NO_CODE
IRQ_handler 0x1C, NO_CODE
IRQ_handler 0x1D, NO_CODE
IRQ_handler 0x1E, NO_CODE
IRQ_handler 0x1F, NO_CODE
IRQ_handler 0x20, NO_CODE
IRQ_handler 0x21, NO_CODE
IRQ_handler 0x22, NO_CODE
IRQ_handler 0x23, NO_CODE
IRQ_handler 0x24, NO_CODE
IRQ_handler 0x25, NO_CODE
IRQ_handler 0x26, NO_CODE
IRQ_handler 0x27, NO_CODE
IRQ_handler 0x28, NO_CODE
IRQ_handler 0x29, NO_CODE
IRQ_handler 0x2a, NO_CODE
IRQ_handler 0x2b, NO_CODE
IRQ_handler 0x2c, NO_CODE
IRQ_handler 0x2d, NO_CODE
IRQ_handler 0x2e, NO_CODE
IRQ_handler 0x2f, NO_CODE