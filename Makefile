# --
# Copyright (c) 2018, Lukasz Marcin Podkalicki <lpodkalicki@gmail.com>
# https://github.com/lpodkalicki/attiny-onewire-library/blob/master/examples/attiny13/Makefile
# --

MCU=attiny13
FUSE_L=0x3B
FUSE_H=0xFF
F_CPU=128000
CC=avr-gcc
LD=avr-ld
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump
SIZE=avr-size
AVRDUDE=avrdude
CFLAGS =-std=c99 -Wall -g -Os
CFLAGS+=-flto -fno-reorder-blocks
CFLAGS+=-Iinclude
CFLAGS+=-mmcu=${MCU} -DF_CPU=${F_CPU}
TEST_CFLAGS =${CFLAGS}
TEST_CFLAGS+=-I/include/simavr/avr -DTEST
TEST_CFLAGS+=-Wl,--undefined=_mmcu,--section-start=.mmcu=0x910000
TEST_CFLAGS+=-Wl,--relax,--gc-sections
TARGET=nixie

SRCS = src/main.c src/i2c.c src/rtc.c src/mcp23018.c
TESTS = test/test.c

.PHONY: all test flash fuse clean

all:
	${CC} ${CFLAGS} -o ${TARGET}.o ${SRCS}
	${LD} -o ${TARGET}.elf ${TARGET}.o
	${OBJCOPY} -j .text -j .data -O ihex ${TARGET}.o ${TARGET}.hex
	${OBJDUMP} -j .text -j .data -j .bss -d ${TARGET}.o > ${TARGET}.s
	${SIZE} -C --mcu=${MCU} ${TARGET}.elf

test:
	${CC} ${TEST_CFLAGS} -o ${TARGET}_test.o ${SRCS}
	gcc -Wall -Iinclude -I/include/simavr ${TESTS} -lsimavr -o test.o

flash:
	${AVRDUDE} -p ${MCU} -c usbasp -U flash:w:${TARGET}.hex:i -F -P usb

fuse:
	$(AVRDUDE) -p ${MCU} -c usbasp -U hfuse:w:${FUSE_H}:m -U lfuse:w:${FUSE_L}:m

clean:
	rm -f *.c~ *.o *.elf *.hex *.s
