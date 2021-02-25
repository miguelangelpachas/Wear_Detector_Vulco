
/* Fiole inclusion */
#include "project_defines.h"
#include "global.h"
#include "MT_delay.h"
#include <avr/interrupt.h>


/* Variable definition */
volatile uint32_t timeout_limit_ms = 0;
volatile uint32_t timeout_counter_ms = 0;


/* Function definition */

void Delay_ms(uint32_t value_ms){
	
	timeout_limit_ms = value_ms;
	timeout_counter_ms = 0;
	
	cli();
	system_flags &= ~(1 << TIME_OUT_FLAG);
	sei();
	
	while(!(system_flags & (1 << TIME_OUT_FLAG))){
		// Wait until the desired time has elapsed
	}
	
}

