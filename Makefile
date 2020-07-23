BIN = tea
KVM_BIN = tea-kvm

Q ?= @
B = build
BUILD_DIR= $(shell mkdir -p $(B); cd $(B); pwd)

BUILD_HW_DIR = $(BUILD_DIR)/hw
BUILD_KVM_DIR = $(BUILD_DIR)/kvm

TARGET = $(BUILD_HW_DIR)/$(BIN)
TARGET_KVM = $(BUILD_KVM_DIR)/$(KVM_BIN)

LDS = tea.lds

#EXLIBS = musl-libc

all: $(LDS) $(TARGET).bin $(TARGET_KVM).bin

CC = gcc
LD = ld
OBJCOPY = objcopy
NM = nm

CFLAGS = -O2 -nostdlib -fno-builtin -Wall -MMD -m32
ASFLAGS = -nostdlib -Wall -MMD -m32 -DASSEMBLER
CFLAGS_KVM = $(CFLAGS) -DKVM
ASFLAGS_KVM = $(ASFLAGS) -DKVM

LD_FLAGS = -nostdlib -T$(LDS)

ifeq ($(EXLIBS), musl-libc)
LD_FLAGS += -L./musl-libc/ -lc
endif

INCLUDES = -I./include/ \
	-I./include/libs/ \
	-I./include/interrupt/ \
	-I./include/arch/ \
	-I./include/log/ \
	-I./include/driver/

ASM_SRC += arch/head.S
ASM_SRC += interrupt/isr.S

C_SRC += init.c
C_SRC += mod_init.c
C_SRC += config.c

C_SRC += arch/lapic.c
C_SRC += arch/tsc.c
C_SRC += arch/mtrr.c

C_SRC += libs/memory.c
C_SRC += libs/string.c
C_SRC += libs/div64.c

C_SRC += log/log_utils.c
C_SRC += log/memlog.c

C_SRC += interrupt/idt.c

C_SRC += driver/uart/uart.c
C_SRC += driver/time/timer.c
C_SRC += driver/ipc/ipc.c

C_SRC += apps/demo.c
C_SRC += apps/demo_ipc.c

C_OBJS = $(patsubst %.c, $(BUILD_HW_DIR)/%.o, $(C_SRC))
ASM_OBJS = $(patsubst %.S, $(BUILD_HW_DIR)/%.o, $(ASM_SRC))

KVM_C_OBJS = $(patsubst %.c, $(BUILD_KVM_DIR)/%.o, $(C_SRC))
KVM_ASM_OBJS = $(patsubst %.S, $(BUILD_KVM_DIR)/%.o, $(ASM_SRC))

$(LDS): $(LDS).in
	@echo "Generating linker script"
	cp $< $(LDS).S
	$(Q) $(CC) -C $(ASFLAGS) $(INCLUDES) -P -E $(LDS).S -o $@

$(TARGET).bin: $(TARGET).elf
	$(Q) $(OBJCOPY) -O binary $^ $@
	$(Q) $(NM) -a $< > $(TARGET).map
	cp $@ $(shell pwd)

$(TARGET).elf: $(C_OBJS) $(ASM_OBJS)
	@ mkdir -p $(dir $@)
	@echo "Linking : $@"
	$(Q) $(LD) $^ $(LD_FLAGS) -o $@

$(BUILD_HW_DIR)/%.o: %.c
	@ mkdir -p $(dir $@)
	@echo "Compiling : $<"
	$(Q) $(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

$(BUILD_HW_DIR)/%.o: %.S
	@ mkdir -p $(dir $@)
	@echo "Compiling : $<"
	$(Q) $(CC) -c $(ASFLAGS) $(INCLUDES) $< -o $@

$(TARGET_KVM).bin: $(TARGET_KVM).elf
	$(Q) $(OBJCOPY) -O binary $^ $@
	$(Q) $(NM) -a $< > $(TARGET_KVM).map
	cp $@ $(shell pwd)

$(TARGET_KVM).elf: $(KVM_C_OBJS) $(KVM_ASM_OBJS)
	@ mkdir -p $(dir $@)
	@echo "Linking : $@"
	$(Q) $(LD) $^ $(LD_FLAGS) -o $@

$(BUILD_KVM_DIR)/%.o: %.c
	@ mkdir -p $(dir $@)
	@echo "Compiling : $<"
	$(Q) $(CC) -c $(CFLAGS_KVM) $(INCLUDES) $< -o $@

$(BUILD_KVM_DIR)/%.o: %.S
	@ mkdir -p $(dir $@)
	@echo "Compiling : $<"
	$(Q) $(CC) -c $(ASFLAGS_KVM) $(INCLUDES) $< -o $@

.PHONY: clean
clean:
	-rm -rf $(B) $(LDS) $(LDS).S *.d
