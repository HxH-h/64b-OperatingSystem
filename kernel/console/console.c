# include "console.h"
# include "font.h"
# include "../lib/string.h"
# include <stdarg.h>

typedef struct{
    uint16_t x;
    uint16_t y;
} Cursor;

typedef struct{
    Cursor cursor;
    uint32_t * screen_buffer;
    uint8_t print_buffer[1024];
    uint32_t bg_color;
} Console;

Console console;


void init_console(uint32_t bg_color){ 
    // 创建光标
    console.cursor.x = 0;
    console.cursor.y = 0;

    // 设置背景色
    console.bg_color = bg_color;
    console.screen_buffer = SCREEN_BUFFER;

    if(bg_color != COLOR_BLACK){
        uint32_t i = 0;
        for(i = 0; i < WIDTH * HEIGHT; i++) console.screen_buffer[i] = bg_color;
    }

}

void next_line(){
    console.cursor.x = 0;
    console.cursor.y += FONT_HEIGHT;

    // 满屏
    if(console.cursor.y >= HEIGHT){
        console.cursor.y = 0;
    }
}

void next_cursor(){
    console.cursor.x += FONT_WIDTH;
    if(console.cursor.x >= WIDTH) next_line();
}

void back_cursor(){ 
    console.cursor.x -= FONT_WIDTH;
    if(console.cursor.x < 0){
        if(console.cursor.y < FONT_HEIGHT) return;
        else{
            console.cursor.x = WIDTH - FONT_WIDTH;
            console.cursor.y -= FONT_HEIGHT;
        }
    }
    for (int y = 0; y < FONT_HEIGHT; y++) {
        for (int x = 0; x < FONT_WIDTH; x++) {
            // 计算在帧缓冲区中的位置
            uint32_t pixel_offset = (console.cursor.y + y) * WIDTH + (console.cursor.x + x);
            // 背景色
            console.screen_buffer[pixel_offset] = console.bg_color;
        }
    }
}

void put_char_color(char ch , uint32_t fg_color){
    if (ch < 0 || ch > 127) return;

    const unsigned char* font_data = font_ascii[ch];
    uint8_t x = console.cursor.x , y = console.cursor.y;

     // 绘制字符
    for (int y = 0; y < FONT_HEIGHT; y++) {
        for (int x = 0; x < FONT_WIDTH; x++) {
            // 计算在帧缓冲区中的位置
            uint32_t pixel_offset = (console.cursor.y + y) * WIDTH + (console.cursor.x + x);
            
            // 检查字体数据中该像素是否应该点亮
            if (font_data[y] & (0x80 >> x)) {
                // 前景色
                console.screen_buffer[pixel_offset] = fg_color;
            } else {
                // 背景色
                console.screen_buffer[pixel_offset] = console.bg_color;
            }
        }
    }

    // 移动光标
    next_cursor();
}

void print_color(const char *format , uint32_t fg_color , ...){ 
    
    va_list args;
    va_start(args, fg_color);
    int len = vsprintf(console.print_buffer, format, args);
    va_end(args);

    uint32_t i = 0;
    for(i = 0; i < len; i++){
        if(console.print_buffer[i] == '\n') next_line();
        else if(console.print_buffer[i] == '\b') back_cursor();
        else put_char_color(console.print_buffer[i] , fg_color);
    }
  

}


