
	
/* File inclusion */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include "project_defines.h"
#include <util/delay.h>
#include "MT_delay.h"
#include "MT_timer.h"
#include "MT_twi_master.h"
#include "MT_rtc_pcf8523.h"
#include "MT_24AAxx.h"
#include "servo_driver.h"
#include "bluetooth_interface.h"
#include "xbee_interface.h"
#include "analog_measures.h"
#include "grinding_mill_wearing_out_monitor_node.h"
#include "serial_communication.h"
#include "eeprom_manager.h"
#include "external_interrupts.h"


/* Variable definition */

/* Store constant strings in the Flash Memory */
const char part_number_by_default[] = PART_NUMBER_STR_BY_DEFAULT;
const char emitter_node_id_by_default[] = EMITTER_NODE_ID_STR_BY_DEFAULT;
const char receiver_central_id_by_default[] = RECEIVER_CENTRAL_ID_STR_BY_DEFAULT;


/* Numeric values taken from the EEPROM */
volatile uint8_t servo_indicator_status = SERVO_INDICATOR_ENABLED;
volatile uint8_t number_of_sensors = NUMBER_OF_SENSORS_BY_DEFAULT;
volatile uint16_t this_node_id = NODE_ID_NUMBER_BY_DEFAULT;
volatile uint8_t measurement_event_counter = 0; 
volatile uint8_t measure_period_in_days = MEASURE_PERIOD_IN_DAYS_BY_DEFAULT;

/* Data numeric values */
uint8_t date_buffer[3] = {0, 0, 0};
uint8_t time_buffer[4] = {0, 0, 0, 0};
uint16_t measure_buffer[3] = {0, 0, 0};

resistor_status_t r1_status = Resistor_Open;	
resistor_status_t r2_status = Resistor_Open;

sensor_status_t sensor_status =  Wear_Out_Higher_Than_75;

battery_status_t battery_status = Battery_Low;

/* Data values in ASCII */
char part_number_str[PART_NUMBER_SIZE_IN_BYTES];
char emitter_node_id_str[EMITTER_NODE_ID_SIZE_IN_BYTES];
char receiver_central_id_str[RECEIVER_CENTRAL_ID_SIZE_IN_BYTES];
char date_str[DATA_FRAME_DATE_SIZE_IN_BYTES];
char time_str[DATA_FRAME_TIME_SIZE_IN_BYTES];
char wear_out_state_str[WEAR_OUT_STATE_SIZE_IN_BYTES];
char battery_level_str[BATTERY_LEVEL_SIZE_IN_BYTES];
char emitter_resistance_1_str[EMITTER_OHM1_SIZE_IN_BYTES];
char emitter_resistance_2_str[EMITTER_OHM2_SIZE_IN_BYTES];
char measure_period_in_days_str[MEASURE_PERIOD_STR_SIZE_IN_BYTES];


/* Node data frame (madeof pointers) */
data_frame_t node_data_frame;

/* Servo previous position */
uint16_t servo_previous_position = LOW_BATTERY_INDICATOR;


/* Function definition */

void Ext_Int_Pin_Initialize(void){
	
	/* Enable the pull-up resistor for the RTC interrupt pin */
	
	DDR_RTC_EXT_INT &= ~(1 << RTC_EXT_INT);
	PORT_RTC_EXT_INT |= (1 << RTC_EXT_INT);
	
	/* Enable the pull-up resistor for the reed switches pins */
	
	//DDR_TEST_BT &= ~(1 << TEST_BT);
	//PORT_TEST_BT |= (1 << TEST_BT);
	
	//DDR_TEST_BT &= ~(1 << TEST_BT);
	//PORT_TEST_BT |= (1 << TEST_BT);	
}


void TWI0_Master_Initialize(void){
	
	twi_config_t my_twi;

	/* Configure the TWI module to use an SCL of 200 KHz frequency */	
	my_twi.twbr = 2;
	my_twi.twps = SCL_Prescaler_1;

	TWI0_Configurar(&my_twi);	
}


void Boost_PSU_Initialize(void){
	DDR_BOOST_PSU_ON |= (1 << BOOST_PSU_ON);
	PORT_BOOST_PSU_ON &= ~(1 << BOOST_PSU_ON);
}


void Boost_PSU_Power_On(void){
	PORT_BOOST_PSU_ON |= (1 << BOOST_PSU_ON);
}


void Boost_PSU_Power_Off(void){
	TWCR0 &= ~(1 << TWEN);
	PORT_BOOST_PSU_ON &= ~(1 << BOOST_PSU_ON);
}


void System_Initialize(void){
	
	///* Initialize auxiliar signal */
	//DDR_AUX |= (1 << AUX);
	//PORT_AUX &= ~(1 << AUX);
	/* Disable Watch Dog Timer */
	//WDTCSR = 0x00;
	/* Disable analog comparator */
	ACSR |= (1 << ACD);
	ACSR &= ~(1 << ACBG);
	/* Configure digital pins for control of power supplies */
	Boost_PSU_Initialize();
	Analog_Measures_Initialize();
	/* Configure external interrupt pins as inputs and enable pull-up resistors
	 when required */
	Ext_Int_Pin_Initialize();
	/* Configure external interrupts */
	Test_Switch_Interrupt_Initialize();
	RTC_Interrupt_Initialize();
	/* Configure serial communication modules */
	TWI0_Master_Initialize();
	Xbee_Interface_Initialize();
	Bluetooth_Interface_Initialize();	
	/* Configure Timer0 (used to control ADC) and Timer1(used to control servo) */
	Timer0_Initialize();
	/* Disconnect servomotor */
	Servo_Initialize();
	/* Wait for a moment before turning on the boost power supply*/
	_delay_ms(1000);
	/* Enable interrupts */
	sei();
}



uint8_t Prepare_Data_For_Frame(void){

	uint8_t i2c_error = I2C_OK;

	/* Read date from RTC */
	
	i2c_error = System_Date_Update();
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}
	
	/* Read time from RTC */
	i2c_error = System_Time_Update();
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}
	
	/* Constrain analog values to integer */
	measure_buffer[RES1_MEASURE_INDEX] = (uint16_t)Get_Res1_Ohm();
	measure_buffer[RES2_MEASURE_INDEX] = (uint16_t)Get_Res2_Ohm();
	measure_buffer[BATTERY_MEASURE_INDEX] = (uint16_t)(Get_Battery_Percent());
	
	return i2c_error;
}



