TARGET = ExtPack
MCU_AVR_GCC ?=
F_CPU ?=
DEFINES ?=
V ?= # Verbose

# ------------------------------------------------------------
# Quiet/verbose switch:
#   Default: silent (only status messages are shown)
#   Verbose: make V=1
# ------------------------------------------------------------
ifeq ($(V),1)
  Q :=
  DOXYGEN_QUIET_FLAG :=
else
  Q := @
  DOXYGEN_QUIET_FLAG := -q
endif

SRC_DIR = src
HAL_DIR = $(SRC_DIR)/ExtPack/HAL
CORE_DIR = $(SRC_DIR)/ExtPack/Core
UTIL_DIR = $(SRC_DIR)/ExtPack/Util
SERVICE_DIR = $(SRC_DIR)/ExtPack/Service
BUILD_DIR = build
DOCS_DIR = docs

# ------------------------------------------------------------
# Example build configuration
# ------------------------------------------------------------
EXAMPLES_DIR := examples
EXAMPLE_SRCS := $(wildcard $(EXAMPLES_DIR)/*.c)
EXAMPLE_OBJS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(EXAMPLE_SRCS))
EXAMPLE_ELFS := $(patsubst %.o,%.elf,$(EXAMPLE_OBJS))
EXAMPLE_HEXS := $(patsubst %.elf,%.hex,$(EXAMPLE_ELFS))

EXT_PACK_LIB := build/lib$(TARGET).a

CC      = avr-gcc
AR      = avr-gcc-ar
RANLIB  = avr-gcc-ranlib
OBJCOPY = avr-objcopy

DOXYGEN := doxygen

MKDIR_P ?= mkdir -p		# CHANGE if no unix user to something working on your system
RM_RF   ?= rm -rf		# CHANGE if no unix user to something working on your system

C_DEFINES = $(addprefix -D,$(DEFINES))
CFLAGS = -Wall -Os -mmcu=$(MCU_AVR_GCC) -flto -DF_CPU=$(F_CPU) $(C_DEFINES) -std=c23 -I$(SRC_DIR)

HAL = $(HAL_DIR)/ExtPack_LL_$(MCU_AVR_GCC).c
CORE = $(wildcard $(CORE_DIR)/*.c)
UTIL = $(wildcard $(UTIL_DIR)/*.c)
SERVICE = $(wildcard $(SERVICE_DIR)/*.c)
SRCS = $(HAL) $(CORE) $(UTIL) $(SERVICE)

OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

all: lib docs examples
	$(info ✅ All finished!)

lib: $(EXT_PACK_LIB)
	$(info ✅ Static library build finished!)

# Create .a file (static library)
$(EXT_PACK_LIB): $(OBJ)
	$(info 📦 Creating static library...)
	$(Q)$(AR) rcs $@ $^
	$(Q)$(RANLIB) $@

# Create .o files (compiled files)
$(BUILD_DIR)/%.o: %.c
	$(if $(strip $(MCU_AVR_GCC)),,$(error MCU_AVR_GCC not set! Please use "make MCU_AVR_GCC=atxxxx000 <other params> <target>" and replace atxxxx000 with your controller))
	$(if $(strip $(F_CPU)),,$(error F_CPU not set! Please use "make F_CPU=xxxxxxxUL <other params> <target>" and replace xxxxxxx with the frequency of your controller in Hz))
	$(Q)$(MKDIR_P) $(dir $@)
	$(info 🧱 Compiling $<...)
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

examples: $(EXAMPLE_HEXS)
	$(info ✅ All HEX-files of examples created!)

# Link example object files to ELF
$(BUILD_DIR)/%.elf: $(BUILD_DIR)/%.o $(EXT_PACK_LIB)
	$(info 🔧 Linking $@...)
	$(Q)$(CC) -mmcu=$(MCU_AVR_GCC) -flto -o $@ $^

# Generate HEX from ELF
$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf
	$(info 🔧 Creating HEX-file $@...)
	$(Q)$(OBJCOPY) -O ihex -R .eeprom $< $@

docs:
	$(Q)$(DOXYGEN) $(DOXYGEN_QUIET_FLAG)
	$(info ✅ Doxygen documentation generated!)

clean:
	$(info 🧹 Removing build-folder...)
	$(Q)$(RM_RF) $(BUILD_DIR)
	$(info 🧹 Removing docs-folder...)
	$(Q)$(RM_RF) $(DOCS_DIR)
	$(info ✅ Clean finished!)

.PHONY: all lib examples docs clean