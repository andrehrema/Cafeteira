CC=avr-gcc

CFLAGS=-Wall -g -Os -mmcu=atmega328p

DEPS_DISPLAY=display7.h
DEPS_BT=BT.h

DISPLAY=display7
BT=BT

OBJ=main.o


main.hex: main.bin
	avr-objcopy -j .text -j .data -O ihex $^ $@

main.bin: $(OBJ) $(BT).o $(DISPLAY).o 
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ): main.c 
	$(CC) $(CFLAGS) -c $< -o $@

$(DISPLAY).o: $(DISPLAY).c $(DEPS_DISPLAY)
	$(CC) $(CFLAGS) -c $< -o $@

$(BT).o: $(BT).c $(DEPS_BT)
	$(CC) $(CFLAGS) -c $< -o $@

clean: $(OBJ)
	rm -f *.hex *.bin *.gch *.o

grava: main.hex
	avrdude -p m328p -c avrispmkII -U lfuse:w:0xFF:m -U flash:w:$<:i -F -P usb