uint8_t Data_Frame_Build(void){
	
	uint8_t i2c_error = I2C_OK;
	
	/* Load the dynamic parameters from the EEPROM and build strings */
	
	i2c_error = EEPROM_Load_Number_Of_Sensors((uint8_t *)&number_of_sensors);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}
	
	i2c_error = EEPROM_Load_Measure_Period_In_Days((uint8_t *)&measure_period_in_days);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}
	
	/* Load the static parameters from the EEPROM and build strings */
	
	i2c_error = EEPROM_Load_Part_Number((uint8_t *)part_number_str);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}	

	i2c_error = EEPROM_Load_Emitter_Node_ID((uint8_t *)emitter_node_id_str);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}

	this_node_id = (emitter_node_id_str[EMITTER_NODE_ID_SIZE_IN_BYTES - 3] - '0') + (10 * (emitter_node_id_str[EMITTER_NODE_ID_SIZE_IN_BYTES - 4] - '0'));
	
	i2c_error = EEPROM_Load_Receiver_Central_ID((uint8_t *)receiver_central_id_str);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}
	
	/* Build measure period string */
	sprintf(measure_period_in_days_str, "\"%02d\"", measure_period_in_days);
	measure_period_in_days_str[4] = '\0';
	
	/* Build date string and time string */ 
	sprintf(date_str, "\"%02d/%02d/%04d\"", date_buffer[0], date_buffer[1], 2000 + date_buffer[2]);
	date_str[12] = '\0';
	
	if(time_buffer[3] == AM){
		sprintf(time_str, "\"%02d:%02d a.m.\"", time_buffer[0], time_buffer[1]);
	}else{
		sprintf(time_str, "\"%02d:%02d p.m.\"", time_buffer[0], time_buffer[1]);
	}
	time_str[12] = '\0';

	/* Build sensor status string */
    sprintf(wear_out_state_str, "\"%d\"", (uint8_t)sensor_status);
	wear_out_state_str[3] = '\0';

	/* Build battery charge state string */
	sprintf(battery_level_str, "\"%03d\"", measure_buffer[BATTERY_MEASURE_INDEX]);
	battery_level_str[5] = '\0';

	/* Build resistive values strings */
	
	if(number_of_sensors == 1){
		sprintf(emitter_resistance_1_str, "\"%03d\"", measure_buffer[RES1_MEASURE_INDEX]);
		emitter_resistance_1_str[5] = '\0';
		sprintf(emitter_resistance_2_str, "\" - \"");
		emitter_resistance_2_str[5] = '\0';		
	}else if(number_of_sensors == 2){
		sprintf(emitter_resistance_1_str, "\"%03d\"", measure_buffer[RES1_MEASURE_INDEX]);
		emitter_resistance_1_str[5] = '\0';
		sprintf(emitter_resistance_2_str, "\"%03d\"", measure_buffer[RES2_MEASURE_INDEX]);
		emitter_resistance_2_str[5] = '\0';		
	}else{
		sprintf(emitter_resistance_1_str, "\"   \"");
		emitter_resistance_1_str[5] = '\0';
		sprintf(emitter_resistance_2_str, "\"   \"");
		emitter_resistance_2_str[5] = '\0';		
	}

	/* Select the proper message (string) for the state */ 
	node_data_frame.part_number_str_ptr = part_number_str;
	node_data_frame.emitter_node_id_str_ptr = emitter_node_id_str;
	node_data_frame.receiver_central_id_str_ptr = receiver_central_id_str;
	node_data_frame.period_str_ptr = measure_period_in_days_str;
	node_data_frame.date_str_ptr = date_str;
	node_data_frame.time_str_ptr = time_str;
	node_data_frame.sensor_status_str_ptr = wear_out_state_str;
	node_data_frame.battery_str_ptr = battery_level_str;
	node_data_frame.ohm1_str_ptr = emitter_resistance_1_str;
	node_data_frame.ohm2_str_ptr = emitter_resistance_2_str;

	return i2c_error;
}


uint8_t System_Date_Update(void){
	
	uint8_t i2c_error  = I2C_OK;
	
	i2c_error  = RTC_Date_Read();
	RTC_Get_Date(date_buffer);
	
	return i2c_error ;
}


uint8_t System_Time_Update(void){
	
	uint8_t error_i2c = I2C_OK;
	
	error_i2c = RTC_Time_Read_12h();
	RTC_Get_Time_12h(time_buffer);
	
	return error_i2c;
}



void Timer0_Initialize(void){
	
	Timer_8b_t my_timer;
	
	my_timer.clock = Timer_Clk_Disabled;
	my_timer.mode = Timer_8b_CTC_Mode;
	my_timer.OCRA = 62;
	my_timer.OCRB = 0;
	my_timer.OCA = OC_Disabled;
	my_timer.OCB = OC_Disabled;
	my_timer.interrupt_mask = Timer_Interrupts_Disabled;
	
	Timer0_Configurar(&my_timer);
	
}


void Timer0_On(void){
	
	cli();
	system_flags |= (1 << TIME_OUT_FLAG);
	sei();
	
	/* Configure Timer0 to interrupt every 1ms */
	TIMSK0 |= (1 << OCIE0A);
	TIFR0 |= (1 << OCF0A);
	TCCR0B &= ~(1 << CS02);
	TCCR0B |= ((1 << CS01) | (1 << CS00));

}


void Timer0_Off(void){
	
	/* Disable Timer0 and its interrupt */
	TCCR0B &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));
	TIFR0 |= (1 << OCF0A);
	TIMSK0 &= ~(1 << OCIE0A);
}



void Analog_Measure_Task(void){
	
	/* Reset counter and accumulator */
	Analog_Measures_Start();
	
	/* Take samples */
	Analog_Measure_And_Accumulate();
	
	/* Calculate the average of measures in order to filter noise */
	Analog_Measure_Calculate_Average();
	
	/* Calculate wearing off sensors in OHM and battery level in PERCENTAGE */
	Calc_Res1_Ohm_From_Bin();
	Calc_Res2_Ohm_From_Bin();
	Calc_Battery_Percent_From_Bin();	
}



uint8_t Resistor_Sensor_Status_Calculation(void){
	
	uint8_t i2c_error  = I2C_OK;
	
	i2c_error = EEPROM_Load_Number_Of_Sensors((uint8_t *)&number_of_sensors);
	if(i2c_error  == I2C_ERROR){
		return i2c_error;
	}
	
	if(number_of_sensors == 1){
		One_Res_Sensor_Status_Calculation();
	}else if(number_of_sensors == 2){
		Two_Res_Sensor_Status_Calculation();
	}else{
		// Does nothing
	}
	
	return i2c_error;
}



