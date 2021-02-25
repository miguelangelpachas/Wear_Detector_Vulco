

#ifndef SERVO_DRIVER_H_
#define SERVO_DRIVER_H_

/* File inclusion */
#include <stdint.h>
#include "project_defines.h"
#include "global.h"

/* Type definition */

typedef enum{
	RES_WORE_OUT_25_PERCENT = 416,//350,
	RES_WORE_OUT_50_PERCENT = 1000,//890,//600,
	RES_WORE_OUT_75_PERCENT = 750,//1175,//890,
	LOW_BATTERY_INDICATOR = 1083//1175
}Indicator_Position_t;


/* Function declaration */

void Servo_Initialize(void);
void Servo_Set_Position(Indicator_Position_t pos);
void Servo_On(void);
void Servo_Off(void);
void Servo_Start(void);
void Servo_Stop(void);


/* Inline functions */

static inline void Servo_Auto_Disable_ISR(void){
	
	static uint16_t conta_20ms;
	
	conta_20ms++;
	
	/* Wait 2 sec so the servo reaches the desired position */
	if(conta_20ms >= 50){
		conta_20ms = 0;
		Servo_Stop();
		system_flags |= (1 << SERVO_POS_COMPLETE_FLAG);
	}
}

#endif
