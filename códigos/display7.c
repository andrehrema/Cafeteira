//extern int contador;
//extern int contador2;

#include"display7.h"
#include<stdint.h>
#include<avr/io.h>
#include<stdlib.h>
#include<stdio.h>


fila * inicia_display(fila *f){
	f->hd = 0;
        f->tl = 0;
        ///////////////////////// sequência de instruções necessárias para inicialização no modo 4 bits ///////////////
	f = cria_instrucao(f, 0x30, POS_NULA, POS_NULA,1);
        f = cria_instrucao(f, 0x30, POS_NULA, POS_NULA,1);
        f = cria_instrucao(f, 0x30, POS_NULA, POS_NULA,1);
        f = cria_instrucao(f, 0x20, POS_NULA, POS_NULA,1);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	

        f = cria_instrucao(f, 0x28, POS_NULA, POS_NULA,0); // modo 4 bits, 2 linhas, 5x7
        f = cria_instrucao(f, 0x0D, POS_NULA, POS_NULA,0); // display ligado com o cursor apagado
        f = cria_instrucao(f, 0x06, POS_NULA, POS_NULA,0); // escreve deslocando o cursor para a direita
        f = cria_instrucao(f, 0x01, POS_NULA, POS_NULA,0); // apaga a tela e coloca no início da primeira linha

	return f;

}


fila * remove_instrucao(fila *f){

	if (f->hd == 0){
		return f;
	}
	no *remove = f->hd;
	f->hd = f->hd->prox;
	
	free(remove);
	
	return f;
}

fila * adiciona_instrucao(fila *f, no* inst){

	if (f->hd == 0){ //adicionando o primeiro elemento da fila
		f->hd = inst;
		f->tl = inst;
		f->hd->prox = 0;
	}
	else{ //adicinoando no final da fila
	
		f->tl->prox = inst;
		f->tl = inst;
		f->tl->prox = 0;
	}
	


	return f;
}

fila * cria_instrucao(fila *f, char c, int x, int y, int comando_solo){



	if ((!x && !y) || (c=='\n')){
	
		no *inst_com = malloc(sizeof(no));
		inst_com->i.comando_solo = comando_solo;	
	
		inst_com->i.dados = c;
	

		if (c == '\n'){
			inst_com->i.dados = 0xC0; //inicio linha 2
		}


	//else if (x & y){
		//inst_com->i.dados = 0x80; // indo para a posicao da direita
	//}

		inst_com->i.RS = 0; // comando
		inst_com->i.RW = 0; //escrevendo

		f = adiciona_instrucao(f, inst_com); //adicionando o comando
	
	}

	else{
		no *inst_dado = malloc(sizeof(no));

		inst_dado->i.dados = c;
		
		

		
		inst_dado->i.RW = 0; //escrevendo;
		
		inst_dado->i.RS = 1; //dado
		inst_dado->prox = 0;
		inst_dado->i.comando_solo= 0; 
		f = adiciona_instrucao(f, inst_dado);
	}
	
	return f;

}

		
fila * envia_dados(fila *f, display_7seg *d){

	//no *inst = malloc(sizeof(no)); 

	//inst = hd;
	//while(inst!=0){
	
	if (f->hd == 0){
	
		return f;
	}

	PORTC |= (f->hd->i.dados >> 4) & 15; // escrevendo os bits mais altos
	PORTC |= (f->hd->i.RS<<4 & 16); // escrevendo no pino RS
      	PORTC |= (f->hd->i.RW<<5 & 32); //escrevendo nos pinos RW
	PORTD |= ENABLE; //habilitando o enable;

	PORTD &= ~ENABLE; //desligando o enable
	
	PORTC &= ~(15);
	if (f->hd->i.comando_solo != 1){
			
		PORTC |= (f->hd->i.dados) & 15; //escrevendo os bits mais baixos
		PORTD |= ENABLE; //ligando o enable
		PORTD &= ~ENABLE; //desligando o enable

		
	}
		

	f = remove_instrucao(f);
	
	PORTC &= ~(63);
	return f;
}
