
/* File inclusion */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "project_defines.h"
#include "global.h"
#include "serial_communication.h"
#include "xbee_interface.h"
#include "bluetooth_interface.h"
#include "MT_rtc_pcf8523.h"


/* Variable definition */
char serial_rx_buffer[SERIAL_RX_BUFFER_SIZE];
char serial_command_buffer[SERIAL_COMMAND_BUFFER_SIZE];
uint8_t serial_rx_buff_index = 0;


/* Function definition */ 

void Serial_Rx_Buffer_Flush(void){
	
	for(serial_rx_buff_index = 0; serial_rx_buff_index < SERIAL_RX_BUFFER_SIZE; serial_rx_buff_index++){
		serial_rx_buffer[serial_rx_buff_index] = '\0';
	}
	
	serial_rx_buff_index = 0;
	
}


void Serial_Command_Buffer_Flush(void){

	uint8_t index;

	for(index = 0; index < SERIAL_COMMAND_BUFFER_SIZE; index++){
		serial_command_buffer[index] = '\0';
	}
	
}


void Serial_Rx_Buffer_Store_Byte(uint8_t byte){
	
	serial_rx_buffer[serial_rx_buff_index] = byte;
	serial_rx_buff_index++;
	
	if(serial_rx_buff_index >= SERIAL_RX_BUFFER_SIZE){
		serial_rx_buff_index = 0;
	}
	
}


void Serial_Rx_Buffer_Load_Command(void){
	
	//uint8_t byte = '\0';
	
	uint8_t serial_rx_buff_index = 0;

	//while(byte = serial_rx_buffer[serial_rx_buff_index]){
	while(serial_rx_buffer[serial_rx_buff_index]){
		serial_command_buffer[serial_rx_buff_index] = serial_rx_buffer[serial_rx_buff_index];
		serial_rx_buff_index++;
	};

	
}


uint8_t Serial_Check_If_Command_Received(uint16_t flag_mask, uint32_t time_limit_ms){
	
	uint16_t command_received = 0;
	uint16_t time_up = 0;
	
	uint8_t command_on_time = 0; 	
	
	/* Start time count */
	timeout_limit_ms = time_limit_ms;
	timeout_counter_ms = 0;
	
	/* Clear flags */
	cli();
	system_flags &= ~flag_mask;
	system_flags &= ~(1 << TIME_OUT_FLAG);
	sei();	
	
	//Wait until either a response is received or the time is up
	while((!command_received) && (!time_up)){
		command_received = system_flags & flag_mask;
		time_up = system_flags & (1 << TIME_OUT_FLAG);
	}	


	if(command_received){
		command_on_time = 1;
	}else{
		command_on_time = 0;
	}


	cli();
	system_flags |= flag_mask;
	system_flags |= (1 << TIME_OUT_FLAG);
	sei();

	return command_on_time;
	
}


uint8_t Serial_Verify_Command_Received(char *command_expected){
	
	uint8_t command_correct = 0;
	
	/* Compare received string to the expected command */
	if(strcmp((const char *)serial_command_buffer, (const char *)command_expected) == 0){
		command_correct = 1;
	}
	
	return command_correct;
}



uint8_t Serial_Verify_Command_Header(char *header_expected){
	
	uint8_t command_correct = 1;
	uint8_t index = 0;
	
	while((command_correct == 1) && (*(header_expected + index) != '\0')){
		if(*(header_expected + index) != *(serial_command_buffer + index)){
			command_correct = 0;
		}
		
		index++;
	}
	
	return command_correct;
}


uint8_t Serial_Get_Measure_Period(uint8_t *data){
	
	uint8_t value_correct = 1;
	uint8_t d = 0, u = 0;
	uint8_t temp;
	
	d = *(serial_command_buffer + BT_MEASURE_PERIOD_SET_HEADER_SIZE - 1) - 48;
	u = *(serial_command_buffer + BT_MEASURE_PERIOD_SET_HEADER_SIZE) - 48;
	
	temp = (10 * d) + u;
	
	if((temp >= MEASURE_PERIOD_MIN_VALUE) && (temp <= MEASURE_PERIOD_MAX_VALUE)){
		value_correct = 1;
		*data = temp;
	}else{
		value_correct = 0;
	}
	
	return value_correct;
}


