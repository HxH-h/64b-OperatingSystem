# pragma once

# include "../lib/stdtype.h"

# define SCREEN_BUFFER ((uint32_t *)0xFFFFFF0000000000);

# define WIDTH 1440
# define HEIGHT 900

# define FONT_HEIGHT 16
# define FONT_WIDTH 8

// 基本颜色定义
#define COLOR_BLACK       0x000000  // 黑色
#define COLOR_WHITE       0xFFFFFF  // 白色
#define COLOR_RED         0xFF0000  // 红色
#define COLOR_GREEN       0x00FF00  // 绿色
#define COLOR_BLUE        0x0000FF  // 蓝色
#define COLOR_YELLOW      0xFFFF00  // 黄色

typedef enum{
    LIGHT,DARK
} Appearance;

void init_console(Appearance appear);
void put_char_color(char ch , uint32_t fg_color);
void print_color(uint32_t fg_color , const char *format , ...);
void print(const char *format , ...);