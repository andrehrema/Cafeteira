#ifndef BLUETOOTH
#include<stdint.h>
#define BLUETOOTH
#define F_CPU 2000000
#define BAUDRATE 9600
#define PRESCALER ((F_CPU / (BAUDRATE * 16UL)) -1)
#define REPLY_OK 1
#define REPLY_NOK 0
#define REPLY_INCOMPLETO 2

typedef struct no_BT BT_no;
typedef struct fila_BT_dados BT_dados;

struct no_BT{

	char byte;
	BT_no *prox;
};


struct fila_BT_dados{
	BT_no *hd;
	int index;
	BT_no *tl;
};


volatile BT_dados * adiciona_inst_BT(volatile BT_dados *buffer_bth, char comando);
volatile BT_dados * remove_inst_BT(volatile BT_dados *buffer_bth);
int8_t verifica_BT_rx(volatile BT_dados *buffer_bth);
#endif
