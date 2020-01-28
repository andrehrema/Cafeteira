#ifndef DISPLAY //macro que não deixa o arquivo ser incluido duas vezes
#define DISPLAY // define criado somente para a macro acima
#define ENABLE 4
#define QUATRO_BITS 0x02
#define DISPLAY_ON 0x0E
#define POS_NULA 0


#include<stdint.h>
typedef struct no_instrucao no;
typedef struct display display_7seg;
typedef struct display_bits instrucao;
typedef struct fila_comandos fila;

struct display_bits{
	uint16_t dados: 8;
	uint16_t RS: 1;
	uint16_t RW: 1;
	uint16_t comando_solo: 1;
};


struct no_instrucao{
	instrucao i;
	no *prox;
};

struct fila_comandos{
	no *hd;
	no *tl;
};



struct display{
	int linha_escrita; //cima {1,2}
	int coluna_escrita; //esquerda [1, 16]

	int linha_prox;
	int coluna_prox;

	int linha_user;
	int coluna_user;
};



//fila * configura_display(fila *f, display_7seg *d);

fila * inicia_display(fila *f);
fila * remove_instrucao (fila *f);
fila * adiciona_instrucao (fila *f, no *inst);
fila * cria_instrucao(fila* f, char c, int x, int y, int comando_solo);
fila * envia_dados(fila *f, display_7seg *d);

#endif
