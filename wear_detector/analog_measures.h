

#ifndef ANALOG_MEASURES_H_
#define ANALOG_MEASURES_H_


/* File inclusion */
#include "stdint.h"


/* Global variables */
extern volatile uint16_t analog_binary_values[N_ANALOG_INPUTS];
extern const uint8_t analog_channel_index[N_ANALOG_INPUTS];


 /* Function declaration */ 
void Analog_Measures_Initialize(void);
void Analog_Measures_On(void);
void Analog_Measures_Start(void);
void Analog_Measures_Off(void);
void ADC_Initialize(void);
void ADC_Channel_Index_Reset(void);
void Analog_Accumulator_Reset(void);
uint8_t Analog_Measure_Read_And_Channel_Update(void);
void Analog_Measure_And_Accumulate(void);
void Analog_Measure_Calculate_Average(void);
void Analog_Measure_New_Sample(void);

void Calc_Res1_Ohm_From_Bin(void);
void Calc_Res2_Ohm_From_Bin(void);
void Calc_Battery_Percent_From_Bin(void);

float Get_Res1_Ohm(void);
float Get_Res2_Ohm(void);
float Get_Battery_Volt(void);
float Get_Battery_Percent(void);

#endif