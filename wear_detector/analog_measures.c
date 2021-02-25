
/* File inclusion */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "project_defines.h"
#include "global.h" 
#include  "MT_delay.h"
#include <stdint.h> 
#include "MT_adc.h"
#include "analog_measures.h"

/* Variable definition */

// Binary value of the analog measures
volatile uint32_t analog_binary_accumulator[N_ANALOG_INPUTS] = {0, 0, 0};
volatile uint16_t analog_binary_value[N_ANALOG_INPUTS] = {0, 0, 0};
volatile uint8_t adc_channel_index = 0;
volatile uint16_t round_counter = 0;	

// Indexes for every analog channel in use
const uint8_t analog_channel_index[N_ANALOG_INPUTS] = {RES1_SENSOR_ADC_CHANNEL, RES2_SENSOR_ADC_CHANNEL, BATTERY_LEVEL_ADC_CHANNEL};

// Measured values of res1, res2 and battery
float res1_val_ohm = 0;
float res2_val_ohm = 0;
float battery_val_voltage = 0;
float battery_val_percent = 0;


/* Function definition */

void Analog_Measures_Initialize(void){
 
	/* Disable the analog block power supply */
	DDR_ANALOG_MEASURE_ON |= (1 << ANALOG_MEASURE_ON);
	PORT_ANALOG_MEASURE_ON &= ~(1 << ANALOG_MEASURE_ON);
	
	/* Configure the 10-bit ADC */
	ADC_Initialize();		
}


void Analog_Measures_On(void){
	
	/* Enable the analog block power supply */
	PORT_ANALOG_MEASURE_ON |= (1 << ANALOG_MEASURE_ON);

}

void Analog_Measures_Start(void){

	/* Reset channel index and accumulator */
	ADC_Channel_Index_Reset();
	Analog_Accumulator_Reset();
	
	/* Perform a first conversion */
	ADCSRA |= (1 << ADSC);
	while(!(ADCSRA & (1 << ADIF))){
		// Wait until the conversion is complete
	}
	
	/* Clear the ADC conversion complete flag */
	ADCSRA |= (1 << ADIF);
	
}



void Analog_Measures_Off(void){
	/* Disable the analog block power supply */
	PORT_ANALOG_MEASURE_ON &= ~(1 << ANALOG_MEASURE_ON);
}


void ADC_Initialize(void){
	
	ADC_t my_adc;
	
	my_adc.prescaler = ADC_Prescaler_32;
	my_adc.channel = RES1_SENSOR_ADC_CHANNEL;
	my_adc.vref = Internal_1p1_Bandgap;
	my_adc.trigger_source = ADC_Free_Running_Mode;
	my_adc.auto_trigger = ADC_Autotrigger_Disabled;
	my_adc.interrupt_mask =  ADC_Interrupt_Disabled;
	
	ADC_Configurar(&my_adc);
	
	/* Disable the digital ports that will not be used */
	DIDR0 |= (1 << RES1_SENSOR_ADC_CHANNEL) | (1 << RES2_SENSOR_ADC_CHANNEL) | (1 << BATTERY_LEVEL_ADC_CHANNEL);
	DIDR0 |= (1 << 3) | (1 << 7);
	
	/* Perform a first conversion */
	ADCSRA |= (1 << ADSC);
	while(!(ADCSRA & (1 << ADIF))){
		// Wait until the conversion is complete	
	}
	
	/* Clear the ADC conversion complete flag */
	ADCSRA |= (1 << ADIF);

}


void ADC_Channel_Index_Reset(void){	
	adc_channel_index = 0;	
}


void Analog_Accumulator_Reset(void){
	
	uint8_t index;
	
	for(index = 0; index < N_ANALOG_INPUTS; index++){
		analog_binary_accumulator[index] = 0;
	}
}


uint8_t Analog_Measure_Read_And_Channel_Update(void){
	
	uint8_t round_complete = 0;
	
	/* Store the converted value */
	analog_binary_accumulator[adc_channel_index] += ADC;
	
	adc_channel_index++;
	if(adc_channel_index >= N_ANALOG_INPUTS){
		adc_channel_index = 0;
		round_complete = 1;
	}
	
	
	/* Update the ADC channel */
	ADC_Seleccionar_Canal(analog_channel_index[adc_channel_index]);
	
	return round_complete;
}


