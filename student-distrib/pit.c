#include "pit.h"
#include "shared_global_variables.h"
#include "linkage.h"
#include "execute.h"
#include "paging.h"
#include "keyboard.h"

int scheduled_terminal = 0;
int visible_terminal = 0;
int calls_to_schedule = 0;
int paging_scheme = 0;
int x = 0;
PCB_t * active_processes[3];

void initialize_pit(){
    outb(0x36, 0x43);		// enable mode 3
    // outb(0x00110110, 0x43);
    outb(11905 & 0x000000FF, 0x40); //low byte
    outb(11905 >> 8, 0x40); //high 8 bits
    int j;
    for(j = 0; j < 3; j++) {
        active_processes[j] = NULL;
    }
    enable_irq(0x0); //irq is zero
}

void pit_handler() {
    //do something with curr process tracker
    schedule();
    send_eoi(0x0);
}

void schedule() {
    calls_to_schedule++;
    if(total_processes == 0){
        execute((uint8_t *)"shell");            
    }
    if(total_processes == 1 || total_processes == 2) {
        save_ebp_esp((uint32_t)curr_pcb + ESP2_LOCATION, (uint32_t)curr_pcb + EBP2_LOCATION);
        switch_terminal(total_processes, 0);
        scheduled_terminal++;
        execute((uint8_t *)"shell");
    }
    
    save_ebp_esp((uint32_t)curr_pcb + ESP2_LOCATION, (uint32_t)curr_pcb + EBP2_LOCATION);

    if(calls_to_schedule == 4) {
        //printf("test");
        //calls_to_schedule++;
        switch_terminal(0, 1);
    }
    else {
        //save cursor
        curr_pcb->screen_x = get_x();
        curr_pcb->screen_y = get_y();
    }     

    //increment scheduled terminal number
    scheduled_terminal++;
    //if done with all active processes, go to start of active proceses
    if(scheduled_terminal == 3) {
        scheduled_terminal = 0;
    }
    if(calls_to_schedule == 4) {
        printf("scheduled terminal: %d", scheduled_terminal);
        printf("visible terminal: %d", visible_terminal);
    }


    // //switch paging for video memory
    // if(scheduled_terminal == visible_terminal) { 
    //     pageTable[VIDEO_MEMORY_IDX >> 12] = (VIDEO_MEMORY_IDX | 0x003); // 0x3 are bits needed to set present, rw, supervisor
    //     paging_scheme = 0;
    // }
    // else {
    //     pageTable[VIDEO_MEMORY_IDX >> 12] = ((VIDEO_MEMORY_IDX + (0x1000*(scheduled_terminal + 1))) | 0x003);
    //     paging_scheme = scheduled_terminal + 1;
    // }
    // flush_tlb();

    //get curr_pcb for new process
    curr_pcb = active_processes[scheduled_terminal];
    // printf("curr_pcb: %d\n", curr_pcb);

    //set cursor 
    if(calls_to_schedule != 4) {
        update_cursor(curr_pcb->screen_x, curr_pcb->screen_y);
    }

    //switch paging for user program memory
    switch_task_memory();

    //set TSS
    prepare_context_switch();

    //restore ebp and esp for newly scheduled process
    restore_ebp_esp(curr_pcb->esp2, curr_pcb->ebp2); 
}

void switch_terminal(uint32_t terminal_num, int state){
    //save cursor
    active_processes[visible_terminal]->screen_x = get_x();
    active_processes[visible_terminal]->screen_y = get_y(); 

    //save and restore video memory
    memcpy((void *) (VIDEO_MEMORY_IDX + ((0x1000*(visible_terminal + 1)))), (void *) VIDEO_MEMORY_IDX, 0x1000);
    visible_terminal = terminal_num;
    memcpy((void *)VIDEO_MEMORY_IDX, (void *) (VIDEO_MEMORY_IDX + ((0x1000*(terminal_num + 1)))), 0x1000);  

    //set cursor
    if(state == 0) {
        update_cursor(0, 0);
    }
    else {
        update_cursor(active_processes[visible_terminal]->screen_x, active_processes[visible_terminal]->screen_y);
    }
    restore_original_paging();
    send_eoi(0x01);
}
    
//writing to terminal: 
    //when the user types something, the program should write to virtual video memory using putc, which should point to physical video memory.
    //when a program writes to memory using terminal write, the virtual video memory should point to either the backup or the physical video memory.
    

 
