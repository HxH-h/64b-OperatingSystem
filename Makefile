CFLAGS := -mcmodel=large -fno-builtin -m64 -fno-stack-protector
ASFLAGS := --64
OUTPUT_DIR = ../output

all: $(OUTPUT_DIR)/system
	objcopy -I elf64-x86-64 -S -R ".eh_frame" -R ".comment" -O binary $(OUTPUT_DIR)/system $(OUTPUT_DIR)/kernel.bin
	dd if=$(OUTPUT_DIR)/kernel.bin of=/home/bochs/hd60.img bs=512 count=100 seek=10 conv=notrunc

clean:
	rm -rf $(OUTPUT_DIR)/*

$(OUTPUT_DIR)/system: $(OUTPUT_DIR)/main.o $(OUTPUT_DIR)/console.o $(OUTPUT_DIR)/string.o $(OUTPUT_DIR)/memory.o
	ld -b elf64-x86-64 -z muldefs -T kernel.lds -Map $(OUTPUT_DIR)/kernel.map $^ -o $@



$(OUTPUT_DIR)/main.o : kernel/main.c
	gcc $(CFLAGS) -c $< -o $@

$(OUTPUT_DIR)/console.o : kernel/console/console.c
	gcc $(CFLAGS) -c $< -o $@

$(OUTPUT_DIR)/memory.o : kernel/memory/memory.c
	gcc $(CFLAGS) -c $< -o $@

$(OUTPUT_DIR)/string.o : kernel/lib/string.c
	gcc $(CFLAGS) -c $< -o $@

