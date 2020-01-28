#include<avr/io.h> //#include<avr/common.h> //registradores comuns aos microcontroladores AVR, incluido pelo avr/io.h. Esse header vai identificar qual o uC pela definição do mmcu e irá incluir o header adequado. O 
#include<avr/interrupt.h> // header de interrupções
#include<avr/sleep.h> // header referente ao dormir
#include<avr/fuse.h>
#include"display7.h"
#include<time.h> // header com as referências de tempo (ano, mês, dia, hora, ...)
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include"BT.h"

void pisca_led();
uint16_t contador = 0;
char RX_buffer;
volatile BT_dados *buffer_tx_bth;  //buffer de envio dos dados, volatil para que o compilador não otimize os dados
volatile BT_dados *buffer_rx_bth; //buffer de recebimento dos dados, volatil para que o compilador não otimize os dados

ISR (TIMER2_COMPA_vect){ //tratador de interrupção do timer

	contador+=1; 
}


ISR(USART_RX_vect){ //tratador de interrupção da recepção
	UCSR0B &= ~(1<<TXCIE0); //desabilitando interrupção por transmissão

	char resultado = UDR0; // criando variável e armazenando o valor recebido nela
	PORTB^=2; // sinal de que entrou no ISR de rx
	buffer_rx_bth = adiciona_inst_BT(buffer_rx_bth, resultado); //adicionando a informaçao na fila
	
	UCSR0B |= (1<<TXCIE0); //reabilitando instrução por transmissão
}

ISR(USART_UDRE_vect){ //tratador de interrupção quando o buffer de transmissão estiver vazio
	
}

ISR(USART_TX_vect){
}


