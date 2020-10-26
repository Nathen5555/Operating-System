#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"

#include "linkage.h" //REMOVE LATER

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* exception_test 
 * 
 * Tests the exceptions by dereferencing a null pointer.
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 */

int exception_test(){
	TEST_HEADER;
	int *res;
	int a = 1;
	res = &a;
	res = NULL;
	int exception = *res;
	return exception;
}


/* divide_test
 * 
 * Tests the exceptions by dividing by zero.
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 */

int divide_test(){
	TEST_HEADER;
	//return 1 / 0;
	return 0;
}

/* system_call_2
 * 
 * Tests the system call using the int 0x80 instruction.
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 */

int system_call_2(){
	asm volatile( 
          "movl $10, %eax;"
		  "int $0x80"
    );
	return 0;
}

/* Paging Out of Bounds Test
 *
 * Checks if an invalid memory address can be accessed
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Files: paging.h/c
 */

int paging_oob(){
	int * invalidInt = (int*)(0xB8000 - 1);
	int x;
	x = *invalidInt;
	return FAIL;
}

/* Paging In Bounds Test
 *
 * Checks if an valid memory address can be accessed
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Files: paging.h/c
 */

int paging_ib(){
	int * validInt = (int*)(0xB8000 + 1);
	int x;
	x = *validInt;
	return PASS;
}



// add more tests here

/* Checkpoint 2 tests */
int rtc_test(){
	TEST_HEADER;
	int32_t i, f = 2;
	int32_t *f_ptr = &f;
	while(f <= 1024){
		rtc_write(0, f_ptr, 4);
		for(i = 0; i < 10; i++){
			rtc_read(0,0,0);
			add_to_kdb_buf('1');
		}
		*f_ptr *= 2;
		add_to_kdb_buf('\n');
		clear();

	}
	return PASS;
}

int terminal_test(){
	TEST_HEADER;
	//while(1){	
		char buf[128];
		terminal_read(0,buf,128);
		terminal_write(0,buf,128);
		return PASS;
	//}
}
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */

/* Test suite entry point */
void launch_tests(){
	//TEST_OUTPUT("idt_test", idt_test());
	//TEST_OUTPUT("Dereference NULL test", exception_test());
	//TEST_OUTPUT("divide-by-zero test", divide_test());
	//TEST_OUTPUT("system call test 2", system_call_2());
	//TEST_OUTPUT("paging test 1", paging_oob());	
	//TEST_OUTPUT("paging test 2", paging_ib());	
	//TEST_OUTPUT("rtc test", rtc_test());
	TEST_OUTPUT("terminal r/w", terminal_test());
	// launch your tests here
}





