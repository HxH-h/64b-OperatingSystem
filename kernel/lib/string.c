# include "string.h"
# include "stdtype.h"
# include <stdarg.h>


// 字符串长度函数
int strlen(const char *str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

// 反转字符串
void reverse(char *str, int len) {
    int start = 0;
    int end = len - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

// 32位有符号整数转字符串
int itoa(int32_t num, char *str, int32_t base) {
    int32_t i = 0;
    int32_t is_negative = 0;
    
    // 处理0的特殊情况
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return i;
    }
    
    // 处理负数 (仅适用于十进制)
    if (num < 0 && base == 10) {
        is_negative = 1;
        num = -num;
    }
    
    // 处理数字
    while (num != 0) {
        int32_t rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }
    
    // 添加负号
    if (is_negative) {
        str[i++] = '-';
    }
    
    str[i] = '\0';
    
    // 反转字符串
    reverse(str, i);
    
    return i;
}

// 64位无符号整数转字符串
int ulltoa(uint64_t num, char *str, int32_t base) {
    int32_t i = 0;
    
    // 处理0的特殊情况
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return i;
    }
    
    // 处理数字
    while (num != 0) {
        uint64_t rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }
    
    str[i] = '\0';
    
    // 反转字符串
    reverse(str, i);
    
    return i;
}

// vsprintf实现
int vsprintf(char *buffer, const char *format, va_list args) {
    char *str = buffer;
    int written = 0;
    
    for (int i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%') {
            i++;
            // 处理格式说明符（简化版）
            switch (format[i]) {
                case 'c': {
                    *str++ = (char)va_arg(args, int);
                    written++;
                    break;
                }
                case 's': {
                    char *arg = va_arg(args, char*);
                    if (arg == NULL) arg = "(null)";
                    while (*arg) {
                        *str++ = *arg++;
                        written++;
                    }
                    break;
                }
                case 'd': {
                    int num = va_arg(args, int);
                    // 整数转字符串的简化实现
                    char num_str[12]; // 足够容纳32位整数
                    itoa(num, num_str, 10);
                    for (int j = 0; num_str[j]; j++) {
                        *str++ = num_str[j];
                        written++;
                    }
                    break;
                }
                case 'x':
                case 'X': {
                    unsigned int num = va_arg(args, unsigned int);
                    char num_str[12]; // 足够容纳32位无符号整数的十六进制表示
                    int len = 0;
                    int is_upper = (format[i] == 'X');
                    
                    // 处理0的特殊情况
                    if (num == 0) {
                        *str++ = '0';
                        written++;
                        break;
                    }
                    
                    // 转换为十六进制
                    while (num != 0) {
                        int rem = num % 16;
                        if (rem < 10) {
                            num_str[len++] = rem + '0';
                        } else {
                            num_str[len++] = (is_upper ? 'A' : 'a') + (rem - 10);
                        }
                        num = num / 16;
                    }
                    
                    // 反转字符串
                    for (int j = len - 1; j >= 0; j--) {
                        *str++ = num_str[j];
                        written++;
                    }
                    break;
                }
                default:
                    *str++ = format[i];
                    written++;
                    break;
            }
        } else {
            *str++ = format[i];
            written++;
        }
    }
    *str = '\0'; // 添加字符串结束符
    return written;
}