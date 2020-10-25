#define KEYBOARD_IRQ 0x01
#define NUM_KEYS   88         //number of scan codes
#define KEYBOARD_PORT	0x60 
#define CASE_CONVERSION 32   //to convert ascii upper case and lower case  

#include "i8259.h"

//extern functions
extern void initialize_keyboard();
extern void key_board_handler();


