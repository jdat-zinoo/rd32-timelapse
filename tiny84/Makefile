# Project name and target (used to generate PROJECT/TARGET.hex)
PROJECT	= tiny84
TARGET = Release

# Programmer type and port (use -P PORT to specify port or leave empty)
PROG_TYPE = usbasp
PROG_PORT = 

# AVR type and clock frequency
MCU 	= attiny84
F_CPU 	= 1000000

# AVR fuses
LFUSE 	= 0x62
HFUSE 	= 0xDE
XFUSE 	= 0xFF

# Compiler environment
CXX = avr-g++
LD = avr-g++
OBJDUMP = avr-objdump
OBJCOPY = avr-objcopy
OBJSIZE = avr-size

# Extra includes (-Ipath) and libraries (-Lpath -llibrary)
INCLUDES = 
LIBS 	= 

CFLAGS  += -Wall
CFLAGS  += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS  += -ffunction-sections -fdata-sections
CFLAGS	+= -g -mmcu=$(MCU) -Os -DF_CPU=$(F_CPU)
#CFLAGS	+= -g -mmcu=$(MCU) -Os
LFLAGS  += -Wl,--relax,--gc-sections
LFLAGS 	+= -g -mmcu=$(MCU)

HDR = $(wildcard *.h)
SRC = $(wildcard *.cpp)
OBJ = $(SRC:%.cpp=$(TARGET)/%.o)

.PHONY: all fuse program clean

all: $(TARGET)/$(PROJECT).hex $(TARGET)/$(PROJECT).lst

$(TARGET)/$(PROJECT).hex: $(TARGET)/$(PROJECT).elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

$(TARGET)/$(PROJECT).lst: $(TARGET)/$(PROJECT).elf
	$(OBJDUMP) -h -S $< >$@

$(TARGET)/$(PROJECT).elf: $(OBJ) $(HDR) Makefile
	$(LD) -o $@ $(OBJ) $(LFLAGS) $(LIBS)
	$(OBJSIZE) $@

$(TARGET)/%.o: %.cpp
	mkdir -p $(TARGET)
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

fuse:
	avrdude -p $(MCU) -c $(PROG_TYPE) -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(XFUSE):m

program: $(TARGET)/$(PROJECT).hex
	avrdude -p $(MCU) -c $(PROG_TYPE) -U flash:w:$(TARGET)/$(PROJECT).hex

clean:
	rm -rf $(TARGET)

