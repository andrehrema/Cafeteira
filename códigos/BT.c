#include"BT.h"
#include<stdint.h>
#include<stdlib.h>
#include<avr/io.h>

volatile BT_data * add_inst_BT(volatile BT_data *buffer_bth, char command){
	
	BT_node *inst = malloc(sizeof(BT_node)); //criando uma instrucao
	inst->byte = command;
	
	if (buffer_bth->hd == 0){
		buffer_bth->hd = inst;
		buffer_bth->hd->next = 0; //pointing to the end of the queue		
		buffer_bth->tl = buffer_bth->hd; //tail pointing to zero
		return buffer_bth;
	}
	else{
		inst->next = buffer_bth->tl->next; // pointing to the next element
		buffer_bth->tl->next = inst; // inserting inst in the queue
		buffer_bth->tl = inst; // tail poiting to inst
	}
	
	return buffer_bth;
}





volatile BT_data * remove_inst_BT(volatile BT_data *buffer_bth){ //funcao que remove instrucao da fila
	
	while(buffer_bth->hd->byte != '|'){ // character '|' indicate end of received string
		
		BT_node *erase = buffer_bth->hd;	

		buffer_bth->hd=buffer_bth->hd->next;
		free(erase); // deleting element
	}

	BT_node *erase = buffer_bth->hd;			
	if(buffer_bth->hd == buffer_bth->tl)
		buffer_bth->tl = buffer_bth->tl->next;
		
	buffer_bth->hd=buffer_bth->hd->next; 
	free(erase); // deleting node
	
	return buffer_bth;
}


int8_t verify_BT_rx(volatile BT_data *buffer_bth){
	BT_node *node_verification = buffer_bth->hd;


	if(node_verification->byte == 'O' && (node_verification->next != 0)){
		

		if (node_verification->next->byte == 'K'){

			return REPLY_OK;
			
		}
	}

	if (!node_verification || !node_verification->next){
		
		return REPLY_INCOMPLETE; //incomplete message received
	}
	
	return REPLY_NOK; // not ok responded

}
