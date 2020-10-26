#include "keyboard.h"

/* SOURCES:  https://wiki.osdev.org/PS/2_Keyboard
    some code from Linux documentation of PS/2 Keyboard
*/
static int buf_counter = 0;
static int row_flag = 0;
static uint8_t check_if_letter(char index);
static char check_if_symbol(char index);

static char scan_codes[NUM_KEYS]= 	{ //presses
    '\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0', '\0',
	 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', '\0', 'a', 's',
	 'd', 'f', 'g', 'h', 'j', 'k', 'l' , ';', '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v', 
	 'b', 'n', 'm',',', '.', '/', '\0', '*', '\0', ' ', '\0', '\0', '\0', '\0', '\0', '\0',
      '\0', '\0', '\0', '\0', '\0','\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 
      '\0', '\0', '\0', '\0', '\0', '\0', '\0'
     }; 

//0: shift state, 1: capslock state, 3: ctrl state
static uint8_t states[NUM_STATES] = {0, 0, 0};


/* initialize_keyboard
 * 
 * enables interrupt on IRQ line of keyboard on the pic
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Return value: None
 */ 
void initialize_keyboard(){
    enable_irq(KEYBOARD_IRQ);   //enable interrupt on pin on pic
}
 
/* key_board_handler
 * 
 * handles the interrupts of the keyboard, prints to console
 * Inputs: None
 * Outputs: character on screen
 * Side Effects: None
 * Return value: None
 */ 
void key_board_handler(){ 
    //0x3A, keycode for capslock  
    if(inb(KEYBOARD_PORT) == 0x3A){
        states[1] = ~(states[1]);
        send_eoi(KEYBOARD_IRQ);  
        return;
    }
    //---------shift checks---------
    if(states[0]){  //states[0] is shift state        
        //conversion to uppercase
        if(check_if_letter(inb(KEYBOARD_PORT))){
            add_to_kdb_buf(scan_codes[inb(KEYBOARD_PORT)] - CASE_CONVERSION);
            //putc(); 
            send_eoi(KEYBOARD_IRQ);  
            return;
        }
        else if(check_if_symbol(inb(KEYBOARD_PORT))){
            add_to_kdb_buf(check_if_symbol(inb(KEYBOARD_PORT)));
            //putc();
            send_eoi(KEYBOARD_IRQ);
            return;
        }
        //0xAA and 0xB6, release scan codes for l and r shift respectively
        else if(inb(KEYBOARD_PORT) != 0xAA || inb(KEYBOARD_PORT) != 0xB6){
            states[0] = 0;
            send_eoi(KEYBOARD_IRQ); 
            return;
        }
    }
    //-----caps lock checks------------
    if(states[1]){  //state[1] is capslock state
        if(check_if_letter(inb(KEYBOARD_PORT))){
            add_to_kdb_buf(scan_codes[inb(KEYBOARD_PORT)] - CASE_CONVERSION);
            //putc(scan_codes[inb(KEYBOARD_PORT)] - CASE_CONVERSION); 
            send_eoi(KEYBOARD_IRQ); 
            return; 
        }   
    }
    //-----control check---------
    if(states[2]){   //state[2] is ctrl state
        //0x26 is scancode for l
        if(inb(KEYBOARD_PORT) == 0x26){
            clear();
            send_eoi(KEYBOARD_IRQ);
            return;
        }
        //0xE0, 0x9D, release scan codes for l,r ctrl respectively
        else if(inb(KEYBOARD_PORT) == 0xE0 || inb(KEYBOARD_PORT) ==0x9D){
            states[2] = 0;
            send_eoi(KEYBOARD_IRQ);
            return;
        }
    }
    //----set states and generic output-----
                    
    //0x2A and 0x36 scan codes for l,r shifts respecitvely
    if(inb(KEYBOARD_PORT) == 0x2A || inb(KEYBOARD_PORT) == 0x36){
        states[0] = 1;
        send_eoi(KEYBOARD_IRQ); 
        return;
    }
    //0x1D, 0xE0, scan codes for l,r ctrl respectively
    else if(inb(KEYBOARD_PORT) == 0xE0 || inb(KEYBOARD_PORT) == 0x1D){
        states[2] = 1;
        send_eoi(KEYBOARD_IRQ);
        return;            
    }
    //0x0F, scan code for tab
    else if(inb(KEYBOARD_PORT) == 0x0F){
        add_to_kdb_buf(' ');
        add_to_kdb_buf(' ');
        add_to_kdb_buf(' ');
        add_to_kdb_buf(' ');
        /*
        putc(' ');
        putc(' ');
        putc(' ');
        putc(' ');*/
        send_eoi(KEYBOARD_IRQ);
        return;             
    }
    else if(scan_codes[inb(KEYBOARD_PORT)] == '\0'){
        send_eoi(KEYBOARD_IRQ);
        return;
    } 
    //check if scan code is in bounds of scan code array
    else if(inb(KEYBOARD_PORT) < NUM_KEYS && inb(KEYBOARD_PORT) >= 0){   
        add_to_kdb_buf(scan_codes[inb(KEYBOARD_PORT)]);
        //putc();   //print character to screen
        send_eoi(KEYBOARD_IRQ);  //stop interrupt on pin
        return;
    }
    
    send_eoi(KEYBOARD_IRQ);  //stop interrupt on pin  
    return;  
}