void One_Res_Sensor_Status_Calculation(void){
	
	static resistor_status_t prev_r1_status = Resistor_Open;
	uint16_t res1_val;
	
	/* First, determine the state of the sensing resistors (with hysteresis) */
	
	res1_val = measure_buffer[RES1_MEASURE_INDEX];
	
	if(prev_r1_status == Resistor_Open){
		if(res1_val <= (RES1_SENSE_THRESHOLD_OHM - RES_SENSOR_HYSTERESIS_VALUE)){
			r1_status = Resistor_Closed;
		}else{
			r1_status = Resistor_Open;
		}
		
	}else if(prev_r1_status == Resistor_Closed){
		if(res1_val >= RES1_SENSE_THRESHOLD_OHM){
			r1_status = Resistor_Open;
		}else{
			r1_status = Resistor_Closed;
		}	
	}else{
		// Does nothing
	}
	
	prev_r1_status = r1_status;
	
	/* Then, determine the status of the wearing out sensor */
	if(r1_status == Resistor_Closed){
		sensor_status = Wear_Out_Lower_Than_50;
	}else if(r1_status == Resistor_Open){
		sensor_status = Wear_Out_Higher_Than_75;
	}else{
		// Does nothing
	}	
	
}


void Two_Res_Sensor_Status_Calculation(void){

	static resistor_status_t prev_r1_status = Resistor_Open;
	static resistor_status_t prev_r2_status = Resistor_Open;
	
	uint16_t res1_val, res2_val;
	
	res1_val = measure_buffer[RES1_MEASURE_INDEX];
	res2_val = measure_buffer[RES2_MEASURE_INDEX];
	
	/* First, determine the state of the sensing resistors (with hysteresis) */
	
	if(prev_r1_status == Resistor_Open){
		if(res1_val <= (RES1_SENSE_THRESHOLD_OHM - RES_SENSOR_HYSTERESIS_VALUE)){
			r1_status = Resistor_Closed;
			}else{
			r1_status = Resistor_Open;
		}
	}else{
		if(res1_val >= RES1_SENSE_THRESHOLD_OHM ){
			r1_status = Resistor_Open;
			}else{
			r1_status = Resistor_Closed;
		}
	}
	
	
	if(prev_r2_status == Resistor_Open){
		if(res2_val <= (RES2_SENSE_THRESHOLD_OHM - RES_SENSOR_HYSTERESIS_VALUE)){
			r2_status = Resistor_Closed;
			}else{
			r2_status = Resistor_Open;
		}
	}else{
		if(res2_val >= RES2_SENSE_THRESHOLD_OHM ){
			r2_status = Resistor_Open;
			}else{
			r2_status = Resistor_Closed;
		}
	}
	
	prev_r1_status = r1_status;
	prev_r2_status = r2_status;


	/* Then, determine the status of the wearing out sensor */
	
	if((r1_status == Resistor_Closed) && (r2_status == Resistor_Closed)){
		sensor_status = Wear_Out_Lower_Than_50;
	}else if((r1_status == Resistor_Closed) && (r2_status == Resistor_Open)){
		sensor_status = Wear_Out_Between_50_And_75;
	}else{
		sensor_status = Wear_Out_Higher_Than_75;
	}	
	
}




void Battery_Status_Calculation(void){

	if(measure_buffer[BATTERY_MEASURE_INDEX] < BATTERY_LOW_LEVEL_THRESHOLD_PERCENT){
		battery_status = Battery_Low;
	}
	
	
	if(measure_buffer[BATTERY_MEASURE_INDEX] >= (BATTERY_LOW_LEVEL_THRESHOLD_PERCENT + BATTERY_HYSTERESIS_VALUE)){
		battery_status = Battery_OK;
	}
	
}



void Servo_Status_Indicator_Task(void){

	uint16_t servo_new_position; 
	
	if(battery_status == Battery_Low){
		servo_new_position = LOW_BATTERY_INDICATOR;
	}else{
		if(sensor_status == Wear_Out_Lower_Than_50){
			servo_new_position = RES_WORE_OUT_25_PERCENT;
		}else if(sensor_status == Wear_Out_Between_50_And_75){
			servo_new_position = RES_WORE_OUT_50_PERCENT;
		}else{
			servo_new_position = RES_WORE_OUT_75_PERCENT;
		}
	}
	
	
	Servo_Set_Position(servo_new_position);	
	Servo_Start();
	servo_previous_position = servo_new_position;
	
	while(!(system_flags & (1 << SERVO_POS_COMPLETE_FLAG))){
		//Wait for 1 second (until the servo has reached its final position)	
	}
	
}



