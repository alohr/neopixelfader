# use alternative core
BOARD_TAG = attiny85-16
BOARDS_TXT = $(ARDUINO_SKETCHBOOK)/hardware/attiny/boards.txt
ARDUINO_VAR_PATH = $(ARDUINO_SKETCHBOOK)/hardware/attiny/variants

# programmer
AVRDUDE = /usr/local/CrossPack-AVR/bin/avrdude
AVRDUDE_CONF = /usr/local/CrossPack-AVR/etc/avrdude.conf
AVRDUDE_OPTS = -v

ISP_PROG = avrispv2
ISP_PORT = usb

ISP_LOW_FUSE = 0xef
ISP_HIGH_FUSE = 0xdc
ISP_EXT_FUSE = 0xff

# libraries
ARDUINO_LIBS = Adafruit_NeoPixel
ARDUINO_SKETCHBOOK = $(HOME)/Arduino

include ../Arduino-Makefile/arduino-mk/Common.mk
include ../Arduino-Makefile/arduino-mk/Arduino.mk
