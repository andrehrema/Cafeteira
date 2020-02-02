#include<avr/io.h>
#include<stdint.h>

volatile uint16_t counter = 0;

ISR (TIMER2_COMPA_vect){ //timer interruption handler

        counter+=1;
}


void configure_timer(){

        TCCR2A = (1<<WGM21); //timer counts until OCRA value
        //OCR2A = 125; //valor até onde o timer vai contar; preciso contar 125 vezes até 125
        OCR2A  = 250; //OCRA value

        TIMSK2 = (1<<OCIE2A); // interruption when timer value == OCRA value
        //TIMSK2 _SFR_MEM8 = OCIE2A; //habilita interrupção ao chegar no valor do compare A
        //TCCR2B = 10; //timer conta até o valor definido no OCRA, dividindo o clock de 125Khz por 8 resultando em 15625 Hz

        TCCR2B = 2; // timer dividided by 8, from 2MHz to 250KHz
        TCNT2 = 0; //timer intial value

}

