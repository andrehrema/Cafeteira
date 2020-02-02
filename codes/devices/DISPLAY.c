//extern int contador;
//extern int contador2;

#include"DISPLAY.h"
#include<stdint.h>
#include<avr/io.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>

display_queue * display_initialization(display_queue *f){
	f->hd = 0;
        f->tl = 0;
        ///////////////////////// 4 bit mode instruction sequence ///////////////
	f = create_instruction(f, 0x30, POS_NULL, POS_NULL,1);
        f = create_instruction(f, 0x30, POS_NULL, POS_NULL,1);
        f = create_instruction(f, 0x30, POS_NULL, POS_NULL,1);
        f = create_instruction(f, 0x20, POS_NULL, POS_NULL,1);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	

        f = create_instruction(f, 0x28, POS_NULL, POS_NULL,0); // modo 4 bits, 2 linhas, 5x7
        f = create_instruction(f, 0x0D, POS_NULL, POS_NULL,0); // display ligado com o cursor apagado
        f = create_instruction(f, 0x06, POS_NULL, POS_NULL,0); // escreve deslocando o cursor para a direita
        f = create_instruction(f, 0x01, POS_NULL, POS_NULL,0); // apaga a tela e coloca no início da primeira linha

	return f;

}


void display_time_update(struct tm *local_time){
	if (local_time->tm_sec < 59){
			local_time->tm_sec += 1;
		}
	else{
		local_time->tm_sec = 0;
		if (local_time->tm_min < 59){
			local_time->tm_min += 1;
		}
		else{
			local_time->tm_min = 0;
			if (local_time->tm_hour < 23){
				local_time->tm_hour +=1;
			}
			else {
				local_time->tm_hour = 0;
				if (local_time->tm_mday < 30 && local_time->tm_mon != 1){
					local_time->tm_mday += 1;
				}
				else if (local_time->tm_mday == 30 && ( local_time->tm_mon == 3 || local_time->tm_mon == 5 || local_time->tm_mon == 8 || local_time->tm_mon == 10)){
					local_time->tm_mday = 1;
					local_time->tm_mon += 1;
				}
				else if (local_time->tm_mday == 31 && ( local_time->tm_mon == 0 || local_time->tm_mon == 2 || local_time->tm_mon == 4 ||
							local_time->tm_mon == 6 || local_time->tm_mon == 7 || local_time->tm_mon ==9)){
					local_time->tm_mday = 1;
					local_time->tm_mon +=1;
				}
				else if (local_time->tm_mday == 31 && local_time->tm_mon == 11){
					local_time->tm_mday = 1;
					local_time->tm_mon = 0;
					local_time->tm_year +=1;
				}
				else if (local_time->tm_mday == 28 && local_time->tm_mon == 1){
					local_time->tm_mday = 1;
					local_time->tm_mon +=1;
				}

			}
		}
		

	}
}

display_queue * remove_instruction(display_queue *f){

	if (f->hd == 0){
		return f;
	}
	display_node *remove = f->hd;
	f->hd = f->hd->next;
	
	free(remove);
	
	return f;
}

display_queue * add_instruction(display_queue *f, display_node* inst){

	if (f->hd == 0){ // inserting the first instruction
		f->hd = inst;
		f->tl = inst;
		f->hd->next = 0;
	}
	else{ // inserting instruction in the tail
	
		f->tl->next = inst;
		f->tl = inst;
		f->tl->next = 0;
	}
	
	return f; //returning queue_display
}

display_queue * create_instruction(display_queue *f, char c, int x, int y, int command_solo){



	if ((!x && !y) || (c=='\n')){
	
		display_node *inst_com = malloc(sizeof(display_node));
		inst_com->i.command_solo = command_solo;	
	
		inst_com->i.data = c;
	

		if (c == '\n'){
			inst_com->i.data = 0xC0; //beginning of second line
		}


	//else if (x & y){
		//inst_com->i.dados = 0x80; // indo para a posicao da direita
	//}

		inst_com->i.RS = 0; // comando
		inst_com->i.RW = 0; //escrevendo

		f = add_instruction(f, inst_com); //adicionando o comando
	
	}

	else{
		display_node *inst_data = malloc(sizeof(display_node));

		inst_data->i.data = c;
		
		
		inst_data->i.RW = 0; //escrevendo;
		
		inst_data->i.RS = 1; //dado
		inst_data->next = 0;
		inst_data->i.command_solo= 0; 
		f = add_instruction(f, inst_data);
	}
	
	return f;

}

		
display_queue * send_data(display_queue *f){

	
	if (f->hd == 0){
	
		return f;
	}

	PORTC |= (f->hd->i.data >> 4) & 15; // sending higher bits
	PORTC |= (f->hd->i.RS<<4 & 16); // sending RS
      	PORTC |= (f->hd->i.RW<<5 & 32); //sending RW
	PORTD |= ENABLE; //enabling;

	PORTD &= ~ENABLE; //disabling
	
	PORTC &= ~(15);
	if (f->hd->i.command_solo != 1){
			
		PORTC |= (f->hd->i.data) & 15; //sending higher bits
		PORTD |= ENABLE; //enabling
		PORTD &= ~ENABLE; //disabling
	}
		

	f = remove_instruction(f);
	
	PORTC &= ~(63);
	return f;
}