/* check_if_letter
 * 
 * Description:checks if the scan code is a letter so 
 * it can be capitalized during a shift
 * Inputs: index: scan code index
 * Outputs: none
 * Side Effects: None
 * Return value: true/false (0/1)
 */ 
static uint8_t check_if_letter(char index){
    uint8_t check = 0;
    if(index >= 0x10 && index <= 0x19){
        check = 1;
    }
    else if(index >= 0x1E && index <= 0x26){
        check = 1;
    }
    else if(index >= 0x2C && index <= 0x32){
        check = 1;
    }
    return check;
}

/* check_if_symbol
 * 
 * Description:checks if the scan code is a symbol so 
 * it can be changed during a shift
 * Inputs: index: scan code index
 * Outputs: none
 * Side Effects: None
 * Return value: converted index (0 if not symbol)
 */ 
static char check_if_symbol(char index){
    switch(index){
        //scancodes (0x02-0x0B) for numbers 0-9
        case 0x02:
            return '!';
        case 0x03:
            return '@';
        case 0x04:
            return '#';
        case 0x05:
            return '$';
        case 0x06:
            return '%';
        case 0x07:
            return '^';
        case 0x08:
            return '&';
        case 0x09:
            return '*';
        case 0x0A:
            return '(';
        case 0x0B:
            return ')';
        //scancode for - (0x0C)
        case 0x0C:
            return '_';
        //scancode for = (0x0D)
        case 0x0D:
            return '+';
        //scancode for [ (0x1A)
        case 0x1A:
            return '{';
        //scancode for ] (0x1B)
        case 0x1B:
            return '}';
        //scancode for ; (0x27)
        case 0x27:
            return ':';
        //scancode for ' (0x28)
        case 0x28:
            return '\"';
        //scancode for ` (0x29)
        case 0x29:
            return '~';
        //scancode for \ (0x2B)
        case 0x2B:
            return '|';
        //scancode for , (0x33)
        case 0x33:
            return '<';
        //scancode for . (0x34)
        case 0x34:
            return '>';
        //scancode for / (0x35)
        case 0x35:
            return '?';
        //not symbol
        default:
            return 0;
    }
}
/* add_to_kdb_buf
 * 
 * Description: Adds to keyboard buffer to keep track of the 128 char limit for calling terminal driver.
 * Also will roll over to next line when max number of chars is entered for a row.
 * Inputs: Char to print
 * Outputs: none
 * Side Effects: Rolls over to next line
 * Return value: none
 */ 
void add_to_kdb_buf(char c){
    kbd_buf[buf_counter] = c;
    buf_counter++;
    putc(c);
    int x = get_x();
    //if the cursor is at the last spot, set a flag
    if(row_flag == 1){ //in the case we typed at the last spot in the row
        if(c != '\n') putc('\n'); //ignores newline so it does not print newline twice
        row_flag = 0;
    }
    if(x == 79) row_flag = 1;

    int i;

    if(buf_counter == 128){
        buf_counter = 0;
        for(i = 0; i < 128; i++){
            kbd_buf[i] = '\0';
        }
    }
    

}
