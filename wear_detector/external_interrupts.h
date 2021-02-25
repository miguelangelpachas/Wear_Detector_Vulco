/*
 * external_interrupts.h
 *
 * Created: 16/12/2020 12:01:52
 *  Author: mtorres
 */ 


#ifndef EXTERNAL_INTERRUPTS_H_
#define EXTERNAL_INTERRUPTS_H_

void Test_Switch_Interrupt_Initialize(void);
void Test_Switch_Interrupt_Enable(void);
void Test_Switch_Interrupt_Disable(void);

void RTC_Interrupt_Initialize(void);
void RTC_Interrupt_Enable(void);
void RTC_Interrupt_Disable(void);


#endif /* EXTERNAL_INTERRUPTS_H_ */