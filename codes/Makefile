CC=avr-gcc

CFLAGS=-Wall -g -Os -mmcu=atmega328p

DEPS_DISPLAY=devices/DISPLAY.h
DEPS_BT=devices/BT.h

DEPS_USART=drivers/USART.h
DEPS_TIMER=drivers/TIMER.h

DISPLAY=devices/DISPLAY
BT=devices/BT

USART=drivers/USART
TIMER=drivers/TIMER

OBJ=main.o


main.hex: main.bin
	avr-objcopy -j .text -j .data -O ihex $^ $@

main.bin: $(OBJ) $(TIMER).o $(BT).o $(DISPLAY).o $(USART).o
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ): main.c 
	$(CC) $(CFLAGS) -c $< -o $@

$(DISPLAY).o: $(DISPLAY).c $(DEPS_DISPLAY)
	$(CC) $(CFLAGS) -c $< -o $@

$(USART).o: $(USART).c $(DEPS_USART)
	$(CC) $(CFLAGS) -c $< -o $@

$(BT).o: $(BT).c $(DEPS_BT)
	$(CC) $(CFLAGS) -c $< -o $@

$(TIMER).o: $(TIMER).c $(DEPS_TIMER)
	$(CC) $(CFLAGS) -c $< -o $@

clean: $(OBJ)
	find . -name '*.o' -delete
	find . -name '*.hex' -delete
	find . -name '*.bin' -delete
	find . -name '*.gch' -delete


grava: main.hex
	avrdude -p m328p -c avrispmkII -U lfuse:w:0xFF:m -U flash:w:$<:i -F -P usb
