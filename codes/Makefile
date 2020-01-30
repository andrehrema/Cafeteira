CC=avr-gcc

CFLAGS=-Wall -g -Os -mmcu=atmega328p

DEPS_DISPLAY=display.h
DEPS_BT=BT.h
DEPS_CONF=configure.h

DISPLAY=display
BT=BT
CONF=configure

OBJ=main.o


main.hex: main.bin
	avr-objcopy -j .text -j .data -O ihex $^ $@

main.bin: $(OBJ) $(BT).o $(DISPLAY).o $(CONF).o
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ): main.c 
	$(CC) $(CFLAGS) -c $< -o $@

$(DISPLAY).o: $(DISPLAY).c $(DEPS_DISPLAY)
	$(CC) $(CFLAGS) -c $< -o $@

$(BT).o: $(BT).c $(DEPS_BT)
	$(CC) $(CFLAGS) -c $< -o $@

$(CONF).o: $(CONF).c $(DEPS_CONF)
	$(CC) $(CFLAGS) -c $< -o $@

clean: $(OBJ)
	rm -f *.hex *.bin *.gch *.o

grava: main.hex
	avrdude -p m328p -c avrispmkII -U lfuse:w:0xFF:m -U flash:w:$<:i -F -P usb
