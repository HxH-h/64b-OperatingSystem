# include "keyboard.h"
# include "../global.h"
# include "../lib/stdtype.h"
# include "../console/console.h"
# include "../interrupt/intr.h"
# include "../interrupt/apic.h" 

# define KEYBOARD_PORT 0x60
# define EXT_CODE 0xE0

// 控制键 宏定义
# define SHIFT_L_MAKE 0x2A
# define SHIFT_R_MAKE 0x36
# define ALT_L_MAKE 0x38
# define ALT_R_MAKE 0xE038
# define CTRL_L_MAKE 0x1D
# define CTRL_R_MAKE 0xE01D
# define CAPS_LOCK_MAKE 0x3A

static bool ctrl_status , alt_status , shift_status , caps_status , ext_status;
static char keymap[][2] = {
    { '\0', '\0' }, // 0x00
    { '\x1B', '\x1B' }, // 0x01 - Escape
    { '1', '!' }, // 0x02
    { '2', '@' }, // 0x03
    { '3', '#' }, // 0x04
    { '4', '$' }, // 0x05
    { '5', '%' }, // 0x06
    { '6', '^' }, // 0x07
    { '7', '&' }, // 0x08
    { '8', '*' }, // 0x09
    { '9', '(' }, // 0x0A
    { '0', ')' }, // 0x0B
    { '-', '_' }, // 0x0C
    { '=', '+' }, // 0x0D
    { '\b', '\b' }, // 0x0E - Backspace
    { '\t', '\t' }, // 0x0F - Tab
    { 'q', 'Q' }, // 0x10
    { 'w', 'W' }, // 0x11
    { 'e', 'E' }, // 0x12
    { 'r', 'R' }, // 0x13
    { 't', 'T' }, // 0x14
    { 'y', 'Y' }, // 0x15
    { 'u', 'U' }, // 0x16
    { 'i', 'I' }, // 0x17
    { 'o', 'O' }, // 0x18
    { 'p', 'P' }, // 0x19
    { '[', '{' }, // 0x1A
    { ']', '}' }, // 0x1B
    { '\n', '\n' }, // 0x1C - Enter
    { '\0', '\0' }, // 0x1D - Left Control
    { 'a', 'A' }, // 0x1E
    { 's', 'S' }, // 0x1F
    { 'd', 'D' }, // 0x20
    { 'f', 'F' }, // 0x21
    { 'g', 'G' }, // 0x22
    { 'h', 'H' }, // 0x23
    { 'j', 'J' }, // 0x24
    { 'k', 'K' }, // 0x25
    { 'l', 'L' }, // 0x26
    { ';', ':' }, // 0x27
    { '\'', '"' }, // 0x28
    { '`', '~' }, // 0x29
    { '\0', '\0' }, // 0x2A - Left Shift
    { '\\', '|' }, // 0x2B
    { 'z', 'Z' }, // 0x2C
    { 'x', 'X' }, // 0x2D
    { 'c', 'C' }, // 0x2E
    { 'v', 'V' }, // 0x2F
    { 'b', 'B' }, // 0x30
    { 'n', 'N' }, // 0x31
    { 'm', 'M' }, // 0x32
    { ',', '<' }, // 0x33
    { '.', '>' }, // 0x34
    { '/', '?' }, // 0x35
    { '\0', '\0' }, // 0x36 - Right Shift
    { '*', '*' }, // 0x37 - Keypad Multiply
    { '\0', '\0' }, // 0x38 - Left Alt
    { ' ', ' ' }, // 0x39 - Spacebar
    { '\0', '\0' }  // 0x3a - Caps Lock
};

void keyboard_intr_handler(void){
    // 读取键盘码
    uint16_t code = inb(KEYBOARD_PORT);

    // 判断是否为拓展码
    if(code == EXT_CODE) {
        ext_status = true;
        return;
    }
    // 拓展码处理
    if(ext_status) {
        code = ( EXT_CODE << 8 | code);
        ext_status = false;
    }
    // 判断通码断码
    bool break_code = ((code & 0x80) != 0) ;
    // 判断哪种字符
    bool up = false;

    if(break_code) {
        code &= 0xFF7F;
        // 断码处理控制键
        if(code == CTRL_L_MAKE || code == CTRL_R_MAKE) ctrl_status = false;
        if(code == SHIFT_L_MAKE || code == SHIFT_R_MAKE) shift_status = false;
        if(code == ALT_L_MAKE || code == ALT_R_MAKE) alt_status = false;

        return ;
    } else if (code > 0x00 && code < 0x3B || code == ALT_R_MAKE || code == CTRL_R_MAKE) {
        // 通码处理
        // 字母 SHIFT和CAPS_LOCK 共同影响
        if (code >= 0x10 && code <= 0x19 ||
        code >= 0x1E && code <= 0x26 ||
        code >= 0x2C && code <= 0x32 ) up = shift_status ^ caps_status;
        else up = shift_status;

        code &= 0xFF;
        char ch = keymap[code][up];

        // 可见字符
        if (ch){
            put_char(ch);
            return ;
        }

        // 控制字符
        if(code == CTRL_L_MAKE || code == CTRL_R_MAKE) ctrl_status = true;
        else if(code == SHIFT_L_MAKE || code == SHIFT_R_MAKE) shift_status = true;
        else if(code == ALT_L_MAKE || code == ALT_R_MAKE) alt_status = true;

    } else return ;

}


void init_keyboard(){
    ctrl_status = alt_status = shift_status = caps_status = ext_status = false;
    register_rte(PIO_KEYBOARD , KEYBOARD , 0);
    register_handler(KEYBOARD, keyboard_intr_handler);
}
