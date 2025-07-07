TARGET = ExtPack
MCU_AVR_GCC ?=
F_CPU ?=
DEFINES ?=
V ?= # Verbose (1 or 0)

# ------------------------------------------------------------
# Quiet/verbose switch:
#   Default: silent (only status messages are shown)
#   Verbose: make V=1
# ------------------------------------------------------------
ifeq ($(V),1)
  Q :=
else
  Q := @
endif

SRC_DIR = src
HAL_DIR = $(SRC_DIR)/HAL
BUILD_DIR = build

CC      = avr-gcc
AR      = avr-gcc-ar
RANLIB  = avr-gcc-ranlib

MKDIR_P ?= mkdir -p		# CHANGE if no unix user to something working on your system
RM_RF   ?= rm -rf		# CHANGE if no unix user to something working on your system

C_DEFINES = $(addprefix -D,$(DEFINES))
CFLAGS = -Wall -Os -mmcu=$(MCU_AVR_GCC) -flto -DF_CPU=$(F_CPU) $(C_DEFINES) -std=c23 -I$(SRC_DIR)

SRC = $(wildcard $(SRC_DIR)/*.c)
HAL = $(wildcard $(HAL_DIR)/*.c)
SRCS = $(SRC) $(HAL)

OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

all: $(BUILD_DIR)/lib$(TARGET).a
	$(info ✅ Build abgeschlossen!)

# Create .a file (static library)
$(BUILD_DIR)/lib$(TARGET).a: $(OBJ)
	$(info 📦 Erzeuge static library...)
	$(Q)$(AR) rcs $@ $^
	$(Q)$(RANLIB) $@

# Create .o files (compiled files)
$(BUILD_DIR)/%.o: %.c
	$(if $(strip $(MCU_AVR_GCC)),,$(error MCU_AVR_GCC not set! Please use "make MCU_AVR_GCC=atxxxx000 <other params> <target>" and replace atxxxx000 with your controller))
	$(if $(strip $(F_CPU)),,$(error F_CPU not set! Please use "make F_CPU=xxxxxxxUL <other params> <target>" and replace xxxxxxx with the frequency of your controller in Hz))
	$(Q)$(MKDIR_P) $(dir $@)
	$(info 🧱 Kompiliere $<...)
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(info 🧹 Entferne Build-Verzeichnis...)
	$(Q)$(RM_RF) $(BUILD_DIR)
	$(info ✅ Clean abgeschlossen)

.PHONY: all clean
