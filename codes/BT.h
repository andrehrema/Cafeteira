#ifndef BLUETOOTH
	#include<stdint.h>
	#define BLUETOOTH
	#define F_CPU 2000000
	#define BAUDRATE 2400
	#define PRESCALER ((F_CPU / (BAUDRATE * 16UL)) -1)
	#define REPLY_OK 1
	#define REPLY_NOK 0
	#define REPLY_INCOMPLETE 2

	typedef struct node_BT BT_node;
	typedef struct queue_BT_data BT_data;

	struct node_BT{

		char byte;
		BT_node *next;
	};


	struct queue_BT_data{
		BT_node *hd;
		BT_node *tl;
	};


	volatile BT_data * add_inst_BT(volatile BT_data *buffer_bth, char command);
	volatile BT_data * remove_inst_BT(volatile BT_data *buffer_bth);
	int8_t verify_BT_rx(volatile BT_data *buffer_bth);
#endif
