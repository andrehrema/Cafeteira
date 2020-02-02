#ifndef DISPLAY //macro que não deixa o arquivo ser incluido duas vezes
	
	#define DISPLAY // define criado somente para a macro acima
	#define ENABLE 4
	#define QUATRO_BITS 0x02
	#define DISPLAY_ON 0x0E
	#define POS_NULL 0

	#include<stdint.h>
	#include<time.h>

	typedef struct node_instruction display_node;
	typedef struct display display_7seg;
	typedef struct display_bits instruction;
	typedef struct queue_commands display_queue;

	struct display_bits{
		uint16_t data: 8;
		uint16_t RS: 1;
		uint16_t RW: 1;
		uint16_t command_solo: 1;
	};


	struct node_instruction{
		instruction i;
		display_node *next;
	};

	struct queue_commands{
		display_node *hd;
		display_node *tl;
	};



	//fila * configura_display(fila *f, display_7seg *d);

	display_queue * display_initialization(display_queue *f);
	display_queue * remove_instruction (display_queue *f);
	display_queue * add_instruction (display_queue *f, display_node *inst);
	display_queue * create_instruction(display_queue* f, char c, int x, int y, int command_solo);
	display_queue * send_data(display_queue *f);
	void display_update (struct tm *local_time);

#endif
