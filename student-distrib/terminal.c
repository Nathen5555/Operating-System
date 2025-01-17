#include "terminal.h"
#include "lib.h"
#include "shared_global_variables.h"
#include "paging.h"
#include "execute.h"
int terminal_write_flag[3] = {1,1,1};
volatile int entered_flag[3] = {0,0,0};
/* terminal_read
 * 
 * Description: Copies from keyboard buffer to terminal buffer the appropriate characters while maintaining the limit.
 * Inputs: Terminal buffer
 * Outputs: none
 * Side Effects: Changes terminal buffer
 * Return value: Number of bytes read
 */ 
int32_t terminal_read (int32_t fd, void* buf, int32_t nbytes){
	if(buf == NULL || nbytes <= 0) return 0;
	//clear the buffer being sent in
	//terminal_write_flag[visible_terminal] = 0;
	char * buf_ = (char *)buf;

	int i;
	for(i = 0; i < BUF_SIZE; i++){
		buf_[i] = '\0';
	}
	//entered_flag[visible_terminal] = 0;
	while(!entered_flag[scheduled_terminal]){
		//wait for newline to be entered on the scheduled terminal
	}
	entered_flag[scheduled_terminal] = 0; //resets the flag to accept another newline
	int32_t j = 0;
	while(j != BUF_SIZE){
		//copy from keyboard buf to terminal buf the appropriate characters
		if(j == nbytes) break; //reached the maximum bytes acceptable
		if(kbd_buf[scheduled_terminal][j] == '\0') continue; //return to beginning of loop		
		buf_[j] = kbd_buf[scheduled_terminal][j];
		if(buf_[j] == '\n'){
			j++;
			break;
		}
		j++; //not newline
	}
	//terminal_write_flag[visible_terminal] = 1;
	return j; //returns number of bytes
}

/* terminal_write
 * 
 * Description: Writes from keyboard buf to screen
 * Inputs: Terminal buffer, number of bytes to write
 * Outputs: none
 * Side Effects: Writes to screen
 * Return value: number of bytes written
 */ 
int32_t terminal_write (int32_t fd, const void* buf, int32_t nbytes){
	//write to scheduled terminal
	if(buf == NULL || nbytes <= 0) return -1; //no bytes to read
	
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	uint32_t i, counter = 0;
	char * buf_ = (char *)buf;
	// if(scheduled_terminal == visible_terminal){
	// 	//write to screen
	// 	putcTerminalW();
	// }
	// else{
	// 	//putc to backup
	// }
	//pageTable[VIDEO_MEMORY_IDX >> 12] = ((VIDEO_MEMORY_IDX + (0x1000*(scheduled_terminal + 1))) | 0x003);
	//switch paging for video memory
	// active_processes[scheduled_terminal]->screen_x = get_x();
	// active_processes[scheduled_terminal]->screen_y = get_y();
	for(i = 0; i < nbytes; i++){
		//if(i == BUF_SIZE) break; //iterates until reaches max size of buffer or the number of bytes
		if(buf_[i] != '\0'){ //will ignore NULL
			//terminal_write_flag = 1;
			putcTerminalW(buf_[i]);
			
			//putcTerminalW(buf_[i]); //write to screen
			counter++;
		} 
	}
	//terminal_write_flag = 0;
	for(i = 0; i < BUF_SIZE; i++){
		//change back to scheduled
		kbd_buf[scheduled_terminal][i] = '\0'; //reset keyboard buf
	}
	set_boundary();
	// curr_pcb->screen_x = get_x();
	// curr_pcb->screen_y = get_y();
	
    return counter; //number of bytes read
}
/* terminal_open
 * 
 * Description: Opens terminal
 * Inputs: None used
 * Outputs: none
 * Side Effects: none
 * Return value: Zero on success
 */ 
int32_t terminal_open (const uint8_t* filename){
    return 0;
}
/* terminal_close
 * 
 * Description: Closes terminal
 * Inputs: None used
 * Outputs: none
 * Side Effects: None
 * Return value: Zero on success
 */ 
int32_t terminal_close (int32_t fd){
    return -1;
}
