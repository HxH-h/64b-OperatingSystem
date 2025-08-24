CFLAGS := -mcmodel=large -fno-builtin -m64 -fno-stack-protector
ASFLAGS := -f elf64
OUTPUT_DIR = ../output

all: $(OUTPUT_DIR)/system
	objcopy -I elf64-x86-64 -S -R ".eh_frame" -R ".comment" -O binary $(OUTPUT_DIR)/system $(OUTPUT_DIR)/kernel.bin
	dd if=$(OUTPUT_DIR)/kernel.bin of=/home/bochs/hd60.img bs=512 count=100 seek=10 conv=notrunc

clean:
	rm -rf $(OUTPUT_DIR)/*

$(OUTPUT_DIR)/system: $(OUTPUT_DIR)/main.o \
					 $(OUTPUT_DIR)/memory.o	$(OUTPUT_DIR)/slab.o \
					 $(OUTPUT_DIR)/console.o $(OUTPUT_DIR)/device.o $(OUTPUT_DIR)/timer.o $(OUTPUT_DIR)/keyboard.o \
					 $(OUTPUT_DIR)/apic.o $(OUTPUT_DIR)/intr.o $(OUTPUT_DIR)/idt.o \
					$(OUTPUT_DIR)/string.o $(OUTPUT_DIR)/bitmap.o $(OUTPUT_DIR)/linkedlist.o
	ld -b elf64-x86-64 -z muldefs -T kernel.lds -Map $(OUTPUT_DIR)/kernel.map $^ -o $@


$(OUTPUT_DIR)/main.o : kernel/main.c
	gcc $(CFLAGS) -c $< -o $@

$(OUTPUT_DIR)/memory.o : kernel/memory/memory.c
	gcc $(CFLAGS) -c $< -o $@

$(OUTPUT_DIR)/slab.o : kernel/memory/slab.c
	gcc $(CFLAGS) -c $< -o $@

$(OUTPUT_DIR)/apic.o : kernel/interrupt/apic.c
	gcc $(CFLAGS) -c $< -o $@
$(OUTPUT_DIR)/intr.o : kernel/interrupt/intr.c
	gcc $(CFLAGS) -c $< -o $@
$(OUTPUT_DIR)/idt.o : kernel/interrupt/idt.asm
	nasm $(ASFLAGS) $< -o $@

$(OUTPUT_DIR)/console.o : kernel/console/console.c
	gcc $(CFLAGS) -c $< -o $@
$(OUTPUT_DIR)/device.o : kernel/device/device.c
	gcc $(CFLAGS) -c $< -o $@
$(OUTPUT_DIR)/timer.o : kernel/device/timer.c
	gcc $(CFLAGS) -c $< -o $@
$(OUTPUT_DIR)/keyboard.o : kernel/device/keyboard.c
	gcc $(CFLAGS) -c $< -o $@

$(OUTPUT_DIR)/string.o : kernel/lib/string.c
	gcc $(CFLAGS) -c $< -o $@
$(OUTPUT_DIR)/bitmap.o : kernel/lib/bitmap.c
	gcc $(CFLAGS) -c $< -o $@
$(OUTPUT_DIR)/linkedlist.o : kernel/lib/linkedlist.c
	gcc $(CFLAGS) -c $< -o $@

