LOADER_START_ADDRESS equ 0x900
LOADER_START_SECTOR equ 0x2
SECTOR_COUNT equ 4


section mbr vstart=0x7c00
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov ss, ax

    mov sp, 0x7c00

    ; 利用bios中断清屏
    mov ax , 0x0600
    mov bx , 0x0700
    mov cx , 0
    mov dx , 0x184f
    int 0x10
    ; 初始化光标
    mov ax , 0x0200
    mov bx , 0x0000
    mov dx , 0x0000
    int 0x10

    mov ax , 0x1301
    mov bx , 0x008f
    mov dx , 0x0000
    mov cx , 10
    mov bp , StartBoot
    int 0x10

    ; 加载loader
    mov si, packet         ; DS:SI 指向数据包
    mov ah, 0x42           ; 扩展读取功能
    mov dl, 0x80            ; 驱动器号
    int 0x13 
    

    

jmp LOADER_START_ADDRESS

StartBoot: db 'Start Boot\n'
packet:
    .size     db 16                         ; 数据包大小
    .reserved db 0                          ; 保留字段
    .count    dw SECTOR_COUNT               ; 要读取的扇区数
    .buffer   dd LOADER_START_ADDRESS       ; 数据缓冲区地址
    .lba_low  dd LOADER_START_SECTOR        ; LBA 地址低32位
    .lba_high dd 0                          ; LBA 地址高32位


times 510 - ($ - $$) db 0
dw 0xaa55

