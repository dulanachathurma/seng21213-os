AS = nasm
CC = i686-elf-gcc
LD = i686-elf-ld
OBJCOPY = i686-elf-objcopy

ifeq ($(shell which $(CC)),)
    CC = gcc
    LD = ld
    CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra
else
    CFLAGS = -ffreestanding -O2 -Wall -Wextra
endif

ASFLAGS     = -f bin
ASFLAGS_ELF = -f elf
BUILD = build

all: seng21213.img

$(BUILD)/boot/boot.bin: boot/boot.asm
	@mkdir -p $(BUILD)/boot
	@echo "AS $<"
	@$(AS) $(ASFLAGS) $< -o $@

$(BUILD)/kernel.o: kernel/kernel.c
	@mkdir -p $(BUILD)
	@$(CC) $(CFLAGS) -Iinclude -c kernel/kernel.c -o $@

$(BUILD)/shell.o: kernel/shell.c
	@mkdir -p $(BUILD)
	@$(CC) $(CFLAGS) -Iinclude -c kernel/shell.c -o $@

$(BUILD)/string.o: kernel/string.c
	@mkdir -p $(BUILD)
	@$(CC) $(CFLAGS) -Iinclude -c kernel/string.c -o $@

$(BUILD)/vga.o: drivers/vga/vga.c
	@mkdir -p $(BUILD)
	@$(CC) $(CFLAGS) -Iinclude -c drivers/vga/vga.c -o $@

$(BUILD)/keyboard.o: drivers/keyboard/keyboard.c
	@mkdir -p $(BUILD)
	@$(CC) $(CFLAGS) -Iinclude -c drivers/keyboard/keyboard.c -o $@

$(BUILD)/process.o: kernel/process.c
	@mkdir -p $(BUILD)
	@$(CC) $(CFLAGS) -Iinclude -c kernel/process.c -o $@

$(BUILD)/scheduler.o: kernel/scheduler.c
	@mkdir -p $(BUILD)
	@$(CC) $(CFLAGS) -Iinclude -c kernel/scheduler.c -o $@

$(BUILD)/thread.o: kernel/thread.c
	@mkdir -p $(BUILD)
	@$(CC) $(CFLAGS) -Iinclude -c kernel/thread.c -o $@

$(BUILD)/sync.o: kernel/sync.c
	@mkdir -p $(BUILD)
	@$(CC) $(CFLAGS) -Iinclude -c kernel/sync.c -o $@

$(BUILD)/mm.o: kernel/mm.c
	@mkdir -p $(BUILD)
	@$(CC) $(CFLAGS) -Iinclude -c kernel/mm.c -o $@

$(BUILD)/pmm.o: kernel/pmm.c
	@mkdir -p $(BUILD)
	@$(CC) $(CFLAGS) -Iinclude -c kernel/pmm.c -o $@

$(BUILD)/syscall.o: kernel/syscall.c
	@mkdir -p $(BUILD)
	@$(CC) $(CFLAGS) -Iinclude -c kernel/syscall.c -o $@

$(BUILD)/switch.o: boot/switch.asm
	@mkdir -p $(BUILD)
	@$(AS) $(ASFLAGS_ELF) boot/switch.asm -o $@

$(BUILD)/isr.o: boot/isr.asm
	@mkdir -p $(BUILD)
	@$(AS) $(ASFLAGS_ELF) boot/isr.asm -o $@

KERN_OBJS = \
	$(BUILD)/kernel.o \
	$(BUILD)/shell.o \
	$(BUILD)/string.o \
	$(BUILD)/vga.o \
	$(BUILD)/keyboard.o \
	$(BUILD)/process.o \
	$(BUILD)/scheduler.o \
	$(BUILD)/thread.o \
	$(BUILD)/sync.o \
	$(BUILD)/mm.o \
	$(BUILD)/pmm.o \
	$(BUILD)/syscall.o \
	$(BUILD)/switch.o \
	$(BUILD)/isr.o

$(BUILD)/kernel.elf: $(KERN_OBJS) linker.ld
	@echo "LD $@"
	@$(LD) -m elf_i386 -T linker.ld -o $@ $(KERN_OBJS)

$(BUILD)/kernel.bin: $(BUILD)/kernel.elf
	@echo "BIN $@"
	@$(OBJCOPY) -O binary $< $@

seng21213.img: $(BUILD)/boot/boot.bin $(BUILD)/kernel.bin
	@echo "IMG $@"
	@cat $(BUILD)/boot/boot.bin $(BUILD)/kernel.bin > $@
	@echo "Disk image built successfully."

run: seng21213.img
	qemu-system-i386 -drive format=raw,file=seng21213.img

clean:
	rm -rf $(BUILD) seng21213.img

.PHONY: all run clean