uint8_t Xbee_Test_Mode_Cycle(void){
	
	uint8_t *aux_ptr;
	uint8_t i2c_error = I2C_OK;
	data_frame_transfer_status_t data_frame_received = DATA_FRAME_TIMEOUT;
	
	/* Turn on Timer0 (in order to implement delays and timeouts) */
	Timer0_On();
	/* Turn on the power supply for the digital devices and wait until it is stable */
	Boost_PSU_Power_On();
	Delay_ms(BOOST_PSU_SETUP_TIME_MS);	
	/* Turn on the power supply for the the analog measures and wait until it is stable */
	Analog_Measures_On();
	Delay_ms(ANALOG_POWER_SETUP_TIME_MS);
	/* Measure the resistor (analog sensors) and the battery */
	Analog_Measure_Task();	
	/* Turn off the power supply for the the analog measures */
	Analog_Measures_Off();
	Delay_ms(ANALOG_POWER_SETUP_TIME_MS);
	
	/* Get the analog measures, date and time */
	i2c_error = Prepare_Data_For_Frame();
	if(i2c_error == I2C_ERROR){
		Boost_PSU_Power_Off();
		Delay_ms(BOOST_PSU_SETUP_TIME_MS);
		/* Turn off Timer0 */
		Timer0_Off();
		return i2c_error;		
	}
	
	/* Determine the state of the resistive sensor */
	i2c_error = Resistor_Sensor_Status_Calculation();
	if(i2c_error == I2C_ERROR){
		Boost_PSU_Power_Off();
		Delay_ms(BOOST_PSU_SETUP_TIME_MS);
		/* Turn off Timer0 */
		Timer0_Off();
		return i2c_error;
	}
	
	i2c_error = EEPROM_Load_Servo_Indicator_State((uint8_t *)&servo_indicator_status);
	if(i2c_error == I2C_ERROR){
		Boost_PSU_Power_Off();
		Delay_ms(BOOST_PSU_SETUP_TIME_MS);
		/* Turn off Timer0 */
		Timer0_Off();
		return i2c_error;
	}
	
	if(servo_indicator_status == SERVO_INDICATOR_ENABLED){		
		/* Determine the state of the battery */
		Battery_Status_Calculation();
		/* Turn on the power supply for the servo */
		Servo_On();
		Delay_ms(SERVO_POWER_SETUP_TIME_MS);
		/* Set servo to the proper position */
		Servo_Status_Indicator_Task();
		/* Turn off the power supply for the servo */
		Servo_Off();
		Delay_ms(SERVO_POWER_SETUP_TIME_MS);
	}else{
		// Does nothing
	}
	
	/* Turn on the Xbee module and wait until it is stable */
	//Xbee_Interface_On();
	//Delay_ms(XBEE_POWER_SETUP_TIME_MS);
	
	/* Clean rx and command buffer */
	//Serial_Rx_Buffer_Flush();
	//Serial_Command_Buffer_Flush();
	/* Build the frames to be sent and compared */
	Data_Frame_Build();
	//Xbee_Build_Data_Accepted_Frame(&node_data_frame);
	//Xbee_Build_Data_Rejected_Frame(&node_data_frame);
	Xbee_Build_Data_Frame(&node_data_frame);
	/* Send data frame and verify that it was received */
	//data_frame_received = Xbee_Send_Data_And_Verify();
	/* If data frame was not received ... store it in the EEPROM */
	//if(data_frame_received != DATA_FRAME_ACCEPTED){
		aux_ptr = Xbee_Get_Data_Frame_Buffer_Pointer();
		i2c_error = EEPROM_Save_Backup_Data_Frame(aux_ptr);
		if(i2c_error == I2C_ERROR){
			/* Disable the Xbee module */
			//Xbee_Interface_Off();
			//Delay_ms(XBEE_POWER_SETUP_TIME_MS);
			/* Turn off boost power supply */
			Boost_PSU_Power_Off();
			Delay_ms(BOOST_PSU_SETUP_TIME_MS);
			/* Turn off Timer0 */
			Timer0_Off();
			return i2c_error;
		}
	//}else{
		//i2c_error = Xbee_Send_Backup_Data_Frames();	
	//}
		
	/* Disable the Xbee module */
	//Xbee_Interface_Off();
	//Delay_ms(XBEE_POWER_SETUP_TIME_MS);
	
	/* Turn off the power supply for the digital devices */
	Boost_PSU_Power_Off();
	Delay_ms(BOOST_PSU_SETUP_TIME_MS);
	
	/* Turn off Timer0 */
	Timer0_Off();
	
	return i2c_error;	
}



uint8_t Xbee_Periodic_Measure_Mode_Cycle(void){
	
	uint8_t *aux_ptr;
	uint8_t i2c_error = I2C_OK;
	data_frame_transfer_status_t data_frame_received = DATA_FRAME_TIMEOUT;
	
	/* Turn on Timer0 (in order to implement delays and timeouts) */
	Timer0_On();
	/* Turn on the power supply for the digital devices and wait until it is stable */
	Boost_PSU_Power_On();
	Delay_ms(BOOST_PSU_SETUP_TIME_MS);
	/* Clear the RTC interrupt flag of TimerA (CTAF) */
	i2c_error = RTC_TimerA_Int_Flag_Clear();
	if(i2c_error == I2C_ERROR){
		Timer0_Off();
		return i2c_error;
	}
	
	
	/* Turn on the power supply for the the analog measures and wait until it is stable */
	Analog_Measures_On();
	Delay_ms(ANALOG_POWER_SETUP_TIME_MS);
	/* Measure the resistor (analog sensors) and the battery */
	Analog_Measure_Task();
	/* Turn off the power supply for the the analog measures */
	Analog_Measures_Off();
	Delay_ms(ANALOG_POWER_SETUP_TIME_MS);
	
	
	/* Get the analog measures, date and time */
	i2c_error = Prepare_Data_For_Frame();
	if(i2c_error == I2C_ERROR){
		Boost_PSU_Power_Off();
		Delay_ms(BOOST_PSU_SETUP_TIME_MS);
		/* Turn off Timer0 */
		Timer0_Off();
		return i2c_error;
	}
	
	/* Determine the state of the resistive sensor */
	i2c_error = Resistor_Sensor_Status_Calculation();
	if(i2c_error == I2C_ERROR){
		Boost_PSU_Power_Off();
		Delay_ms(BOOST_PSU_SETUP_TIME_MS);
		/* Turn off Timer0 */
		Timer0_Off();
		return i2c_error;
	}
	
	i2c_error = EEPROM_Load_Servo_Indicator_State((uint8_t *)&servo_indicator_status);
	if(i2c_error == I2C_ERROR){
		Boost_PSU_Power_Off();
		Delay_ms(BOOST_PSU_SETUP_TIME_MS);
		/* Turn off Timer0 */
		Timer0_Off();
		return i2c_error;
	}
	
	
	if(servo_indicator_status == SERVO_INDICATOR_ENABLED){
		/* Determine the state of the battery */
		Battery_Status_Calculation();
		/* Turn on the power supply for the servo */
		Servo_On();
		Delay_ms(SERVO_POWER_SETUP_TIME_MS);
		/* Set servo to the proper position */
		Servo_Status_Indicator_Task();
		/* Turn off the power supply for the servo */
		Servo_Off();
		Delay_ms(SERVO_POWER_SETUP_TIME_MS);
	}else{
		// Does nothing
	}
	
	/* Turn on the Xbee module and wait until it is stable */
	//Xbee_Interface_On();
	//Delay_ms(XBEE_POWER_SETUP_TIME_MS);
	/* Clean rx and command buffer */
	//Serial_Rx_Buffer_Flush();
	//Serial_Command_Buffer_Flush();
	/* Build the frames to be sent and compared */
	Data_Frame_Build();
	//Xbee_Build_Data_Accepted_Frame(&node_data_frame);
	//Xbee_Build_Data_Rejected_Frame(&node_data_frame);
	Xbee_Build_Data_Frame(&node_data_frame);
	/* Send data frame and verify that it was received */
	//data_frame_received = Xbee_Send_Data_And_Verify();
	
	/* If data frame was not received ... store it in the EEPROM */
	//if(data_frame_received != DATA_FRAME_ACCEPTED){
		aux_ptr = Xbee_Get_Data_Frame_Buffer_Pointer();
		i2c_error = EEPROM_Save_Backup_Data_Frame(aux_ptr);
		if(i2c_error == I2C_ERROR){
			//Xbee_Interface_Off();
			//Delay_ms(XBEE_POWER_SETUP_TIME_MS);
			Boost_PSU_Power_Off();
			Delay_ms(BOOST_PSU_SETUP_TIME_MS);
			/* Turn off Timer0 */
			Timer0_Off();
			return i2c_error;
		}
	//}else{ // Else ... transmit all the frames that are stored in the EEPROM
			//i2c_error = Xbee_Send_Backup_Data_Frames();
	//}
	
	/* Disable the Xbee module */
	//Xbee_Interface_Off();
	//Delay_ms(XBEE_POWER_SETUP_TIME_MS);
	
	/* Turn off the power supply for the digital devices */
	Boost_PSU_Power_Off();
	Delay_ms(BOOST_PSU_SETUP_TIME_MS);
	
	/* Turn off Timer0 */
	Timer0_Off();
	
	return i2c_error;
}


