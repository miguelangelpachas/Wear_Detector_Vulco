
/* File inclusion */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "MT_timer.h"
#include "servo_driver.h"

/* Function definition */

void Timer1_Initialize(void){
	
	Timer_16b_t	my_timer;
	
	my_timer.clock = Timer_Clk_Disabled;
	my_timer.mode = Timer_16b_Fast_PWM_OCRA_Mode;
	my_timer.OCRA = 9999;
	my_timer.OCRB = 0;
	my_timer.ICR = 0;
	my_timer.OCA = OC_Disabled;
	my_timer.OCB = OC_Disabled;
	my_timer.interrupt_mask = Timer_CompA_IE;
	
	Timer1_Configurar(&my_timer);
}


void Servo_Initialize(void){
	
	/* Initialize the flag */
	system_flags |= (1 << SERVO_POS_COMPLETE_FLAG);
	
	/* Disable the power supply to "brake" the servomotor  */
	DDR_SERVO_ON |= (1<< SERVO_ON);
	PORT_SERVO_ON &= ~(1 << SERVO_ON);
	
	/* Force the PWM output to LOW */
	DDR_SERVO_PWM |= (1 << SERVO_PWM);
	PORT_SERVO_PWM &= ~(1 <<SERVO_PWM);
	
	/* Configure Timer1 - OC1B as PWM with 20ms period */ 
	Timer1_Initialize();	
}


void Servo_Set_Position(Indicator_Position_t pos){
	OCR1B = pos;
}


void Servo_Start(void){
	/* Enable servo PWM and power supply */
	PORT_SERVO_ON |= ( 1<< SERVO_ON);
	TCCR1A &= ~(1 << COM1B0);
	TCCR1A |= (1 << COM1B1);
	
	/* Reset and start Timer1 */
	TCNT1 = 0x0000;
	TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10)); 
	TCCR1B |= (1 << CS11);
	
	/* Initialize the flag */
	cli();
	system_flags &= ~(1 << SERVO_POS_COMPLETE_FLAG);
	sei();
}


void Servo_Stop(void){
	/* Stop and reset Timer1 */
	TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));
	TCNT1 = 0x0000;
	
	/*Disable th servo PWM signal and power supply*/
	PORT_SERVO_PWM &= ~(1 << SERVO_PWM);
	TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0));
	PORT_SERVO_ON &= ~(1 << SERVO_ON);
	
	/* Set the flag */
	system_flags |= (1 << SERVO_POS_COMPLETE_FLAG);
}

void Servo_On(void){
	PORT_SERVO_ON |= ( 1<< SERVO_ON);	
}


void Servo_Off(void){
	PORT_SERVO_ON &= ~(1 << SERVO_ON);	
}