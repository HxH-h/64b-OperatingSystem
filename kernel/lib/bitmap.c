# include "bitmap.h"


// 位图结构体 + 位图存储的地址 + 位图长度
void bitmap_init(Bitmap* bmp, uint8_t* bmp_addr, uint32_t bmp_len) {
    bmp->bmp_bytes_len = bmp_len;
    bmp->start_bits = bmp_addr;
    // 初始化位图
    uint8_t *start = bmp->start_bits;
    uint32_t i = 0; 
    for (; i < bmp_len; i++) start[i] = 0;
}


// 设置位图指定位
void bitmap_set(Bitmap* bmp, uint32_t bit_idx , uint8_t value) { 

    uint32_t byte_idx = bit_idx / 8;
    uint8_t idx_in_byte = bit_idx % 8;

    if (value == 0) bmp->start_bits[byte_idx] &= ~(1 << idx_in_byte);
    else bmp->start_bits[byte_idx] |= (1 << idx_in_byte);
}

void bitmap_con_set(Bitmap* bmp, uint32_t bit_idx , uint32_t cnt, uint8_t value) { 
    if(bit_idx < 0 || bit_idx + cnt > bmp->bmp_bytes_len * 8) return;
    int i = 0;
    if(!value){
        for (; i < cnt; i++) {
            bmp->start_bits[(bit_idx + i) / 8] &= ~(1 << ((bit_idx + i) % 8));
        }
    }else{
        for (; i < cnt; i++) {
            bmp->start_bits[(bit_idx + i) / 8] |= (1 << ((bit_idx + i) % 8));
        }
    }
    
}

// 扫描位图
// cnt: 需要连续的位数
// 返回值：连续位的起始索引
int bitmap_scan(const Bitmap* bmp, uint32_t cnt) {
    if (!bmp || cnt == 0) return -1;

    const uint32_t total_bits = bmp->bmp_bytes_len * 8;
    uint32_t zero_count = 0;
    uint32_t start_bit = 0;

    for (uint32_t byte_i = 0; byte_i < bmp->bmp_bytes_len; ++byte_i) {
        uint8_t byte = bmp->start_bits[byte_i];

        // 如果整字节都是1，跳过整个字节
        if (byte == 0xFF) {
            zero_count = 0;
            continue;
        }

        // 检查这个字节中的每一位
        for (uint8_t bit = 0; bit < 8; ++bit) {
            uint32_t bit_index = byte_i * 8 + bit;
            if (bit_index >= total_bits) break;

            if (((byte >> bit) & 1) == 0) {
                if (zero_count == 0) start_bit = bit_index;
                
                zero_count++;
                if (zero_count == cnt) return start_bit;
                
            } else zero_count = 0;
        }
    }
    
    return -1; // 未找到

}