uint8_t Bluetooth_Measure_And_Get_Data(void){
	
	uint8_t i2c_error = I2C_OK;
	
	/* Turn on the power supply for the the analog measures and wait until it is stable */
	Analog_Measures_On();
	Delay_ms(ANALOG_POWER_SETUP_TIME_MS);
	/* Measure the resistor (analog sensors) and the battery */
	Analog_Measure_Task();
	/* Turn off the power supply for the the analog measures */
	Analog_Measures_Off();
	Delay_ms(ANALOG_POWER_SETUP_TIME_MS);
	
	/* Get the analog measures, date and time */
	i2c_error = Prepare_Data_For_Frame();
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}
	
	/* Determine the state of the resistive sensor */
	i2c_error = Resistor_Sensor_Status_Calculation();
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}
	
	i2c_error = EEPROM_Load_Servo_Indicator_State((uint8_t *)&servo_indicator_status);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}
	
	if(servo_indicator_status == SERVO_INDICATOR_ENABLED){
		/* Determine the state of the battery */
		Battery_Status_Calculation();
		/* Turn on the power supply for the servo */
		Servo_On();
		Delay_ms(SERVO_POWER_SETUP_TIME_MS);
		/* Set servo to the proper position */
		Servo_Status_Indicator_Task();
		/* Turn off the power supply for the servo */
		Servo_Off();
		Delay_ms(SERVO_POWER_SETUP_TIME_MS);
	}else{
		// Does nothing
	}
	
	/* Build the frames to be sent and compared */
	Data_Frame_Build();
	Bluetooth_Build_Data_Frame(&node_data_frame);
	/* Send data frame */
	Bluetooth_Send_Data_Frame();
		
	return i2c_error;	
}





