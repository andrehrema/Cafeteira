#ifndef USART_H
#define USART_H
#define F_CPU 2000000
#define BAUDRATE 9600
#define PRESCALER ((F_CPU / (BAUDRATE * 16UL)) -1)
#include<stdint.h>
	
	
	typedef struct buffer_usart usart_buff;

	static volatile usart_buff usart_buff_rx;
	static volatile usart_buff usart_buff_tx;


	struct buffer_usart{
	
		char buffer[15];
		int8_t start;
		int8_t end;
	};

	void configure_USART(); // USART configuration
	char read_rx_buffer(); // reading data from USART
	void transmit (char value); //sending data
	uint8_t received_message();
#endif
