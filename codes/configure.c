#include<avr/io.h>
#include<stdlib.h>
#include"BT.h"

void configure_USART(){

        //one stop bit
        UBRR0H = (uint8_t) ((PRESCALER >> 8) & 0x0F); //baud rate = 9600
        UBRR0L = (uint8_t) PRESCALER; // baud rate = 9600

        UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)|(1<<TXCIE0); //enabling RX and TX, enabling RX and TX interruption
        UCSR0C |= (0x26); // even parity, 8 data bits

}
