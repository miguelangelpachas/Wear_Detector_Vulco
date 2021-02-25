/*
 * external_interrupts.c
 *
 * Created: 16/12/2020 12:01:18
 *  Author: mtorres
 */ 

#include <avr/io.h>
#include "external_interrupts.h"


void Test_Switch_Interrupt_Initialize(void){
	/* Enable interrupt by 0 Level in INT0 pin */
	EICRA &= ~(1 << ISC00);
	EICRA &= ~(1 << ISC01);
	EIMSK &= ~(1 << INT0);	
}


void Test_Switch_Interrupt_Enable(void){
	EIFR |= (1 << INTF0);
	EIMSK |= (1 << INT0);	
}


void Test_Switch_Interrupt_Disable(void){
	EIMSK &= ~(1 << INT0);	
}


void RTC_Interrupt_Initialize(void){
	/* Enable interrupt by 0 Level in INT1 pin */
	EICRA &= ~(1 << ISC10);
	EICRA &= ~(1 << ISC11);
	EIMSK &= ~(1 << INT1);	
}


void RTC_Interrupt_Enable(void){
	EIFR |= (1 << INTF1);
	EIMSK |= (1 << INT1);	
}


void RTC_Interrupt_Disable(void){
	EIMSK &= ~(1 << INT1);	
}