uint8_t Bluetooth_Test_Mode_Cycle(void){
	
	char temp_str[30];
	uint8_t i2c_error = I2C_OK;
	uint8_t command_received = 0;
	uint8_t keep_bluetooth_connected = 1;
	
	/* Configure the UART for bluetooth to work */
	Bluetooth_Interface_Working_Mode_UART_Init();
	/* Enable Timer0 to generate delays */
	Timer0_On();
	/* Turn on boost power supply for digital devices */
	Boost_PSU_Power_On();
	Delay_ms(BOOST_PSU_SETUP_TIME_MS);
	/* Turn on bluetooth module */
	Bluetooth_Interface_On();
	Delay_ms(BLUETOOTH_POWER_SETUP_TIME_MS);

	/* While the bluetooth module keeps receiving commands ... */
	while(keep_bluetooth_connected){
		
		/* Clean reception buffer and enable the Xbee reception interrupt */
		Serial_Rx_Buffer_Flush();
		Bluetooth_Serial_Rx_Enable();
		
		/* Wait for a command */
		command_received = Serial_Check_If_Command_Received((1 << BT_COMM_RESPONSE_FLAG), BLUETOOTH_TIMEOUT_PERIOD_MS);
		
		if(command_received){
			
			if(Bluetooth_Verify_Command_Data_Frame_Request()){
				i2c_error = Bluetooth_Measure_And_Get_Data();
				if(i2c_error == I2C_ERROR){
					Bluetooth_Serial_Rx_Disable();
					/* Turn off bluetooth module */
					Bluetooth_Interface_Off();
					Delay_ms(BLUETOOTH_POWER_SETUP_TIME_MS);
					/* Turn off boost power supply for digital devices */
					Boost_PSU_Power_Off();
					Delay_ms(BOOST_PSU_SETUP_TIME_MS);
					/* Disable Timer0 */
					Timer0_Off();
					return i2c_error;				
				}
				
				keep_bluetooth_connected = 1;
			
			}else if(Bluetooth_Verify_Command_Emitter_ID_Request()){
				i2c_error = EEPROM_Load_Emitter_Node_ID((uint8_t *)temp_str);
				if(i2c_error == I2C_ERROR){
					Bluetooth_Serial_Rx_Disable();
					/* Turn off bluetooth module */
					Bluetooth_Interface_Off();
					Delay_ms(BLUETOOTH_POWER_SETUP_TIME_MS);
					/* Turn off boost power supply for digital devices */
					Boost_PSU_Power_Off();
					Delay_ms(BOOST_PSU_SETUP_TIME_MS);
					/* Disable Timer0 */
					Timer0_Off();
					return i2c_error;
				}
				
				temp_str[EMITTER_NODE_ID_SIZE_IN_BYTES - 2] = '\0';
				Bluetooth_Send_String(temp_str + 1);
				
				keep_bluetooth_connected = 1;
				
			}else if(Bluetooth_Verify_Command_Receiver_ID_Request()){
				i2c_error = EEPROM_Load_Receiver_Central_ID((uint8_t *)temp_str);
				if(i2c_error == I2C_ERROR){
					Bluetooth_Serial_Rx_Disable();
					/* Turn off bluetooth module */
					Bluetooth_Interface_Off();
					Delay_ms(BLUETOOTH_POWER_SETUP_TIME_MS);
					/* Turn off boost power supply for digital devices */
					Boost_PSU_Power_Off();
					Delay_ms(BOOST_PSU_SETUP_TIME_MS);
					/* Disable Timer0 */
					Timer0_Off();
					return i2c_error;
				}
			
				temp_str[RECEIVER_CENTRAL_ID_SIZE_IN_BYTES - 2] = '\0';
				Bluetooth_Send_String(temp_str + 1);
				
				keep_bluetooth_connected = 1;
			
			}else if(Bluetooth_Verify_Command_Part_Number_Request()){
				i2c_error = EEPROM_Load_Part_Number((uint8_t *)temp_str);
				if(i2c_error == I2C_ERROR){
					Bluetooth_Serial_Rx_Disable();
					/* Turn off bluetooth module */
					Bluetooth_Interface_Off();
					Delay_ms(BLUETOOTH_POWER_SETUP_TIME_MS);
					/* Turn off boost power supply for digital devices */
					Boost_PSU_Power_Off();
					Delay_ms(BOOST_PSU_SETUP_TIME_MS);
					/* Disable Timer0 */
					Timer0_Off();
					return i2c_error;
				}
			
				temp_str[PART_NUMBER_SIZE_IN_BYTES - 2] = '\0';
				Bluetooth_Send_String(temp_str + 1);
			
				keep_bluetooth_connected = 1;
			
			}else if(Bluetooth_Verify_Command_Date_And_Time_Request()){
				i2c_error = System_Date_Update();
				if(i2c_error == I2C_ERROR){
					Bluetooth_Serial_Rx_Disable();
					/* Turn off bluetooth module */
					Bluetooth_Interface_Off();
					Delay_ms(BLUETOOTH_POWER_SETUP_TIME_MS);
					/* Turn off boost power supply for digital devices */
					Boost_PSU_Power_Off();
					Delay_ms(BOOST_PSU_SETUP_TIME_MS);
					/* Disable Timer0 */
					Timer0_Off();
					return i2c_error;
				}

				i2c_error = System_Time_Update();
				if(i2c_error == I2C_ERROR){
					Bluetooth_Serial_Rx_Disable();
					/* Turn off bluetooth module */
					Bluetooth_Interface_Off();
					Delay_ms(BLUETOOTH_POWER_SETUP_TIME_MS);
					/* Turn off boost power supply for digital devices */
					Boost_PSU_Power_Off();
					Delay_ms(BOOST_PSU_SETUP_TIME_MS);
					/* Disable Timer0 */
					Timer0_Off();
					return i2c_error;
				}
			
				/* Build date string and time string */
				sprintf(date_str, "%02d/%02d/%04d", date_buffer[0], date_buffer[1], 2000 + date_buffer[2]);
	
				if(time_buffer[3] == AM){
					sprintf(time_str, "%02d:%02d a.m.", time_buffer[0], time_buffer[1]);
				}else{
					sprintf(time_str, "%02d:%02d p.m.", time_buffer[0], time_buffer[1]);
				}
			
				sprintf(temp_str,"%s %s", date_str, time_str);
				Bluetooth_Send_String(temp_str);
				
				keep_bluetooth_connected = 1;
			
			}else if(Bluetooth_Verify_Command_EEPROM_Reset()){
				i2c_error = EEPROM_Clear_Data_Frames_Only();
				if(i2c_error == I2C_ERROR){
					Bluetooth_Serial_Rx_Disable();
					/* Turn off bluetooth module */
					Bluetooth_Interface_Off();
					Delay_ms(BLUETOOTH_POWER_SETUP_TIME_MS);
					/* Turn off boost power supply for digital devices */
					Boost_PSU_Power_Off();
					Delay_ms(BOOST_PSU_SETUP_TIME_MS);
					/* Disable Timer0 */
					Timer0_Off();
					return i2c_error;
				}
			
				Bluetooth_Send_String("Memoria de backup limpia.");
			
				keep_bluetooth_connected = 1;
			
			}else if(Bluetooth_Verify_Command_EEPROM_Data_Request()){
				
				i2c_error = Bluetooth_Send_Backup_Data_Frames();
				if(i2c_error == I2C_ERROR){
					Bluetooth_Serial_Rx_Disable();
					/* Turn off bluetooth module */
					Bluetooth_Interface_Off();
					Delay_ms(BLUETOOTH_POWER_SETUP_TIME_MS);
					/* Turn off boost power supply for digital devices */
					Boost_PSU_Power_Off();
					Delay_ms(BOOST_PSU_SETUP_TIME_MS);
					/* Disable Timer0 */
					Timer0_Off();
					return i2c_error;
				}
			
				keep_bluetooth_connected = 1;
				
			}else if(Bluetooth_Verify_Command_Measure_Period_Set()){
				
				if(Serial_Get_Measure_Period((uint8_t *)temp_str)){
					
					i2c_error = EEPROM_Save_Measure_Period_In_Days((uint8_t *)temp_str);	
					if(i2c_error == I2C_ERROR){
						Bluetooth_Serial_Rx_Disable();
						/* Turn off bluetooth module */
						Bluetooth_Interface_Off();
						Delay_ms(BLUETOOTH_POWER_SETUP_TIME_MS);
						/* Turn off boost power supply for digital devices */
						Boost_PSU_Power_Off();
						Delay_ms(BOOST_PSU_SETUP_TIME_MS);
						/* Disable Timer0 */
						Timer0_Off();
						return i2c_error;
					}
			
					i2c_error = RTC_Configure_TimerA_Interrupt();
					if(i2c_error == I2C_ERROR){
						Bluetooth_Serial_Rx_Disable();
						/* Turn off bluetooth module */
						Bluetooth_Interface_Off();
						Delay_ms(BLUETOOTH_POWER_SETUP_TIME_MS);
						/* Turn off boost power supply for digital devices */
						Boost_PSU_Power_Off();
						Delay_ms(BOOST_PSU_SETUP_TIME_MS);
						/* Disable Timer0 */
						Timer0_Off();
						return i2c_error;
					}
					
					Bluetooth_Send_String("Periodo configurado.");			
				
				}
				 
				keep_bluetooth_connected = 1;
			
			}else if(Bluetooth_Verify_Command_Emitter_ID_Set()){
			
				Serial_Get_Emitter_ID((uint8_t *)temp_str);
				sprintf(emitter_node_id_str, "\"%s\"", temp_str);
				i2c_error = EEPROM_Save_Emitter_Node_ID((uint8_t *)emitter_node_id_str);
				if(i2c_error == I2C_ERROR){
					Bluetooth_Serial_Rx_Disable();
					/* Turn off bluetooth module */
					Bluetooth_Interface_Off();
					Delay_ms(BLUETOOTH_POWER_SETUP_TIME_MS);
					/* Turn off boost power supply for digital devices */
					Boost_PSU_Power_Off();
					Delay_ms(BOOST_PSU_SETUP_TIME_MS);
					/* Disable Timer0 */
					Timer0_Off();
					return i2c_error;
				}

				keep_bluetooth_connected = 1;
			
			}else if(Bluetooth_Verify_Command_Receiver_ID_Set()){
			
				Serial_Get_Receiver_ID((uint8_t *)temp_str);
				sprintf(receiver_central_id_str, "\"%s\"", temp_str);
				i2c_error = EEPROM_Save_Receiver_Central_ID((uint8_t *)receiver_central_id_str);
				if(i2c_error == I2C_ERROR){
					Bluetooth_Serial_Rx_Disable();
					/* Turn off bluetooth module */
					Bluetooth_Interface_Off();
					Delay_ms(BLUETOOTH_POWER_SETUP_TIME_MS);
					/* Turn off boost power supply for digital devices */
					Boost_PSU_Power_Off();
					Delay_ms(BOOST_PSU_SETUP_TIME_MS);
					/* Disable Timer0 */
					Timer0_Off();
					return i2c_error;
				}
				
				Bluetooth_Send_String("ID receptor configurado.");
			
				keep_bluetooth_connected = 1;
			
			}else if(Bluetooth_Verify_Command_Number_Of_Sensors_Set()){
			
				if(Serial_Get_Number_Of_Sensors((uint8_t *)temp_str)){
					*temp_str = 1;
					i2c_error = EEPROM_Save_Number_Of_Sensors((uint8_t *)temp_str);
					if(i2c_error == I2C_ERROR){
						Bluetooth_Serial_Rx_Disable();
						/* Turn off bluetooth module */
						Bluetooth_Interface_Off();
						Delay_ms(BLUETOOTH_POWER_SETUP_TIME_MS);
						/* Turn off boost power supply for digital devices */
						Boost_PSU_Power_Off();
						Delay_ms(BOOST_PSU_SETUP_TIME_MS);
						/* Disable Timer0 */
						Timer0_Off();
						return i2c_error;
					}
				
				}
			
				keep_bluetooth_connected = 1;
			
			}else if(Bluetooth_Verify_Command_Servo_Enable_Disable()){
			
				if(Serial_Get_Servo_State((uint8_t *)temp_str)){
				
					i2c_error = EEPROM_Save_Servo_Indicator_State((uint8_t *)temp_str);
					if(i2c_error == I2C_ERROR){
						Bluetooth_Serial_Rx_Disable();
						/* Turn off bluetooth module */
						Bluetooth_Interface_Off();
						Delay_ms(BLUETOOTH_POWER_SETUP_TIME_MS);
						/* Turn off boost power supply for digital devices */
						Boost_PSU_Power_Off();
						Delay_ms(BOOST_PSU_SETUP_TIME_MS);
						/* Disable Timer0 */
						Timer0_Off();
						return i2c_error;
					}
					
					if(*temp_str == 1){
						Bluetooth_Send_String("Indicador activado.");
					}else{
						Bluetooth_Send_String("Indicador desactivado.");
					}
					
				}
			
				keep_bluetooth_connected = 1;
			
			}else if(Bluetooth_Verify_Command_Date_And_Time_Set()){
				
				if(Serial_Get_Date_And_Time((uint8_t *)temp_str)){
					
					i2c_error = RTC_Date_Set(*(temp_str), *(temp_str + 1), *(temp_str + 2));
					if(i2c_error == I2C_ERROR){
						Bluetooth_Serial_Rx_Disable();
						/* Turn off bluetooth module */
						Bluetooth_Interface_Off();
						Delay_ms(BLUETOOTH_POWER_SETUP_TIME_MS);
						/* Turn off boost power supply for digital devices */
						Boost_PSU_Power_Off();
						Delay_ms(BOOST_PSU_SETUP_TIME_MS);
						/* Disable Timer0 */
						Timer0_Off();
						return i2c_error;
					}
					
					
					i2c_error = RTC_Time_Set_12h(*(temp_str + 3), *(temp_str + 4), 0, *(temp_str + 5));
					if(i2c_error == I2C_ERROR){
						Bluetooth_Serial_Rx_Disable();
						/* Turn off bluetooth module */
						Bluetooth_Interface_Off();
						Delay_ms(BLUETOOTH_POWER_SETUP_TIME_MS);
						/* Turn off boost power supply for digital devices */
						Boost_PSU_Power_Off();
						Delay_ms(BOOST_PSU_SETUP_TIME_MS);
						/* Disable Timer0 */
						Timer0_Off();
						return i2c_error;
					}
					
					Bluetooth_Send_String("Fecha y hora configurada.");
				}
				
				
				keep_bluetooth_connected = 1;
			
			}else if(Bluetooth_Verify_Command_Disconnect()){
	
				keep_bluetooth_connected = 0;
			
			}else{
			
				keep_bluetooth_connected = 0;
			
			}
			
			
		}else{
			keep_bluetooth_connected = 0;
		}
		
	}
	
	Bluetooth_Serial_Rx_Disable();

	/* Turn off bluetooth module */
	Bluetooth_Interface_Off();
	Delay_ms(BLUETOOTH_POWER_SETUP_TIME_MS);
	
	/* Turn on boost power supply for digital devices */
	Boost_PSU_Power_Off();
	Delay_ms(BOOST_PSU_SETUP_TIME_MS);
	
	/* Disable Timer0 */
	Timer0_Off();
	
	return i2c_error;
}


