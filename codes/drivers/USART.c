#include<avr/io.h>
#include<stdlib.h>
#include"USART.h"
#include<avr/interrupt.h>

static volatile usart_buff usart_buff_rx;
static volatile usart_buff usart_buff_tx;
static volatile int semaphore;

ISR(USART_RX_vect){ // receiving ISR
        

	UCSR0B &= ~(1<<TXCIE0); //disabling TX interruption
	if (!semaphore){
	
		PORTB^=2;
		usart_buff_rx.buffer[usart_buff_rx.end] = UDR0; // storing received byte
		usart_buff_rx.end++;

		if (usart_buff_rx.end>14)
			usart_buff_rx.end = 0;
	}

        UCSR0B |= (1<<TXCIE0); //reabilitando instrução por transmissão
}


void configure_USART(){

        //one stop bit
        UBRR0H = (uint8_t) ((PRESCALER >> 8) & 0x0F); //baud rate = 9600
        UBRR0L = (uint8_t) PRESCALER; // baud rate = 9600

        UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)|(1<<TXCIE0); //enabling RX and TX, enabling RX and TX interruption
        UCSR0C |= (0x26); // even parity, 8 data bits
	
	semaphore=0;

	usart_buff_rx.start = 0; // initialiazing uart_buff_rx
	usart_buff_rx.end = 0; // initializing uart_buff_rx

	usart_buff_tx.start = 0; // initializing uart_buff_tx
	usart_buff_tx.end = 0; // initalizing uart_buff_tx
}

char read_rx_buffer(){

	semaphore = 1;
	char value = usart_buff_rx.buffer[usart_buff_rx.start]; //reading value
	usart_buff_rx.start++; // advancing on buffer
	
	if (usart_buff_rx.start > 14) // if index bigger than buffer length
		usart_buff_rx.start = 0; // index reseted

	semaphore = 0;

	return value;
}

void transmit(char value){

	UDR0 = value;
	usart_buff_rx.start++;

	if (usart_buff_rx.start > 14)
	  usart_buff_rx.start = 0;
}


uint8_t received_message(){
	int index = usart_buff_rx.end -1;
	if (index<0)
		index=0;
	if (usart_buff_rx.buffer[index] == '|')
		return 1;
	
	return 0;
}
