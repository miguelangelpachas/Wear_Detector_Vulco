/*
 * detector_desgaste_emisor.c
 *
 * Created: 13/11/2020 17:46:05
 * Author : mtorres
 */ 


/* File inclusion */
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "project_defines.h"
#include <util/delay.h>
#include "MT_delay.h"
#include "global.h"
#include "servo_driver.h"
#include "MT_uart.h"
#include "bluetooth_interface.h"
#include "xbee_interface.h"
#include "MT_twi_master.h"
#include "MT_rtc_pcf8523.h"
#include "MT_24AAxx.h"
#include "analog_measures.h"
#include "serial_communication.h"
#include "external_interrupts.h"
#include "grinding_mill_wearing_out_monitor_node.h"


/* Variable definition */
volatile uint16_t system_flags = 0;
volatile uint8_t system_operation_mode = SYSTEM_STAND_BY_MODE;

/* Main function */
int main(void)
{	
	uint8_t i2c_error = I2C_OK;
	
	/* Initialize system */
	System_Initialize();
	
	#if(PRIMERA_PROGRAMACION == 1)
	/* Configure systemfor the first time */
	i2c_error = System_First_Time_Configuration();
	#endif
	
	/* Configure RTC (just in case it was reset)) */
	i2c_error = RTC_Boot_Time_Configuration();
	/* Enable external interrupts */
	Test_Switch_Interrupt_Enable();
	RTC_Interrupt_Enable();
	
	/* Enable the System Power-Down Mode */
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	
    /* Replace with your application code */
    while (1) 
    {
	
		/* Enter to Power-Down Mode */
		Microcontroller_Peripherals_Disable();
		sleep_cpu();
		Microcontroller_Peripherals_Enable();
		
		switch(system_operation_mode){
			
			case SYSTEM_XBEE_TEST_MODE:
			
				RTC_Interrupt_Disable();
				Test_Switch_Interrupt_Disable();
				Xbee_Test_Mode_Cycle();
				cli();
				system_operation_mode = SYSTEM_STAND_BY_MODE;
				sei();
				Test_Switch_Interrupt_Enable();
				RTC_Interrupt_Enable();
				break;
				
			case SYSTEM_BLUETOOTH_TEST_MODE:
			
				RTC_Interrupt_Disable();
				Test_Switch_Interrupt_Disable();
				Bluetooth_Test_Mode_Cycle();
				cli();
				system_operation_mode = SYSTEM_STAND_BY_MODE;
				sei();
				Test_Switch_Interrupt_Enable();
				RTC_Interrupt_Enable();
				break;	
	
			case SYSTEM_PERIODIC_MEASURE_MODE:
				
				Test_Switch_Interrupt_Disable();
				RTC_Interrupt_Disable();
				Xbee_Periodic_Measure_Mode_Cycle();
				cli();
				system_operation_mode = SYSTEM_STAND_BY_MODE;
				sei();
				RTC_Interrupt_Enable();
				Test_Switch_Interrupt_Enable();
				break;
		
			default:
				break;
		
		}
			
	}
	
	return 0;
}


/* Timer0 interrupts every 1ms */
ISR(TIMER0_COMPA_vect){
	Timeout_Routine_ISR();
}

/* Timer1 interrupts every time a PWM signal for the servo is completed */
ISR(TIMER1_COMPA_vect){
	Servo_Auto_Disable_ISR();
}

/* USART0 interrupts every time it receives a character */
ISR(USART0_RX_vect){
	
	static uint8_t command_ending_counter = 0;
	uint8_t caracter;
	
	caracter = UDR0;
	Serial_Rx_Buffer_Store_Byte(caracter);
	
	/*  Detect end of frame from the central (every frame ends with "\n\n" ) */
	if(command_ending_counter == 0){
		if(caracter == '\r'){
			command_ending_counter++;
		}
	}else if(command_ending_counter == 1){
		if(caracter == '\n'){
			command_ending_counter++;
		}else{
			command_ending_counter = 0;
		}	
	}else{
		if(caracter == '\n'){
			system_flags |= (1 << XBEE_COMM_RESPONSE_FLAG);
			command_ending_counter = 0;
		}else{
			command_ending_counter = 0;
		}		
	}
}


/* USART1 interrupts every time it receives a character */
ISR(USART1_RX_vect){
	
	static uint8_t command_ending_counter = 0;
	uint8_t caracter;
	
	caracter = UDR1;
	Serial_Rx_Buffer_Store_Byte(caracter);
	
	/*  Detect end of frame from the central (every frame ends with "\n\n" ) */
	if(command_ending_counter == 0){
		if(caracter == '\r'){
			command_ending_counter++;
		}
	}else if(command_ending_counter == 1){
		if(caracter == '\n'){
			command_ending_counter++;
		}else{
			command_ending_counter = 0;
		}
	}else{
		if(caracter == '\n'){
			system_flags |= (1 << BT_COMM_RESPONSE_FLAG);
			command_ending_counter = 0;
		}else{
			command_ending_counter = 0;
		}
	}
}




/* External interrupt when the reed switch TEST_XBEE generates a falling edge */
ISR(INT0_vect){
	
	if(!(PIN_TEST_BT & (1 << TEST_BT))){
		system_operation_mode = SYSTEM_BLUETOOTH_TEST_MODE;
	}else{
		system_operation_mode = SYSTEM_XBEE_TEST_MODE;
	}
	
	
	while(!(PIN_TEST_XBEE & (1 << TEST_XBEE))){
		/* Wait until the reed switch is released */	
	}
	
}


/* External interrupt when RTC generates a falling edge at its pin INT */
ISR(INT1_vect){
	system_operation_mode = SYSTEM_PERIODIC_MEASURE_MODE;

	while(!(PIN_RTC_EXT_INT & (1 << RTC_EXT_INT))){
		/* Wait until the signal is back to HIGH level */	
	}

}

