#include"BT.h"
#include<stdint.h>
#include<stdlib.h>
#include<avr/io.h>

volatile BT_dados * adiciona_inst_BT(volatile BT_dados *buffer_bth, char comando){
	
	BT_no *inst = malloc(sizeof(BT_no)); //criando uma instrucao
	inst->prox = 0; // iniciando o proximo como 0
		
	inst->byte = comando;

	if (buffer_bth->hd == 0){
		
		buffer_bth->hd = inst;
		buffer_bth->hd->prox = 0; //apontando para o fim da fila
		buffer_bth->tl = buffer_bth->hd; //tail apontando para o zero
	}
	else{
		inst->prox = buffer_bth->tl->prox;
		buffer_bth->tl->prox = inst;
		buffer_bth->tl = inst;
	}
	
	return buffer_bth;
}





volatile BT_dados * remove_inst_BT(volatile BT_dados *buffer_bth){ //funcao que remove instrucao da fila

	while(buffer_bth->hd != 0){
		
		BT_no *apaga;	
		if(buffer_bth->hd->byte == '|'){
			
			apaga = buffer_bth->hd;
			if(buffer_bth->hd == buffer_bth->tl)
				buffer_bth->tl = buffer_bth->tl->prox;
			buffer_bth->hd=buffer_bth->hd->prox;
			free(apaga); // apagando o no na fila
			break;
		}

		apaga = buffer_bth->hd;
		buffer_bth->hd=buffer_bth->hd->prox;

		free(apaga); // apagando o nó na fila
	}
	return buffer_bth;
}


int8_t verifica_BT_rx(volatile BT_dados *buffer_bth){
	//BT_no *no_verificacao = malloc(sizeof(BT_no));
	BT_no *no_verificacao = buffer_bth->hd;


	if(no_verificacao->byte == 'O' && (no_verificacao->prox != 0)){
		

		if (no_verificacao->prox->byte == 'K'){
			//free(no_verificacao);

			return REPLY_OK;
			
		}
	}

	if (!no_verificacao || !no_verificacao->prox){
		//free(no_verificacao);
		
		//PORTB|=2;
		return REPLY_INCOMPLETO;
	}
	
	
	//free(no_verificacao);
	return REPLY_NOK;

}
