#include<avr/io.h> //#include<avr/common.h> //registradores comuns aos microcontroladores AVR, incluido pelo avr/io.h. Esse header vai identificar qual o uC pela definição do mmcu e irá incluir o header adequado. O 
#include<avr/interrupt.h> // header de interrupções
#include<avr/sleep.h> // header referente ao dormir
#include<avr/fuse.h>
#include"display.h"
#include<time.h> // header com as referências de tempo (ano, mês, dia, hora, ...)
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include"BT.h"
#include"configure.h"

void blink_led();
uint16_t counter = 0;

volatile BT_data *buffer_tx_bth;  //sending buffer, compiler doesn't optimize  volatile data type
volatile BT_data *buffer_rx_bth; //receiving buffer, compiler doesn't optimize volatile data type

char buffer_rx[40];
int index_rx = 0;

ISR (TIMER2_COMPA_vect){ //tratador de interrupção do timer

	counter+=1; 
}


ISR(USART_RX_vect){ // receiving ISR
	UCSR0B &= ~(1<<TXCIE0); //desabilitando interrupção por transmissão
	
	PORTB ^= 2;
	/*(char resultado = UDR0; // criando variável e armazenando o valor recebido nela
	
	buffer_rx_bth = adiciona_inst_BT(buffer_rx_bth, resultado); //adicionando a informaçao na fila
	*/

	
	buffer_rx[index_rx] = UDR0;
	index_rx++;
	UCSR0B |= (1<<TXCIE0); //reabilitando instrução por transmissão
}

ISR(USART_UDRE_vect){ //tratador de interrupção quando o buffer de transmissão estiver vazio
	
}

ISR(USART_TX_vect){
}


int main (void){

	buffer_rx_bth = malloc(sizeof(BT_data)); //allocating space for buffer_rx_bth
	buffer_rx_bth->hd=0; // head = 0
	buffer_rx_bth->tl=0; // tail = 0



	display_queue *f_control = malloc(sizeof(display_queue));
	f_control->hd = 0;
	f_control->tl = 0;

	display_7seg d; //cursor positioning


	
	//f = cria_instrucao(f, 0x80, POS_NULA, POS_NULA,0); // coloca o cursor no início da primeira linha
	CLKPR = (1<<CLKPCE); // enabling bit writing in clock divider
	//CLKPR = 6; // divisor do clock = 64; O QUE ERA 8 MHZ PASSA A SER 125 KHZ
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
	
	char comandos_conf_BT[] = "AT\nAT+BAUD4\nAT+NAMECAFETEIRA\nAT+PIN8324\n"; //commands of Bluetooth module
		
	int i=1;
	int j=0;
	BT_node *pointer = 0; 
	
	while(i<sizeof(comandos_conf_BT)){

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
		
		if (buffer_rx[index_rx-1] == '|'){
					
			if ( UCSR0A & (1<<UDRE0)){
				PORTB ^= 4;	//sending data
				UDR0 = buffer_rx[j]; 

				if(buffer_rx[j] == '|'){ //verifica se é o fim de mensagem
					//UCSR0B &= ~(1<<TXEN0); //desabilita interrupção
					
					//configure_USART(); 
					//UCSR0B |= RXCIE0; //reabilitando a interrupção de rx
					j = 0; // zerando o ponteiro
					index_rx=0;
				}
				else
					j++;
			}	
		
		}
	
	}
	display_queue *f_display = malloc(sizeof(display_queue)); //fila de comandos do display
	f_display = display_initialization(f_display); // iniciando a fila de comandos



	TCCR2A = (1<<WGM21); //timer counts until OCRA value
	//OCR2A = 125; //valor até onde o timer vai contar; preciso contar 125 vezes até 125
	OCR2A  = 250; //OCRA value
	
	TIMSK2 = (1<<OCIE2A); // interruption when timer value == OCRA value
	//TIMSK2 _SFR_MEM8 = OCIE2A; //habilita interrupção ao chegar no valor do compare A
	//TCCR2B = 10; //timer conta até o valor definido no OCRA, dividindo o clock de 125Khz por 8 resultando em 15625 Hz
	
	TCCR2B = 2; // timer dividided by 8, from 2MHz to 250KHz
	TCNT2 = 0; //timer intial value
	
	
	while (f_display->hd != 0){
		f_display = send_data(f_display, &d);
	}
	
			
	while(1){
		sei(); //enabling interruption
		
		set_sleep_mode(SLEEP_MODE_IDLE); //colocando em idle mode, desabilitando o clock da CPU e da flash
		sleep_mode(); //habilitando o sleep do uC
		
		
		if (counter == 1000){ 
			counter = 0; //reiniciando o contador
			blink_led(); // 1 second passed
			
			
			///////////////// time running ////////////////////
			
			if (local_time.tm_sec < 59){
				local_time.tm_sec += 1;
			}
			else{
				local_time.tm_sec = 0;
				if (local_time.tm_min < 59){
					local_time.tm_min += 1;
				}
				else{
					local_time.tm_min = 0;
					if (local_time.tm_hour < 23){
						local_time.tm_hour +=1;
					}
					else {
						local_time.tm_hour = 0;
						if (local_time.tm_mday < 30 && local_time.tm_mon != 1){
							local_time.tm_mday += 1;
						}
						else if (local_time.tm_mday == 30 && ( local_time.tm_mon == 3 || local_time.tm_mon == 5 || local_time.tm_mon == 8 || local_time.tm_mon == 10)){
							local_time.tm_mday = 1;
							local_time.tm_mon += 1;
						}
						else if (local_time.tm_mday == 31 && ( local_time.tm_mon == 0 || local_time.tm_mon == 2 || local_time.tm_mon == 4 || 
									local_time.tm_mon == 6 || local_time.tm_mon == 7 || local_time.tm_mon ==9)){
							local_time.tm_mday = 1;
							local_time.tm_mon +=1;
						}
						else if (local_time.tm_mday == 31 && local_time.tm_mon == 11){
							local_time.tm_mday = 1;
							local_time.tm_mon = 0;
							local_time.tm_year +=1;
						}
						else if (local_time.tm_mday == 28 && local_time.tm_mon == 1){
							local_time.tm_mday = 1;
							local_time.tm_mon +=1;
						}

					}
				}
			

				sprintf(time, "%i%i/%i%i/%i %i%i:%i%i\n%i%i/%i%i/%i %i%i:%i%i\0", local_time.tm_mday/10, local_time.tm_mday%10, ( local_time.tm_mon+1)/10, ( local_time.tm_mon+1)%10, local_time.tm_year, local_time.tm_hour/10, local_time.tm_hour%10, local_time.tm_min/10,local_time.tm_min%10, target_time.tm_mday/10, target_time.tm_mday%10, (target_time.tm_mon+1)/10, (target_time.tm_mon+1)%10, target_time.tm_year, target_time.tm_hour/10, target_time.tm_hour%10, target_time.tm_min/10, target_time.tm_min%10);

				
				f_display = create_instruction(f_display, 0x01, 0, 0, 0); //apagando a tela do display
				
				index = 0;
			}
			////////////////////////////////////////////////////////////////////////
		}
		if (f_display->hd != 0)
			f_display = send_data(f_display, &d);
		
		
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
