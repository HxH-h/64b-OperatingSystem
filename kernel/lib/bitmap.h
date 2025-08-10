# ifndef __LIB_BITMAP_H__
# define __LIB_BITMAP_H__ 

# include "stdint.h"

# define EMPTY 0


typedef struct{
    // 位图字节长度 B
    uint32_t bmp_bytes_len;     
    // 位图起始地址
    uint8_t* start_bits;
} Bitmap;

void bitmap_init(Bitmap* bmp, uint8_t* bmp_addr, uint32_t bmp_len);
void bitmap_set(Bitmap* bmp, uint32_t bit_idx , uint8_t value);
void bitmap_con_set(Bitmap* bmp, uint32_t bit_idx , uint32_t cnt, uint8_t value) ;
int bitmap_scan(const Bitmap* bmp, uint32_t cnt);



# endif