uint8_t System_First_Time_Configuration(void){
	
	uint8_t i2c_error = I2C_OK;

	/* Turn on Timer0 (in order to implement delays and timeouts) */
	Timer0_On();
	
	/* Turn on boost power supply */
	Boost_PSU_Power_On();
	Delay_ms(BOOST_PSU_SETUP_TIME_MS);

	i2c_error = EEPROM_First_Time_Configuration();
	if(i2c_error == I2C_ERROR){
		Boost_PSU_Power_Off();
		Delay_ms(BOOST_PSU_SETUP_TIME_MS);
		/* Turn off Timer0 */
		Timer0_Off();		
		
		return i2c_error;
	}

	Bluetooth_Interface_First_Time_Configuration();

	/* Turn on the Xbee module and wait until it is stable */
	Xbee_Interface_On(); // For testing purposes only
	Delay_ms(XBEE_POWER_SETUP_TIME_MS); // For testing purposes only

	Xbee_Send_String("Primera configuracion completa ...\n\n\r ");
	
	/* Disable the Xbee module */
	Xbee_Interface_Off();
	Delay_ms(XBEE_POWER_SETUP_TIME_MS);
	
	/* Turn on boost power supply for digital devices */
	Boost_PSU_Power_Off();
	Delay_ms(BOOST_PSU_SETUP_TIME_MS);
	
	/* Turn off Timer0 */
	Timer0_Off();

	return i2c_error;
		
}


