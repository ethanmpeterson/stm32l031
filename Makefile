BUILD_DIR = build
INCLUDES = \
-ICMSIS/Device/ST/STM32L0xx/Include \
-ICMSIS/Include \
-Isubmodules/FreeRTOS-Kernel/include \
-Isubmodules/FreeRTOS-Kernel/portable/GCC/ARM_CM0 \
-Isubmodules/hal/include \
-Isubmodules/hal/include/dev \
-Iinclude \
-Isrc/hal \
-Isrc/dev

CSOURCES = $(wildcard *.c)
CSOURCES += $(wildcard src/*.c)
CSOURCES += $(wildcard src/hal/*.c)
CSOURCES += $(wildcard src/dev/*.c)
CSOURCES += $(wildcard micro_specific/*.c)
CSOURCES += $(wildcard submodules/hal/src/*.c)
CSOURCES += $(wildcard submodules/hal/src/dev/*.c)
CSOURCES += $(wildcard submodules/FreeRTOS-Kernel/*.c)
CSOURCES += $(wildcard submodules/FreeRTOS-Kernel/portable/GCC/ARM_CM0/*.c)
CSOURCES += submodules/FreeRTOS-Kernel/portable/MemMang/heap_4.c

ASMSOURCES = $(wildcard micro_specific/*.s)
CPPSOURCES = $(wildcard *.cpp)

COBJECTS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(CSOURCES))
CPPOBJECTS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(CPPSOURCES))
ASMOBJECTS = $(patsubst %.s,$(BUILD_DIR)/%.o,$(ASMSOURCES))

OBJDIRS := $(patsubst %, $(BUILD_DIR)/%, $(CSOURCES))
ASMOBJDIRS := $(patsubst %, $(BUILD_DIR)/%, $(ASMSOURCES))

CFLAGS += -mcpu=cortex-m0plus -mthumb
CFLAGS += -O0 # optimization off
CFLAGS += -std=gnu11 # use GNU 11 standard
CFLAGS += -mfloat-abi=soft # SOFT FPU
CFLAGS += -fno-common
CFLAGS += -Wall # turn on warnings
CFLAGS += -pedantic # more warnings
CFLAGS += -Wsign-compare
CFLAGS += -Wcast-align
CFLAGS += -Wconversion # neg int const implicitly converted to uint
CFLAGS += -fsingle-precision-constant
CFLAGS += -fomit-frame-pointer # do not use fp if not needed
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -fstack-usage
CFLAGS += -fcallgraph-info
CFLAGS += -Wstack-usage=75
CFLAGS += -g3
CFLAGS += --specs=nosys.specs
CFLAGS += -g
CFLAGS += $(INCLUDES)


LDFLAGS += -Wl,--gc-sections,--print-memory-usage
LDFLAGS += -Wl,-Map=$(BUILD_DIR)/firmware.map
LDFLAGS += -mcpu=cortex-m0plus -mthumb
LDFLAGS += -mfloat-abi=soft # SOFT FPU
LDFLAGS += -T micro_specific/STM32L031K6TX_FLASH.ld
LDFLAGS += --specs=nano.specs
# LDFLAGS += -lstdc++
LDFLAGS += -lm
LDFLAGS += $(INCLUDES)


CC = arm-none-eabi-gcc
CPP = arm-none-eabi-g++
LD = arm-none-eabi-ld

$(BUILD_DIR)/fw.bin: $(BUILD_DIR)/firmware.elf
	arm-none-eabi-objcopy -O binary $^ $@

$(BUILD_DIR)/firmware.elf: $(COBJECTS) $(CPPOBJECTS) $(ASMOBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@
	arm-none-eabi-size $@

$(BUILD_DIR)/%.o: %.c
	# create the object directories first so that we don't need to figure out flattening
	@mkdir -p $(OBJDIRS)
	$(CC) -c $(CFLAGS) $^ -o $@

# Uncomment and adjust if cpp support is needed later
# $(BUILD_DIR)/%.o: %.cpp
# 	$(CPP) -c $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(ASMOBJDIRS)
	$(CC) -c $(CFLAGS) $^ -o $@

clean:
	rm -rf build/*

stlink: $(BUILD_DIR)/firmware.bin
	@st-flash write $(BUILD_DIR)/fw.bin 0x8000000

gdb:
	@st-util

debug: $(BUILD_DIR)/firmware.elf
	@gdb \
		--symbols=$(BUILD_DIR)/firmware.elf \
		-ex 'target extended-remote localhost:4242'

linkermapviz: $(BUILD_DIR)/firmware.map
	@linkermapviz < $(BUILD_DIR)/firmware.map