int main (void){

	buffer_rx_bth = malloc(sizeof(BT_dados));
	buffer_rx_bth->hd=0; // iniciando o head da fila
	buffer_rx_bth->tl=0; // iniciando o tail da fila



	fila *f_controle = malloc(sizeof(fila));
	f_controle->hd = 0;
	f_controle->tl = 0;

	display_7seg d; //posicionamento do cursor


	
	//f = cria_instrucao(f, 0x80, POS_NULA, POS_NULA,0); // coloca o cursor no início da primeira linha
	CLKPR = (1<<CLKPCE); // habilitando escrita nos bits de divisão de clock
	//CLKPR = 6; // divisor do clock = 64; O QUE ERA 8 MHZ PASSA A SER 125 KHZ
 	CLKPR = 2; //divisor do clock = 4, de 8 MHz para 2 MHZ

	DDRB = 7; //setando PB2 e PB1 e PB0 como saída
	DDRC = 63; // setando PC[0,5] como saída
	DDRD = 4; //setando o PD2 como saída


	// como USBS bit no UCSR0C é 0, só um stop bit é utilizado
	
        UBRR0H = (uint8_t) ((PRESCALER >> 8) & 0x0F); //baud rate = 9600
	UBRR0L = (uint8_t) PRESCALER; // baud rate = 9600

	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)|(1<<TXCIE0); //habilita RX e TX, habilita RX e TX interruption
	UCSR0C |= (0x26); // paridade par, 8 bits de dados


	char hora[33];

	struct tm hora_local; //hora_local é o tempo marcado no microcontrolador
	hora_local.tm_sec = 0;
       	hora_local.tm_min = 0;
	hora_local.tm_hour = 15;
	hora_local.tm_mday = 11;
	hora_local.tm_mon = 11;
	hora_local.tm_year = 1995;
	
	struct tm hora_target; //hora_target é o tempo definido para o micro acordar e iniciar a cafeteira
	
	hora_target.tm_sec = 0;
       	hora_target.tm_min = 0;
	hora_target.tm_hour = 0;
	hora_target.tm_mday = 1;
	hora_target.tm_mon = 0;
	hora_target.tm_year = 2020;

	int index = -1;
	
	char comandos_conf_BT[] = "AT\nAT+BAUD4\nAT+NAMECAFETEIRA\nAT+PIN8324\n";
	//UDR0 = comandos_conf_BT[0];
	
	int i=1;
	BT_no *ponteiro = 0;
	
	while(i<sizeof(comandos_conf_BT)){

		sei(); // habilitando as interrupções
		set_sleep_mode(SLEEP_MODE_IDLE); //colocando em idle mode, desabilitando o clock da CPU e da flash
		sleep_mode(); //habilitando o sleep do uC
				
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

		if (buffer_rx_bth->tl != 0){
			if (buffer_rx_bth->tl->byte == '|'){
				PORTB |=4;
				UCSR0B |= (1<<TXCIE0); // habilita as transmissões
					
				if (ponteiro == 0){
					ponteiro = buffer_rx_bth->hd;
				}
				if ( (UCSR0A & (1<<UDRE0)) && ponteiro!=buffer_rx_bth->hd){
					UDR0 = ponteiro->byte;

					if(ponteiro->byte == '|'){ //verifica se é o fim de mensagem
						UCSR0B &= ~(1<<TXCIE0); //desabilita interrupção
						
						remove_inst_BT(buffer_rx_bth); //retira as intruções da fila de rx
						UCSR0B |= RXCIE0; //reabilitando a interrupção de rx
						BT_no *ponteiro = 0; // zerando o ponteiro
					}
					else{
						ponteiro = ponteiro->prox;
					}
				}
				else if(ponteiro == buffer_rx_bth->hd){
					UDR0 = ponteiro->byte; //enviando o valor recebido
					if (ponteiro->byte == '|'){ //verifica se é o fim de mensagem
											
						remove_inst_BT(buffer_rx_bth); //remove as instruções da fila de rx
						BT_no *ponteiro = 0; //zerando o ponteiro
					}
					else
						ponteiro = ponteiro->prox; //pulando para o próximo caractere da mensagem
				}
			}
		}

	}
	

	fila *f_display = malloc(sizeof(fila)); //fila de comandos do display
	f_display = inicia_display(f_display); // iniciando a fila de comandos



	TCCR2A = (1<<WGM21); //timer conta até o valor definido no OCRA
	//OCR2A = 125; //valor até onde o timer vai contar; preciso contar 125 vezes até 125
	OCR2A  = 250; //conta até 250
	
	TIMSK2 = (1<<OCIE2A); //interrupção por comparação com o valor de OCRA
	//TIMSK2 _SFR_MEM8 = OCIE2A; //habilita interrupção ao chegar no valor do compare A
	//TCCR2B = 10; //timer conta até o valor definido no OCRA, dividindo o clock de 125Khz por 8 resultando em 15625 Hz
	
	TCCR2B = 2; // clock do timer dividido por 8, de 2MHz para 250KHz
	TCNT2 = 0;
	
	
	PORTB |= 2;
	while (f_display->hd != 0){
		f_display = envia_dados(f_display, &d);
	}
	
			
	while(1){
		PORTB &= ~4;		
		sei(); //habilitando as interrupções
		
		set_sleep_mode(SLEEP_MODE_IDLE); //colocando em idle mode, desabilitando o clock da CPU e da flash
		sleep_mode(); //habilitando o sleep do uC
		
		//if (contador==125){
		if (contador == 1000){ 
			contador = 0; //reiniciando o contador
			pisca_led();
			
			
			///////////////// Bloco de avanço no tempo ////////////////////
			
			if (hora_local.tm_sec < 59){
				hora_local.tm_sec += 1;
			}
			else{
				hora_local.tm_sec = 0;
				if (hora_local.tm_min < 59){
					hora_local.tm_min += 1;
				}
				else{
					hora_local.tm_min = 0;
					if (hora_local.tm_hour < 23){
						hora_local.tm_hour +=1;
					}
					else {
						hora_local.tm_hour = 0;
						if (hora_local.tm_mday < 30 && hora_local.tm_mon != 1){
							hora_local.tm_mday += 1;
						}
						else if (hora_local.tm_mday == 30 && ( hora_local.tm_mon == 3 || hora_local.tm_mon == 5 || hora_local.tm_mon == 8 || hora_local.tm_mon == 10)){
							hora_local.tm_mday = 1;
							hora_local.tm_mon += 1;
						}
						else if (hora_local.tm_mday == 31 && ( hora_local.tm_mon == 0 || hora_local.tm_mon == 2 || hora_local.tm_mon == 4 || 
									hora_local.tm_mon == 6 || hora_local.tm_mon == 7 || hora_local.tm_mon ==9)){
							hora_local.tm_mday = 1;
							hora_local.tm_mon +=1;
						}
						else if (hora_local.tm_mday == 31 && hora_local.tm_mon == 11){
							hora_local.tm_mday = 1;
							hora_local.tm_mon = 0;
							hora_local.tm_year +=1;
						}
						else if (hora_local.tm_mday == 28 && hora_local.tm_mon == 1){
							hora_local.tm_mday = 1;
							hora_local.tm_mon +=1;
						}

					}
				}
			

				sprintf(hora, "%i%i/%i%i/%i %i%i:%i%i\n%i%i/%i%i/%i %i%i:%i%i\0", hora_local.tm_mday/10,hora_local.tm_mday%10, (hora_local.tm_mon+1)/10, (hora_local.tm_mon+1)%10, hora_local.tm_year, hora_local.tm_hour/10, hora_local.tm_hour%10, hora_local.tm_min/10,hora_local.tm_min%10, hora_target.tm_mday/10, hora_target.tm_mday%10, (hora_target.tm_mon+1)/10, (hora_target.tm_mon+1)%10, hora_target.tm_year, hora_target.tm_hour/10, hora_target.tm_hour%10, hora_target.tm_min/10, hora_target.tm_min%10);

				
				f_display = cria_instrucao(f_display, 0x01, 0, 0, 0); //apagando a tela do display
				
				index = 0;
			}
			////////////////////////////////////////////////////////////////////////
		}
		if (f_display->hd != 0)
			f_display = envia_dados(f_display, &d);
		
		
		//if (index>-1 && index<32){ // verifica se tem dados a serem enviados no display
		if (index>-1 && hora[index] != '\0'){ 

			int y = index/16 +1;
		       	int x = (index-16*(y-1)) %16;
			f_display  = cria_instrucao(f_display, hora[index], x, y, 0); //envia os dados
			//hd = envia_dados(hd, &d);
			index++;
		}


		
	}

	return 0;
}



void pisca_led(){
	PORTB ^= 2;
	
}