uint8_t RTC_Boot_Time_Configuration(void){
	
	uint8_t i2c_error = I2C_OK;
	
	/* Turn on Timer0 (in order to implement delays and timeouts) */
	Timer0_On();
	
	/* Turn on boost power supply */
	Boost_PSU_Power_On();
	Delay_ms(BOOST_PSU_SETUP_TIME_MS);
	
	i2c_error = RTC_Set_Mode_12h();
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}
	
	i2c_error = RTC_Date_Set(DATE_DAY_BY_DEFAULT, DATE_MONTH_BY_DEFAULT, DATE_YEAR_BY_DEFAULT);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}
	
	i2c_error = RTC_Time_Set_12h(TIME_HOUR_BY_DEFAULT, TIME_MINUTE_BY_DEFAULT, TIME_SECOND_BY_DEFAULT, TIME_MERIDIEN_BY_DEFAULT);	
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}	
	
	i2c_error = RTC_Configure_TimerA_Interrupt();	

	/* Turn on boost power supply for digital devices */
	Boost_PSU_Power_Off();
	Delay_ms(BOOST_PSU_SETUP_TIME_MS);

	/* Turn off Timer0 */
	Timer0_Off();

	return i2c_error;
	
}


uint8_t EEPROM_First_Time_Configuration(void){
	
	uint8_t i2c_error = I2C_OK;
	uint8_t period = MEASURE_PERIOD_IN_DAYS_BY_DEFAULT;
	uint8_t media = MAIN_COMM_MEDIA_BY_DEFAULT;
	uint8_t n_sensors= NUMBER_OF_SENSORS_BY_DEFAULT;
	uint8_t servo_ind_state = SERVO_INDICATOR_STATE_BY_DEFAULT;
	
	/* Clear the space that will be used in the EEPROM */
	i2c_error = EEPROM_Clear_All_Data();
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}
	
	i2c_error = EEPROM_Save_Servo_Indicator_State(&servo_ind_state);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}
	
	i2c_error = EEPROM_Save_Number_Of_Sensors(&n_sensors);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}	

	i2c_error = EEPROM_Save_Main_Communication_Media(&media);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}

	i2c_error = EEPROM_Save_Measure_Period_In_Days(&period);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}
		
	i2c_error = EEPROM_Save_Part_Number((uint8_t *)part_number_by_default);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}
	
	i2c_error = EEPROM_Save_Emitter_Node_ID((uint8_t *)emitter_node_id_by_default);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}
	
	i2c_error = EEPROM_Save_Receiver_Central_ID((uint8_t *)receiver_central_id_by_default);			
		
	return i2c_error;
}


uint8_t RTC_Configure_TimerA_Interrupt(void){
	
	uint8_t i2c_error = I2C_OK;
	uint8_t measure_period_in_hours = 20;	
		
	/* Load measure period in days from the EEPROM */
	i2c_error = EEPROM_Load_Measure_Period_In_Days((uint8_t *)&measure_period_in_days);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}
	
	/* Calculate the value to be stored in the TimerA value */
	measure_period_in_hours = (uint8_t)(24 * measure_period_in_days);
	
	/* Disable clockout signal*/
	i2c_error = RTC_Clockout_Source_Select(CLKOUT_DISABLE);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}	
	
	/* Clear the TimerA Interrupt flag */
	i2c_error = RTC_TimerA_Int_Flag_Clear();
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}
		
	/* Disable the TimerA interrupt */
	i2c_error = RTC_TimerA_Int_Disable();
	if(i2c_error == I2C_ERROR){	
		return i2c_error;
	}
	
	/* Disable the TimerA counter */
	i2c_error = RTC_TimerA_Disable();
	if(i2c_error == I2C_ERROR){	
		return i2c_error;
	}
	
	/* Load the proper value in the TimerA */
	i2c_error = RTC_TimerA_Load_Value(measure_period_in_hours);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}
	
	/* Set the proper clock source (pre-scaler) to count hours */
	
	#if(MODO_DE_TRABAJO_DE_NODO == NODO_EN_MODO_TEST_TIEMPO_CORTO)
	i2c_error = RTC_TimerA_Clock_Source_Select(SRC_1_HZ); // This clock source is used for testing
	#elif(MODO_DE_TRABAJO_DE_NODO == NODO_EN_MODO_TRABAJO_TIEMPO_NORMAL)
	i2c_error = RTC_TimerA_Clock_Source_Select(SRC_1_HZ_DIV_3600); // This clock source is used for normal operation
	#else
	 #error Debe elegir un MODO DE TRABAJO valido
	#endif
	
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}

	/* Enable the TimerA counter */
	i2c_error = RTC_TimerA_Enable();
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}

	/* Enable the TimerA interrupt */
	i2c_error = RTC_TimerA_Int_Enable();
	
	return i2c_error;
}





void Microcontroller_Peripherals_Disable(void){
	
	/* Disable UART transmitter for Both Xbee and Bluetooth */
	UCSR0B &= ~(1 << TXEN0);
	UCSR1B &= ~(1 << TXEN1);
	
	/* Disable UART receiver for Bluetooth */
	UCSR1B &= ~(1 << RXEN1);
	
	/* Disable ADC and ADC voltage reference */
	ADCSRA &= ~(1 << ADEN);
	ADMUX &= ~((1 << REFS1) | (1 << REFS0));
	
}


void Microcontroller_Peripherals_Enable(void){
	
	/* Enable UART transmitter for Both Xbee and Bluetooth */
	UCSR0B |= (1 << TXEN0);
	UCSR1B |= (1 << TXEN1);
	
	/* Enable UART receiver for Bluetooth */
	UCSR1B |= (1 << RXEN1);
	
	/* Enable ADC and ADC voltage reference */
	ADCSRA |= (1 << ADEN);
	ADMUX |= ((1 << REFS1) | (1 << REFS0));
}


void WDT_Off(void){
	
	cli();
	wdt_reset();
	MCUSR &= ~(1 << WDRF);
	WDTCSR |= (1 << WDCE) | (1 << WDE);
	WDTCSR = 0x00;
	sei();
	
}