void Analog_Measure_And_Accumulate(void){
	
	uint8_t round_complete = 0;
	
	for(round_counter = 0; round_counter < N_SAMPLES_PER_CHANNEL; round_counter++){	
		do{
			Analog_Measure_New_Sample();
			Delay_ms(10);
			round_complete = Analog_Measure_Read_And_Channel_Update();
		}while(round_complete == 0);
	}
}



void Analog_Measure_Calculate_Average(void){
	
	uint8_t index;
	
	for(index = 0; index < N_ANALOG_INPUTS; index++){
		analog_binary_value[index] = analog_binary_accumulator[index] / N_SAMPLES_PER_CHANNEL;
		analog_binary_accumulator[index] = 0;
	}
}



void Analog_Measure_New_Sample(void){
	
	ADCSRA |= (1 << ADSC);
	while(!(ADCSRA & (1 << ADIF))){
		// Wait until the conversion is complete
	}
}



void Calc_Res1_Ohm_From_Bin(void){

	float r_sens_ohm, r_fix_ohm;
	float v_in_volt = 3.2;
	float v_adc_volt;
	
	r_fix_ohm = 1000; 
	v_adc_volt = ((float)analog_binary_value[RES1_MEASURE_INDEX] * 1.1) / 1024;
	v_adc_volt /= RES1_MEASURE_GAIN;
	r_sens_ohm = (v_adc_volt * r_fix_ohm) / (v_in_volt - v_adc_volt);
	
	if(r_sens_ohm > RES1_SENSE_MAX_VALUE_OHM){
		res1_val_ohm = RES1_SENSE_MAX_VALUE_OHM;
	}else if(r_sens_ohm < RES1_SENSE_MIN_VALUE_OHM){
		res1_val_ohm = RES1_SENSE_MIN_VALUE_OHM;
	}else{
		res1_val_ohm = r_sens_ohm;
	}
}


void Calc_Res2_Ohm_From_Bin(void){

	float r_sens_ohm, r_fix_ohm;
	float v_in_volt = 3.2;
	float v_adc_volt;
	
	r_fix_ohm = 1000;
	v_adc_volt = ((float)analog_binary_value[RES2_MEASURE_INDEX] * 1.1) / 1024.0;
	v_adc_volt /= RES2_MEASURE_GAIN;
	r_sens_ohm = (v_adc_volt * r_fix_ohm) / (v_in_volt - v_adc_volt);
	
	if(r_sens_ohm > RES2_SENSE_MAX_VALUE_OHM){
		res2_val_ohm = RES2_SENSE_MAX_VALUE_OHM;
	}else if(r_sens_ohm < RES2_SENSE_MIN_VALUE_OHM){
		res2_val_ohm = RES2_SENSE_MIN_VALUE_OHM;
	}else{
		res2_val_ohm = r_sens_ohm;
	}
}


void Calc_Battery_Percent_From_Bin(void){
	
	float batt_v, batt_percent;
			
	batt_v = ((float)analog_binary_value[BATTERY_MEASURE_INDEX] * 1.1) / 1024.0;
	batt_v /= BATT_CIRCUIT_GAIN;
	
	battery_val_voltage = batt_v;
	
	batt_percent = ((batt_v - BATT_SENSE_MIN_VALUE_VOLT) * 100) / (BATT_SENSE_MAX_VALUE_VOLT - BATT_SENSE_MIN_VALUE_VOLT);

	if(batt_percent > 100.0){
		battery_val_percent = 100.0;
	}else if(batt_percent < 0.0){
		battery_val_percent = 0;
	}else{
		battery_val_percent = batt_percent;
	}

}


float Get_Res1_Ohm(void){
	return res1_val_ohm;	
}


float Get_Res2_Ohm(void){
	return res2_val_ohm;
}


float Get_Battery_Volt(void){
	return battery_val_voltage;
}


float Get_Battery_Percent(void){
	return battery_val_percent;
}

