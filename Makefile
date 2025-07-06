TARGET = Lib_UART_Extension_Pack
MCU_AVR_GCC ?= attiny212
MCU_AVRDUDE ?= t212
F_CPU ?= 20000000UL
DEFINES ?= USED_UNITS=8 SEND_BUF_LEN=0

PROGRAMMER ?= serialupdi
PROGRAMMER_PORT ?= /dev/tty.usbserial-1120

PROGRAMMER_PORT_EXP = $(if $(PROGRAMMER_PORT),-P $(PROGRAMMER_PORT),)

SRC_DIR = src
LIBS := $(wildcard lib/*)
BUILD_DIR = build

STATUS_TEXT_COLOR = \033[0;35m
SUCCESSFULL_TEXT_COLOR = \033[0;32m
GREY_TEXT_COLOR = \033[38;5;244m
RESET_COLOR = \033[0m

CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
NM = avr-nm
AVRDUDE = avrdude

EXTPACK_HAL_DIR := lib/ExtPack/src/HAL
EXTPACK_HAL_SRC := $(EXTPACK_HAL_DIR)/ExtPack_LL_$(MCU_AVRDUDE).c
LIB_INCLUDES := $(addprefix -I,$(addsuffix /src,$(LIBS)))
C_DEFINES = $(addprefix -D,$(DEFINES))
CFLAGS = -Wall -Os -mmcu=$(MCU_AVR_GCC) -flto -fno-fat-lto-objects -DF_CPU=$(F_CPU) $(C_DEFINES) -std=c23 -I$(SRC_DIR) $(LIB_INCLUDES) -I$(EXTPACK_HAL_DIR)
NMFLAGS = -S --size-sort -td
ASFLAGS = -mmcu=$(MCU_AVR_GCC)
LDFLAGS = -mmcu=$(MCU_AVR_GCC)

SRC = $(shell find $(SRC_DIR) -name '*.c')
ALL_LIB_SRCS := $(shell find $(addsuffix /src,$(LIBS)) -name '*.c')
LIB_SRCS := $(filter-out $(EXTPACK_HAL_DIR)/%.c,$(ALL_LIB_SRCS))
SRCS = $(SRC) $(LIB_SRCS) $(EXTPACK_HAL_SRC)

OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

all: $(BUILD_DIR)/$(TARGET).hex  $(BUILD_DIR)/$(TARGET).eep $(BUILD_DIR)/$(TARGET).lst $(BUILD_DIR)/$(TARGET).asm
	@echo "$(SUCCESSFULL_TEXT_COLOR)✅ Build abgeschlossen!$(GREY_TEXT_COLOR)"

flash: $(BUILD_DIR)/$(TARGET).hex
	@echo "$(STATUS_TEXT_COLOR)🔌 Flashe Firmware auf Mikrocontroller...$(GREY_TEXT_COLOR)"
	$(AVRDUDE) -p $(MCU_AVRDUDE) -c $(PROGRAMMER) $(PROGRAMMER_PORT_EXP) -U flash:w:$(BUILD_DIR)/$(TARGET).hex:i
	@echo "$(SUCCESSFULL_TEXT_COLOR)✅ Flashvorgang abgeschlossen.$(GREY_TEXT_COLOR)"

eeprom: $(BUILD_DIR)/$(TARGET).eep 
	@echo "$(STATUS_TEXT_COLOR)💾 Schreibe EEPROM...$(GREY_TEXT_COLOR)"
	$(AVRDUDE) -p $(MCU_AVRDUDE) -c $(PROGRAMMER) -P $(PROGRAMMER_PORT) -U eeprom:w:$(BUILD_DIR)/$(TARGET).eep:i
	@echo "$(SUCCESSFULL_TEXT_COLOR)✅ EEPROM geschrieben.$(GREY_TEXT_COLOR)"

stats: $(BUILD_DIR)/$(TARGET).elf
	@echo ""
	@echo "$(STATUS_TEXT_COLOR)📦 Größte Symbole im ELF:$(GREY_TEXT_COLOR)"
	@echo ""
	@echo "  START    SIZE  TYPE NAME"
	@$(NM) $(NMFLAGS) $(BUILD_DIR)/$(TARGET).elf
	@echo ""
	@echo "$(STATUS_TEXT_COLOR)📊 Speicherverbrauch:$(RESET_COLOR)"
	@$(OBJDUMP) -Pmem-usage $(BUILD_DIR)/$(TARGET).elf

# Create .elf file
$(BUILD_DIR)/$(TARGET).elf: $(OBJ)
	@echo "$(STATUS_TEXT_COLOR)🔧 Erstelle ELF-Datei...$(GREY_TEXT_COLOR)"
	$(CC) $(LDFLAGS) -o $@ $^

# Create .hex file (flashable)
$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	@echo "$(STATUS_TEXT_COLOR)🔧 Erstelle HEX-Datei...$(GREY_TEXT_COLOR)"
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# Create .lst file (disassembled .elf file - all sections)
$(BUILD_DIR)/$(TARGET).lst: $(BUILD_DIR)/$(TARGET).elf
	@echo "$(STATUS_TEXT_COLOR)📄 Erzeuge LST-Datei (Disassembly)...$(GREY_TEXT_COLOR)"
	$(OBJDUMP) -d $< > $@

# Create .asm file (disassembled .elf file - .text section)
$(BUILD_DIR)/$(TARGET).asm: $(BUILD_DIR)/$(TARGET).elf
	@echo "$(STATUS_TEXT_COLOR)📄 Erzeuge ASM-Datei (Disassembly)...$(GREY_TEXT_COLOR)"
	$(OBJDUMP) -S -C $< > $@

# Create .eep file (eeprom writeable)
$(BUILD_DIR)/$(TARGET).eep: $(BUILD_DIR)/$(TARGET).elf
	@echo "$(STATUS_TEXT_COLOR)📦 Erzeuge EEPROM-Datei...$(GREY_TEXT_COLOR)"
	$(OBJCOPY) -O ihex -j .eeprom $< $@

# Create .o files (compiled files)
$(BUILD_DIR)/%.o: %.c
	@echo "$(STATUS_TEXT_COLOR)📁 Erstelle Ordnerstruktur für $@... $(GREY_TEXT_COLOR)"
	mkdir -p $(dir $@)
	@echo "$(STATUS_TEXT_COLOR)🧱 Kompiliere $<...$(GREY_TEXT_COLOR)"
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "$(STATUS_TEXT_COLOR)🧹 Entferne Build-Verzeichnis...$(GREY_TEXT_COLOR)"
	rm -rf $(BUILD_DIR)
	@echo "$(SUCCESSFULL_TEXT_COLOR)✅ Clean abgeschlossen.$(GREY_TEXT_COLOR)"

.PHONY: all flash eeprom stats clean
