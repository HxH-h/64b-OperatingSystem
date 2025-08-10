# pragma once

# include "stdtype.h"
# include <stdarg.h>


int strlen(const char *str);
void reverse(char *str, int len);
int itoa(int32_t num, char *str, int32_t base);
int ulltoa(uint64_t num, char *str, int32_t base);

int vsprintf(char *buffer, const char *format, va_list args);
