#include<avr/io.h> //#include<avr/common.h> //registradores comuns aos microcontroladores AVR, incluido pelo avr/io.h. Esse header vai identificar qual o uC pela definição do mmcu e irá incluir o header adequado. O 
#include<avr/interrupt.h> // header de interrupções
#include<avr/sleep.h> // header referente ao dormir
#include<avr/fuse.h>

#include<time.h> // header com as referências de tempo (ano, mês, dia, hora, ...)
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>

#include"drivers/USART.h"
#include"drivers/TIMER.h"
#include"devices/DISPLAY.h"
#include"devices/BT.h"


void blink_led();
extern uint16_t counter;
extern volatile usart_buff usart_buff_rx;
extern volatile usart_buff usart_buff_tx;

int main (void){

	display_queue *f_control = malloc(sizeof(display_queue));
	f_control->hd = 0;
	f_control->tl = 0;

	//display_7seg d; //cursor positioning
	
	CLKPR = (1<<CLKPCE); // enabling bit writing in clock divider
 	CLKPR = 2; //clock divider = 4, from 8 MHz to 2 MHZ

	DDRB = 6; //PB2 and PB1 = output
	DDRC = 63; // PC[0,5] as output
	DDRD = 4; //PD2 as output


	configure_USART(); //USART's configuration function

	char time[33]; //time string

	struct tm local_time; //local_time
	local_time.tm_sec = 0;
       	local_time.tm_min = 0;
	local_time.tm_hour = 15;
	local_time.tm_mday = 11;
	local_time.tm_mon = 11;
	local_time.tm_year = 1995;
	
	struct tm target_time; //target_time
	
	target_time.tm_sec = 0;
       	target_time.tm_min = 0;
	target_time.tm_hour = 0;
	target_time.tm_mday = 1;
	target_time.tm_mon = 0;
	target_time.tm_year = 2020;

	int index = -1;
	
	int i=1;
	int j=0;
	
	while(i<2){

		sei(); // enabling interruption 
		set_sleep_mode(SLEEP_MODE_IDLE); //idle mode
		sleep_mode(); //sleep
				
		/*switch(verifica_BT_rx(buffer_rx_bth)){
			case REPLY_OK: //teve um OK do bluetooth, então pode tirar a instrucao da fila de transmissao e recepcao
			
				buffer_rx_bth = remove_inst_BT(buffer_rx_bth); //limpando a fila de recepcao

				UCSR0B |= 0x40; //habilitando interrupção por transmissão
				i++;
				UDR0 = comandos_conf_BT[i];
				break;

		
			case REPLY_NOK: //teve uma resposta final, mas não foi OK, precisa enviar os dados novamente
		
				buffer_rx_bth = remove_inst_BT(buffer_rx_bth); //apagando a resposta
				UCSR0B |= 0x40; //habilitando interrupcao por transmissao
				
				//ponteiro = buffer_tx_bth->hd; //reinciando o envio da instrução
				break;

		}

		if ( (UCSR0A & (1<<UDRE0)) ){
			if (comandos_conf_BT[i] =='\n'){
				PORTB |= 0x04;
													
				UCSR0B |= (1<<RXCIE0); //habilitando a recepção
				UCSR0B &= ~0x40; //desabilitando interrupção por transmissão
			}
			else{

				UDR0 = comandos_conf_BT[i];
				i++;
			}
		}*/
		/*
		if (buffer_rx_bth->tl->byte == '|'){
				
			if (ponteiro == 0){
				PORTB ^= 4;
				ponteiro = buffer_rx_bth->hd;
			}

			
			if ( (UCSR0A & (1<<UDRE0)) && ponteiro!=0){
				UDR0 = ponteiro->byte;

				if(ponteiro->byte == '|'){ //verifica se é o fim de mensagem
					//UCSR0B &= ~(1<<TXEN0); //desabilita interrupção
					
					remove_inst_BT(buffer_rx_bth); //retira as intruções da fila de rx
					//configure_USART(); 
					//UCSR0B |= RXCIE0; //reabilitando a interrupção de rx
					ponteiro = 0; // zerando o ponteiro
				}
				else
					ponteiro = ponteiro->prox;
			}	
		}*/
		
		if (received_message()){
			
			if(j==0){
					
				PORTB ^= 4;	//sending data
				transmit(usart_buff_rx.buffer[usart_buff_rx.start]);
				j++;
			}
			else if ( UCSR0A & (1<<UDRE0)){

				transmit(usart_buff_rx.buffer[usart_buff_rx.start]);
				if (usart_buff_rx.buffer[usart_buff_rx.start] == '|'){
					UCSR0B &= ~(1<<TXCIE0);
					j=0;
				}
			}	
		
		}
	
	}


	display_queue *f_display = malloc(sizeof(display_queue)); //fila de comandos do display
	f_display = display_initialization(f_display); // iniciando a fila de comandos

	configure_timer(); //code regarding timer configuration (driver/TIMER.c)
	
	
	while (f_display->hd != 0){
		f_display = send_data(f_display);
	}
	
			
	while(1){
		sei(); //enabling interruption
		
		set_sleep_mode(SLEEP_MODE_IDLE); //colocando em idle mode, desabilitando o clock da CPU e da flash
		sleep_mode(); //habilitando o sleep do uC
		
		
		if (counter == 1000){ 
			counter = 0; //reiniciando o contador
			blink_led(); // 1 second passed
			
			
			///////////////// time running ////////////////////
			display_update(&local_time);
			
			sprintf(time, "%i%i/%i%i/%i %i%i:%i%i\n%i%i/%i%i/%i %i%i:%i%i\0", local_time.tm_mday/10, local_time.tm_mday%10, ( local_time.tm_mon+1)/10, ( local_time.tm_mon+1)%10, local_time.tm_year, local_time.tm_hour/10, local_time.tm_hour%10, local_time.tm_min/10,local_time.tm_min%10, target_time.tm_mday/10, target_time.tm_mday%10, (target_time.tm_mon+1)/10, (target_time.tm_mon+1)%10, target_time.tm_year, target_time.tm_hour/10, target_time.tm_hour%10, target_time.tm_min/10, target_time.tm_min%10);

				
				f_display = create_instruction(f_display, 0x01, 0, 0, 0); //apagando a tela do display
				
				index = 0;
			
			////////////////////////////////////////////////////////////////////////
		}
		if (f_display->hd != 0)
			f_display = send_data(f_display);
		
		
		//if (index>-1 && index<32){ // verifica se tem dados a serem enviados no display
		if (index>-1 && time[index] != '\0'){ 

			int y = index/16 +1;
		       	int x = (index-16*(y-1)) %16;
			f_display  = create_instruction(f_display, time[index], x, y, 0); //envia os dados
			//hd = envia_dados(hd, &d);
			index++;
		}
		
	}

	return 0;
}



void blink_led(){
	PORTB ^= 2;
	
}
