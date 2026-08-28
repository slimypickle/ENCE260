# =============================================================
# ENCE260 UCFK4 - Group123 - Josh Craythorne, Jamie Pedersen
# =============================================================

# ---- Toolchain / MCU ----
CC       := avr-gcc
OBJCOPY  := avr-objcopy
MCU      := atmega32u2

# ---- Flash tool selection ----
# Use DFU by default (most UCFK4 boards). Override with:
#   make program FLASH_TOOL=avrdude PROGRAMMER=usbtiny
FLASH_TOOL ?= dfu     # options: dfu | avrdude
PROGRAMMER ?= usbtiny # used if FLASH_TOOL=avrdude
PORT      ?=          # e.g., -P /dev/ttyUSB0 (often not needed for usbtiny)

# ---- Includes (relative to Group123/) ----
INCLUDES := -I. -I../../utils -I../../fonts -I../../drivers -I../../drivers/avr

# NOTE: system.h already defines F_CPU, so don't -D it here to avoid redefinition warnings.
CFLAGS   := -mmcu=$(MCU) -Os -Wall -Wstrict-prototypes -Wextra -g $(INCLUDES)
LDFLAGS  := -mmcu=$(MCU) -g -lm

# ---- App sources (in Group123/) ----
APP_SRC := \
	app_core.c \
	control_paddle.c \
	physics_ball.c

# ---- Driver sources (relative to Group123/) ----
DRV_SRC := \
	../../drivers/display.c \
	../../drivers/ledmat.c \
	../../drivers/navswitch.c \
	../../drivers/button.c \
	../../drivers/ir_serial.c \
	../../drivers/ir.c \
	../../drivers/avr/system.c \
	../../drivers/avr/timer0.c \
	../../drivers/avr/timer.c \
	../../drivers/avr/prescale.c \
	../../drivers/avr/ir_uart.c \
	../../drivers/avr/usart1.c

# ---- Utils (relative to Group123/) ----
UTIL_SRC := \
	../../utils/pacer.c \
	../../utils/tinygl.c \
	../../utils/font.c \
	../../utils/task.c

# ---- Targets & objects ----
TARGET := app.out
HEX    := app.hex
SRC    := $(APP_SRC) $(DRV_SRC) $(UTIL_SRC)
OBJ    := $(SRC:.c=.o)
DEP    := $(OBJ:.o=.d)

# ---- Phony ----
.PHONY: all program flash flash_dfu flash_avrdude clean clobber size

# Default build
all: $(TARGET)

# ---- Build rules ----
$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

# Create Intel HEX from ELF
$(HEX): $(TARGET)
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# Compile C -> OBJ (with dep gen)
%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Auto-include deps
-include $(DEP)

# ---- Flash + start on 'make program' ----
# Select the right flash target from FLASH_TOOL
ifeq ($(FLASH_TOOL),avrdude)
program: flash_avrdude
else
# default DFU bootloader
program: flash_dfu
endif

flash: $(HEX)
	@echo "Specify FLASH_TOOL=avrdude for avrdude, or use default DFU."
	@true

# DFU (common for UCFK4 with ATmega32u2 bootloader)
flash_dfu: $(HEX)
	@echo "[DFU] Put the board in DFU mode if required (hold/reset as per kit)."
	dfu-programmer $(MCU) erase
	dfu-programmer $(MCU) flash $(HEX)
	# 'start' resets and runs the new firmware
	dfu-programmer $(MCU) start

# AVRDUDE (e.g., usbtiny, avrisp2). Example PROGRAMMER=usbtiny
flash_avrdude: $(HEX)
	avrdude -p m32u2 -c $(PROGRAMMER) $(PORT) -U flash:w:$(HEX):i

# Optional: show size
size: $(TARGET)
	-@avr-size -C --mcu=$(MCU) $(TARGET) 2>/dev/null || true

# Housekeeping
clean:
	$(RM) $(OBJ) $(DEP) $(TARGET) $(HEX)

clobber:
	find . -name '*.d' -delete
	$(MAKE) clean
