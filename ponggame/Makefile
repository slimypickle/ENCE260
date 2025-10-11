# Makefile — UCFK4 Pong (modular)
# Authors: Josh Craythorne (usercode); Jamie Pedersen (usercode)
# Date: 2025-10-06

CC = avr-gcc
CFLAGS = -mmcu=atmega32u2 -Os -Wall -Wstrict-prototypes -Wextra -g \
         -I. -I../../utils -I../../fonts -I../../drivers -I../../drivers/avr

OBJCOPY = avr-objcopy
SIZE = avr-size
DEL = rm

# Application modules (no subdirectories as per brief).
APP_OBJS = \
    game.o \
    game_model.o \
    game_input.o \
    game_physics.o \
    game_ir.o \
    game_render.o

# Repository drivers/utils.
DRIVER_OBJS = \
    system.o \
    pacer.o \
    tinygl.o \
    font.o \
    display.o \
    ledmat.o \
    navswitch.o \
    button.o \
    ir_serial.o \
    ir.o \
    timer0.o \
    timer.o \
    prescale.o

OBJS = $(APP_OBJS) $(DRIVER_OBJS)

all: game.out

# Application objects
game.o: game.c game.h game_model.h game_input.h game_physics.h game_ir.h game_render.h
	$(CC) -c $(CFLAGS) $< -o $@

game_model.o: game_model.c game_model.h game.h
	$(CC) -c $(CFLAGS) $< -o $@

game_input.o: game_input.c game_input.h game_model.h game.h
	$(CC) -c $(CFLAGS) $< -o $@

game_physics.o: game_physics.c game_physics.h game_model.h game_ir.h game.h
	$(CC) -c $(CFLAGS) $< -o $@

game_ir.o: game_ir.c game_ir.h game_model.h game.h
	$(CC) -c $(CFLAGS) $< -o $@

game_render.o: game_render.c game_render.h game_model.h game.h
	$(CC) -c $(CFLAGS) $< -o $@

# Drivers/Utils (corrected paths)
system.o: ../../drivers/avr/system.c ../../drivers/avr/system.h
	$(CC) -c $(CFLAGS) $< -o $@

pacer.o: ../../utils/pacer.c ../../utils/pacer.h
	$(CC) -c $(CFLAGS) $< -o $@

tinygl.o: ../../utils/tinygl.c ../../utils/tinygl.h ../../drivers/display.h ../../utils/font.h
	$(CC) -c $(CFLAGS) $< -o $@

font.o: ../../utils/font.c ../../utils/font.h
	$(CC) -c $(CFLAGS) $< -o $@

display.o: ../../drivers/display.c ../../drivers/display.h ../../drivers/ledmat.h
	$(CC) -c $(CFLAGS) $< -o $@

ledmat.o: ../../drivers/ledmat.c ../../drivers/ledmat.h
	$(CC) -c $(CFLAGS) $< -o $@

navswitch.o: ../../drivers/navswitch.c ../../drivers/navswitch.h
	$(CC) -c $(CFLAGS) $< -o $@

button.o: ../../drivers/button.c ../../drivers/button.h
	$(CC) -c $(CFLAGS) $< -o $@

ir_serial.o: ../../drivers/ir_serial.c ../../drivers/ir_serial.h ../../drivers/ir.h
	$(CC) -c $(CFLAGS) $< -o $@

ir.o: ../../drivers/ir.c ../../drivers/ir.h ../../drivers/avr/timer0.h
	$(CC) -c $(CFLAGS) $< -o $@

timer0.o: ../../drivers/avr/timer0.c ../../drivers/avr/timer0.h ../../drivers/avr/prescale.h
	$(CC) -c $(CFLAGS) $< -o $@

prescale.o: ../../drivers/avr/prescale.c ../../drivers/avr/prescale.h
	$(CC) -c $(CFLAGS) $< -o $@

timer.o: ../../drivers/avr/timer.c ../../drivers/avr/timer.h
	$(CC) -c $(CFLAGS) $< -o $@

# Link
game.out: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -lm
	$(SIZE) $@

# Clean
.PHONY: clean
clean:
	-$(DEL) *.o *.out *.hex

# Program (dfu)
.PHONY: program
program: game.out
	$(OBJCOPY) -O ihex game.out game.hex
	dfu-programmer atmega32u2 erase; dfu-programmer atmega32u2 flash game.hex; dfu-programmer atmega32u2 start