void Serial_Get_Emitter_ID(uint8_t *data){
	
	uint8_t index;
	
	for(index = 0; index < (EMITTER_NODE_ID_SIZE_IN_BYTES - 2); index++){
		*(data + index) = *(serial_command_buffer + index + BT_EMITTER_ID_SET_HEADER_SIZE - 1);
	}
	
	*(data + EMITTER_NODE_ID_SIZE_IN_BYTES - 3) = '\0';
}


void Serial_Get_Receiver_ID(uint8_t *data){
	
	uint8_t index;
	
	for(index = 0; index < (RECEIVER_CENTRAL_ID_SIZE_IN_BYTES - 2); index++){
		*(data + index) = *(serial_command_buffer + index + BT_RECEIVER_ID_SET_HEADER_SIZE - 1);
	}
	
	*(data + RECEIVER_CENTRAL_ID_SIZE_IN_BYTES - 3) = '\0';
}


uint8_t Serial_Get_Number_Of_Sensors(uint8_t *data){

	uint8_t value_correct = 1;
	uint8_t temp;

	temp = *(serial_command_buffer + BT_NUM_SENSORS_SET_HEADER_SIZE - 1);

	if((temp <= 50) && (temp >= 49)){
		temp -= 48;
		*data = temp;
		value_correct = 1;
	}else{
		value_correct = 0;
	}

	return value_correct;

}


uint8_t Serial_Get_Servo_State(uint8_t *data){
	
	uint8_t value_correct = 1;
	uint8_t temp;
 
	temp = *(serial_command_buffer + BT_SERVO_ENABLE_DISABLE_HEADER_SIZE - 1);

	if((temp <= 49) && (temp >= 48)){
		temp -= 48;
		*data = temp;
		value_correct = 1;
	}else{
		value_correct = 0;
	}

	return value_correct;	
}


uint8_t Serial_Get_Date_And_Time(uint8_t *data){

	uint8_t value_correct = 1;
	uint8_t day = 0, month = 0, year = 0;
	uint8_t hour = 0, minute = 0, meridien = 0;
	uint8_t index = 0;

	/* Convert day ascii value to unsigned integer */
	index = BT_DATE_TIME_SET_HEADER_SIZE - 1;
	day += ((*(serial_command_buffer + index)) - 48);
	
	if((*(serial_command_buffer + index + 2)) == '/'){
		index++;
		day = (10 * day) + ((*(serial_command_buffer + index)) - 48);
	}
	
	/* Convert month ascii value to unsigned integer */
	index += 2;
	month += ((*(serial_command_buffer + index)) - 48);

	if((*(serial_command_buffer + index + 2)) == '/'){
		index++;
		month = (10 * month) + ((*(serial_command_buffer + index)) - 48);
	}
	
	/* Convert year ascii value to unsigned integer */
	index += 4;
	year = 10 * ((*(serial_command_buffer + index)) - 48);
	index++;
	year += ((*(serial_command_buffer + index)) - 48);
	
	if((day < 1) || (day > 31) || (month < 1) || (month > 12) || (year > 99)){
		value_correct = 0;
	}

	///* Convert hour ascii value to unsigned integer */
	index += 2;
	hour += ((*(serial_command_buffer + index)) - 48);
	if((*(serial_command_buffer + index + 2)) == ':'){
		index++;
		hour = (10 * hour) + ((*(serial_command_buffer + index)) - 48);	
	}

	/* Convert minute ascii value to unsigned integer */
	index += 2;
	minute += ((*(serial_command_buffer + index)) - 48);
	if((*(serial_command_buffer + index + 2)) == '.'){
		index++;
		minute = (10 * minute) + ((*(serial_command_buffer + index)) - 48);
	}
	
	if((hour > 23) || (minute > 59)){
		value_correct = 0;
	}
	
	if(value_correct){
	
		/* Determine the meridien */
		if(hour == 0){
			hour = 12;
			meridien = AM;
		}else if(hour >= 12){
			meridien = PM;
			if(hour > 12){
				hour -= 12;
			}
		}else{
			meridien = AM;
		}
	
		*data = day;
		*(data + 1) = month;
		*(data + 2) = year;
		
		*(data + 3) = hour;
		*(data + 4) = minute;
		*(data + 5) = meridien;
	}

	return value_correct;
}



void Xbee_Send_Command_Buff(void){
	Xbee_Send_String(serial_command_buffer);
}


void Bluetooth_Send_Command_Buff(void){
	Bluetooth_Send_String(serial_command_buffer);
}