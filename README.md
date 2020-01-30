# Cafeteira
This project aims to automate my coffe machine using a atmega328p to set the time and control it.

##Folders
- codes: code developed up to now.

##Codes
- main.c: It contains the main functions and microcontroller configurations;
- BT.c: Interface developed to serial communication;
- BT.h: Header of serial interface;
- display.c: 16x2 Display communication interface;
- display.h: Header of display communication interface;
- configure.c: USART configuration;
- configure.h: USART configuration header.
- Makefile.


Because the project is not finished yet, there are two excluding serial communication techinques under test:

  * Queue with dynamic allocation for each byte received (BT.h: BT_data);
  * Fixed buffer length (main.c: buffer_rx)
