BUILD_DIR = build
INCLUDES = \
-ICMSIS/Device/ST/STM32L0xx/Include \
-ICMSIS/Include

CSOURCES = $(wildcard *.c)
ASMSOURCES = $(wildcard *.s)
CPPSOURCES = $(wildcard *.cpp)

COBJECTS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(CSOURCES))
CPPOBJECTS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(CPPSOURCES))
ASMOBJECTS = $(patsubst %.s,$(BUILD_DIR)/%.o,$(ASMSOURCES))

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
CFLAGS += -g3
CFLAGS += --specs=nosys.specs
CFLAGS += -g
CFLAGS += $(INCLUDES)


LDFLAGS += -Wl,--gc-sections
LDFLAGS += -mcpu=cortex-m0plus -mthumb
LDFLAGS += -mfloat-abi=soft # SOFT FPU
LDFLAGS += -T STM32L031K6TX_FLASH.ld
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
	$(CC) -c $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: %.cpp
	$(CPP) -c $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: %.s
	$(CC) -c $(CFLAGS) $^ -o $@

clean:
	rm build/*

stlink: $(BUILD_DIR)/firmware.bin
	@st-flash write $(BUILD_DIR)/fw.bin 0x8000000

gdb:
	@st-util

debug: $(BUILD_DIR)/firmware.elf
	@gdb \
		--symbols=$(BUILD_DIR)/firmware.elf \
		-ex 'target extended-remote localhost:4242'
