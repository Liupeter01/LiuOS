ARMSTUB_NAME = armstub-new
#不添加-mgeneral-regs-only命令操作系统无法执行!!!!!
SFLAGS = -mcpu=cortex-a72 -fPIC -ffreestanding -nostdlib -nostartfiles  \
		-mgeneral-regs-only -D MODEL_4 -D AARCH_64
CFLAGS = -O0 -Wall -Wextra -g
LDFLAGS = -ffreestanding -nostdlib

ASM_FILES = $(wildcard src/kernel/*.S)
KER_C_FILES = $(wildcard src/kernel/*.c)
DRIVER_C_FILES += $(wildcard src/driver/*.c)
COMMON_C_FILES = $(wildcard src/common/*.c)

OBJ_FILES = $(ASM_FILES:src/kernel/%.S=build/%_s.o)
OBJ_FILES += $(KER_C_FILES:src/kernel/%.c=build/%_c.o) 
OBJ_FILES += $(DRIVER_C_FILES:src/driver/%.c=build/%_c.o)
OBJ_FILES += $(COMMON_C_FILES:src/common/%.c=build/%_c.o)

.PHONY: clean all build docs docs_pdf clean_docs release release_armstub
all: build

#编译汇编文件.S
build/%_s.o: src/kernel/%.S
	mkdir -p $(@D)
	$(CC) $(SFLAGS) $(CFLAGS) -Iinclude -c $< -o $@

#编译C文件.C
build/%_c.o: src/kernel/%.c
	mkdir -p $(@D)
	$(CC) $(SFLAGS) $(CFLAGS) -Iinclude -c $< -o $@

#编译C文件.C
build/%_c.o: src/common/%.c
	mkdir -p $(@D)
	$(CC) $(SFLAGS) $(CFLAGS) -Iinclude -c $< -o $@

#编译C文件.C
build/%_c.o: src/driver/%.c
	mkdir -p $(@D)
	$(CC) $(SFLAGS) $(CFLAGS) -Iinclude -c $< -o $@

#生成调试文件--手动链接文件linker.ld
build: $(OBJ_FILES)
	$(CC) -T linker.ld -o kernel8.elf $(LDFLAGS) $^
	objcopy -O binary kernel8.elf kernel8.img

#生成发行文件--手动链接文件linker.ld
release: build
	tar -czvf kernel8-v$(KERNEL_VERSION).tar.gz kernel8.img
	zip kernel8-v$(KERNEL_VERSION).zip kernel8.img

#以下内容为ARM程序标准
STUB_DIR = armstub
STUB_FILES = $(wildcard ${STUB_DIR}/$(SRC_DIR)/*.S)
OBJ_STUB_FILES = $(STUB_FILES:${STUB_DIR}/$(SRC_DIR)/%.S=${STUB_DIR}/$(BUILD_DIR)/%_s.o)

$(STUB_DIR)/$(BUILD_DIR)/%_s.o: $(STUB_FILES)
	mkdir -p $(@D)
	$(CC) $(SFLAGS) $(CFLAGS) -c $< -o $@

armstub: $(OBJ_STUB_FILES)
	@echo "----- Building armstub -----"
	ld --section-start=.text=0 -o ${STUB_DIR}/$(BUILD_DIR)/armstub.elf $<
	objcopy ${STUB_DIR}/$(BUILD_DIR)/armstub.elf -O binary $(ARMSTUB_NAME).bin

## Create compressed binary for armstub
release_armstub: armstub
	@echo "----- Releasing armstub -----"
	tar -czvf $(ARMSTUB_NAME)-v$(KERNEL_VERSION).tar.gz $(ARMSTUB_NAME).bin
	zip $(ARMSTUB_NAME)-v$(KERNEL_VERSION).zip $(ARMSTUB_NAME).bin

clean:
	rm -rf build *.img *.elf *.bin *.gz *.zip $(STUB_DIR)/build
