LOADER_START_ADDRESS equ 0x900
PHY_MEM_SIZE equ 0x800
PAGE_BASE equ 0x90000


section .text vstart=LOADER_START_ADDRESS
    jmp loader_start

    StartLoader: db 'Start Loader'
    MSG_Get_Mem_OK: db 'Get Mem OK'
    MSG_Get_Mem_Fail: db 'Get Mem Fail'

    GDT_BASE:
        dq	0x0000000000000000
        dq	0x00cf9a000000ffff
    	dq	0x00cf92000000ffff
    GDT_LIMIT equ ($ - GDT_BASE) - 1
    GDT_PTR:
        dw GDT_LIMIT
        dd GDT_BASE
    
    GDT_64_BASE:
        dq	0x0000000000000000
        dq  0x0020980000000000
        dq  0x0000920000000000
        dq  0x0020f80000000000
        dq  0x0000f20000000000
    GDT_64_LIMIT equ ($ - GDT_64_BASE) - 1
    GDT_64_PTR:
        dw GDT_64_LIMIT
        dd GDT_64_BASE    

    SELECTOR_CODE equ 0x01 << 3 
    SELECTOR_DATA equ 0x02 << 3 
    SELECTOR_CODE_USER equ 0x03 << 3 | 0x03
    SELECTOR_DATA_USER equ 0x04 << 3 | 0x03

loader_start:

    ; 进入loader
    mov ax , 0x1301
    mov bx , 0x000f
    mov dx , 0x0100
    mov cx , 10
    mov bp , StartLoader
    int 0x10

    ;检测物理内存大小
    mov ebx , 0
    mov di , PHY_MEM_SIZE
    get_mem_size:
        mov	eax,	0xE820
	    mov	ecx,	20
	    mov	edx,	0x534D4150
	    int	15h
	    jc	Label_Get_Mem_Fail
	    add	di,	20

	    cmp	ebx,	0
	    jne	get_mem_size
	    jmp	Label_Get_Mem_OK

        Label_Get_Mem_Fail:
            mov ax , 0x1301
            mov bx , 0x000f
            mov dx , 0x0200
            mov cx , 10
            mov bp , MSG_Get_Mem_Fail
            int 0x10
            jmp $
        Label_Get_Mem_OK:
            mov ax , 0x1301
            mov bx , 0x000f
            mov dx , 0x0200
            mov cx , 10
            mov bp , MSG_Get_Mem_OK
            int 0x10

; 进入保护模式
    ; 打开A20
        in al, 0x92
        or al, 0x02
        out 0x92, al
    
    ; 载入GDT
        lgdt [GDT_PTR]

    ; 设置保护模式
        mov eax, cr0
        or eax, 1
        mov cr0, eax

    ; 设置段选择子
        jmp dword SELECTOR_CODE:next_step


[bits 32]
next_step:
mov ax , SELECTOR_DATA
mov ds , ax
mov es , ax
mov ss , ax
mov esp , LOADER_START_ADDRESS


; 进入IA-32e模式

; 设置页表
    ; 全局页表
    mov	dword	[PAGE_BASE],	(PAGE_BASE + 0x1000)|7
	mov	dword	[PAGE_BASE + 0x800],   (PAGE_BASE + 0x1000)|7

    ; 上级页表
    mov	dword	[PAGE_BASE + 0x1000], (PAGE_BASE + 0x2000)|7
    ; 页表
    mov	dword	[PAGE_BASE + 0x2000], 0x000083

; 重新加载GDT
lgdt [GDT_64_PTR]

; 开启PAE
mov	eax,	cr4
bts	eax,	5
mov	cr4,	eax

; 启用分页
mov	eax,	PAGE_BASE
mov	cr3,	eax

; 启用long mode
mov	ecx,	0C0000080h		;IA32_EFER
rdmsr
bts	eax,	8
wrmsr

mov	eax,	cr0
bts	eax,	0
bts	eax,	31
mov	cr0,	eax





jmp $




    









