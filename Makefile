CFLAGS := -mcmodel=large -fno-builtin -m64
ASFLAGS := --64
OUTPUT_DIR = ../output

all: $(OUTPUT_DIR)/system
	objcopy -I elf64-x86-64 -S -R ".eh_frame" -R ".comment" -O binary $(OUTPUT_DIR)/system $(OUTPUT_DIR)/kernel.bin
	dd if=$(OUTPUT_DIR)/kernel.bin of=/home/bochs/hd60.img bs=512 count=100 seek=10 conv=notrunc

$(OUTPUT_DIR)/system: $(OUTPUT_DIR)/main.o
	ld -b elf64-x86-64 -z muldefs -T kernel.lds $^ -o $@

$(OUTPUT_DIR)/main.o : kernel/main.c
	gcc $(CFLAGS) -c $< -o $@

