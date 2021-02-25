

#ifndef DELAY_MS_H_
#define DELAY_MS_H_

/* File inclusion */
#include <stdint.h>
#include "global.h"


/* Function declaration */
void Delay_ms(uint32_t value_ms);


/* Inline function definition */

static inline void Timeout_Routine_ISR(void){
	
	if(!(system_flags & (1 << TIME_OUT_FLAG))){
		timeout_counter_ms++;
		if(timeout_counter_ms >= timeout_limit_ms){
			system_flags |= (1 <<  TIME_OUT_FLAG);
		}	
	}
}

#endif