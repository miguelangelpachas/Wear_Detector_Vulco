
/* File inclusion */
#include <avr/io.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "project_defines.h"
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "global.h"
#include "MT_uart.h"
#include "MT_twi_master.h"
#include "MT_delay.h"
#include "eeprom_manager.h"
#include "xbee_interface.h"
#include "serial_communication.h"
#include "grinding_mill_wearing_out_monitor_node.h"
#include "string.h"

/* Store constant strings in the Flash Memory */
const char xbee_part_number_by_default [] PROGMEM = PART_NUMBER_STR_BY_DEFAULT;
const char xbee_emitter_node_id_by_default[] PROGMEM = EMITTER_NODE_ID_STR_BY_DEFAULT;
const char xbee_receiver_central_id_by_default[] PROGMEM = RECEIVER_CENTRAL_ID_STR_BY_DEFAULT;
const char xbee_part_number_header[] PROGMEM = PART_NUMBER_HEADER_STR;
const char xbee_emitter_node_id_header[] PROGMEM = EMITTER_NODE_ID_HEADER_STR;
const char xbee_receiver_central_id_header[] PROGMEM = RECEIVER_CENTRAL_ID_HEADER_STR;
const char xbee_emitter_measure_period_header[] PROGMEM = MEASURE_PERIOD_HEADER_STR;
const char xbee_data_frame_date_header[] PROGMEM = DATA_FRAME_DATE_HEADER_STR;
const char xbee_data_frame_time_header[] PROGMEM = DATA_FRAME_TIME_HEADER_STR;
const char xbee_wear_out_state_header[] PROGMEM = WEAR_OUT_STATE_HEADER_STR;
const char xbee_battery_level_header[] PROGMEM = BATTERY_LEVEL_HEADER_STR;
const char xbee_emitter_resistance_1_header[] PROGMEM = EMITTER_OHM1_HEADER_STR;
const char xbee_emitter_resistance_2_header[] PROGMEM = EMITTER_OHM2_HEADER_STR;


/* Variable definition */
char data_accepted_frame[XBEE_DATA_ACCEPTED_FRAME_SIZE]; //"emisor_DET-001_aceptado\r\n\n";
char data_rejected_frame[XBEE_DATA_ACCEPTED_FRAME_SIZE]; //"emisor_DET-001_rechazado\r\n\n";
char xbee_data_frame_buffer[XBEE_DATA_FRAME_BUFF_SIZE];


void Xbee_Interface_Initialize(void){

	/* Disable the Xbee module */
	DDR_XBEE_ON |= (1 << XBEE_ON);
	PORT_XBEE_ON &= ~(1 << XBEE_ON);
	
	Xbee_Interface_UART_Init();
}


void Xbee_Interface_UART_Init(void){
	
	UART_t my_uart;

	my_uart.baud_rate = XBEE_SERIAL_BAUD_RATE;
	my_uart.data_bits = UART_8_Data_Bits;
	my_uart.parity_mode = UART_Parity_Disabled;
	my_uart.stop_bits= UART_1_Stop_Bit;
	my_uart.u2x = U2X_Enabled;
	my_uart.tx_status = UART_Tx_Enabled;
	my_uart.rx_status = UART_Rx_Disabled;
	my_uart.interrupt_mask = UART_Interrupts_Disabled;

	UART0_Configurar(&my_uart);
	
}


void Xbee_Interface_On(void){
	PORT_XBEE_ON |= (1 << XBEE_ON);
}


void Xbee_Interface_Off(void){
	PORT_XBEE_ON &= ~(1 << XBEE_ON);
}


void Xbee_Send_String(char *str){
	UARTn_Tx_String(UART0, str);
}


void Xbee_Build_Data_Accepted_Frame(data_frame_t *data_frame){
	
	sprintf(data_accepted_frame, "emisor%saceptado\r\n\n", data_frame->emitter_node_id_str_ptr);
	
	/* Replace the quotation marks around the node ID by underscores */
	data_accepted_frame[6] = '_';
	data_accepted_frame[14] = '_';
}


void Xbee_Build_Data_Rejected_Frame(data_frame_t *data_frame){

	sprintf(data_rejected_frame, "emisor%srechazado\r\n\n", data_frame->emitter_node_id_str_ptr);

	/* Replace the quotation marks around the node ID by underscores */
	data_accepted_frame[6] = '_';
	data_accepted_frame[14] = '_';
}


void Xbee_Build_Data_Frame(data_frame_t *data_frame){
	
	uint16_t index = 0;
	
	/* Print beginning of frame */
	xbee_data_frame_buffer[0] = '{'; 
	index++;
	/* Load part number header and value*/
	strcpy_P(xbee_data_frame_buffer + index, xbee_part_number_header);
	index +=strlen_P(xbee_part_number_header);
	*(xbee_data_frame_buffer + index) = ':';
	index++;
	strcpy(xbee_data_frame_buffer + index, data_frame->part_number_str_ptr);
	index +=strlen(data_frame->part_number_str_ptr);
	*(xbee_data_frame_buffer + index) = ',';
	index++;
	/* Load emitter node id header and value */
	strcpy_P(xbee_data_frame_buffer + index, xbee_emitter_node_id_header);
	index +=strlen_P(xbee_emitter_node_id_header);
	*(xbee_data_frame_buffer + index) = ':';
	index++;
	strcpy(xbee_data_frame_buffer + index, data_frame->emitter_node_id_str_ptr);
	index +=strlen(data_frame->emitter_node_id_str_ptr);
	*(xbee_data_frame_buffer + index) = ',';
	index++;	
	/* Load receiver central id header and value */
	strcpy_P(xbee_data_frame_buffer + index, xbee_receiver_central_id_header);
	index +=strlen_P(xbee_receiver_central_id_header);
	*(xbee_data_frame_buffer + index) = ':';
	index++;
	strcpy(xbee_data_frame_buffer + index, data_frame->receiver_central_id_str_ptr);
	index +=strlen(data_frame->receiver_central_id_str_ptr);
	*(xbee_data_frame_buffer + index) = ',';
	index++;
	/* Load measure period header and value */
	strcpy_P(xbee_data_frame_buffer + index, xbee_emitter_measure_period_header);
	index +=strlen_P(xbee_emitter_measure_period_header);
	*(xbee_data_frame_buffer + index) = ':';
	index++;	
	strcpy(xbee_data_frame_buffer + index, data_frame->period_str_ptr);
	index +=strlen(data_frame->period_str_ptr);
	*(xbee_data_frame_buffer + index) = ',';
	index++;
	/* Load date header and value */		
	strcpy_P(xbee_data_frame_buffer + index, xbee_data_frame_date_header);
	index +=strlen_P(xbee_data_frame_date_header);
	*(xbee_data_frame_buffer + index) = ':';
	index++;
	strcpy(xbee_data_frame_buffer + index, data_frame->date_str_ptr);
	index +=strlen(data_frame->date_str_ptr);
	*(xbee_data_frame_buffer + index) = ',';
	index++;	
	/* Load time header and value */		
	strcpy_P(xbee_data_frame_buffer + index, xbee_data_frame_time_header);
	index +=strlen_P(xbee_data_frame_time_header);
	*(xbee_data_frame_buffer + index) = ':';
	index++;
	strcpy(xbee_data_frame_buffer + index, data_frame->time_str_ptr);
	index +=strlen(data_frame->time_str_ptr);
	*(xbee_data_frame_buffer + index) = ',';
	index++;
	/* Load sensor state header and value */	
	strcpy_P(xbee_data_frame_buffer + index, xbee_wear_out_state_header);
	index +=strlen_P(xbee_wear_out_state_header);
	*(xbee_data_frame_buffer + index) = ':';
	index++;
	strcpy(xbee_data_frame_buffer + index, data_frame->sensor_status_str_ptr);
	index +=strlen(data_frame->sensor_status_str_ptr);
	*(xbee_data_frame_buffer + index) = ',';
	index++;	
	/* Load battery level header and value */	
	strcpy_P(xbee_data_frame_buffer + index, xbee_battery_level_header);
	index +=strlen_P(xbee_battery_level_header);
	*(xbee_data_frame_buffer + index) = ':';
	index++;
	strcpy(xbee_data_frame_buffer + index, data_frame->battery_str_ptr);
	index +=strlen(data_frame->battery_str_ptr);
	*(xbee_data_frame_buffer + index) = ',';
	index++;	
	/* Load OHM1 header and value */	
	strcpy_P(xbee_data_frame_buffer + index, xbee_emitter_resistance_1_header);
	index +=strlen_P(xbee_emitter_resistance_1_header);
	*(xbee_data_frame_buffer + index) = ':';
	index++;
	strcpy(xbee_data_frame_buffer + index, data_frame->ohm1_str_ptr);
	index +=strlen(data_frame->ohm1_str_ptr);
	*(xbee_data_frame_buffer + index) = ',';
	index++;
	/* Load OHM2 header and value */
	strcpy_P(xbee_data_frame_buffer + index, xbee_emitter_resistance_2_header);
	index +=strlen_P(xbee_emitter_resistance_2_header);
	*(xbee_data_frame_buffer + index) = ':';
	index++;
	strcpy(xbee_data_frame_buffer + index, data_frame->ohm2_str_ptr);
	index +=strlen(data_frame->ohm2_str_ptr);
	/* Print ending of frame */
	strcpy(xbee_data_frame_buffer + index, "}***");							  
}


void Xbee_Send_Data_Frame(void){
	UARTn_Tx_String(UART0, xbee_data_frame_buffer);
}


void Xbee_Serial_Rx_Enable(void){
	
	uint8_t dummy = 0;
	
	/* Clear UART0 rx flag */
	dummy = UDR0;
	/* Enable UART0 receiver */
	UCSR0B |= (1 << RXCIE0);
	/* Enable UART0 rx complete interrupt */
	UCSR0B |= (1 << RXEN0);
}


void Xbee_Serial_Rx_Disable(void){
	/* Disable UART0 rx complete interrupt */
	UCSR0B &= ~(1 << RXEN0);
	/* Disable UART0 receiver */
	UCSR0B &= ~(1 << RXCIE0);
}



data_frame_transfer_status_t Xbee_Send_Data_And_Verify(void){
	
	int8_t tries_count = XBEE_COMMUNICATION_TRIES_NUMBER;
	uint8_t command_received = 0;
	
	data_frame_transfer_status_t data_frame_transfer_status = 0;
	
	/* Clean reception buffer and enable the Xbee reception interrupt */
	Serial_Rx_Buffer_Flush();
	Xbee_Serial_Rx_Enable();
	/* Ask for permission and wait for a response ... repeat if necessary (up to three times) */
	while(tries_count && (!command_received)){
		Xbee_Send_Data_Frame();
		command_received = Serial_Check_If_Command_Received((1 << XBEE_COMM_RESPONSE_FLAG), XBEE_RESPONSE_TIME_MS);
		tries_count--;
	}
	
	//* If a command was received ... */
	if(command_received){
		/* Disable UART rx and interrupt */
		Xbee_Serial_Rx_Disable();
		/* Clean the command buffer*/
		Serial_Command_Buffer_Flush();
		/* Load the received string (command) */
		Serial_Rx_Buffer_Load_Command();
		/* Verify if the data frame was accepted */
		if(Serial_Verify_Command_Received(data_accepted_frame)){
			/* Indicate that the data frame was accepted */
			data_frame_transfer_status = DATA_FRAME_ACCEPTED; 
		}else if(Serial_Verify_Command_Received(data_rejected_frame)){
			/* Indicate that the data frame was accepted */
			data_frame_transfer_status = DATA_FRAME_REJECTED;
		}else{
			/* Indicate that there was a mistake in the transfer */
			data_frame_transfer_status = DATA_FRAME_TRANSFER_ERROR;
		}
		
	}else{
		/* Indicate that a timeout happened */
		data_frame_transfer_status = DATA_FRAME_TIMEOUT;
	}
	
	return data_frame_transfer_status;
	
}



uint8_t *Xbee_Get_Data_Frame_Buffer_Pointer(void){
	
	return xbee_data_frame_buffer;

}



uint8_t Xbee_Send_Backup_Data_Frames(void){
	
	uint16_t frame_address = 0;
	int16_t counter = 0;
	uint8_t n_frames = 0;
	uint8_t i2c_error = I2C_OK;
	data_frame_transfer_status_t frame_received = DATA_FRAME_ACCEPTED;
		
	
	/* Load the data frame counter */
	i2c_error = EEPROM_Load_Data_Frame_Counter(&n_frames);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}
	
	if(n_frames > EEPROM_N_BACKUP_FRAMES){
		n_frames = EEPROM_N_BACKUP_FRAMES;
	}
	
	
	while((counter < n_frames) && (frame_received == DATA_FRAME_ACCEPTED)){
	
		/* Load backup frame from EEPROM */	
		frame_address = EEPROM_FIRST_DATA_FRAME_POSITION + (counter * SYSTEM_DATA_FRAME_MAX_SIZE_IN_BYTES);
		i2c_error = EEPROM_Load_Data_Frame((uint8_t *)xbee_data_frame_buffer, frame_address);
		if(i2c_error == I2C_ERROR){
			break;
		}
		
		frame_received = Xbee_Send_Data_And_Verify();
		counter++;
	}
	
	n_frames = n_frames - counter;
	
	if(counter > 0){
		/* Save the data frame counter */
		i2c_error = EEPROM_Save_Data_Frame_Counter(&n_frames);
		if(i2c_error = I2C_ERROR){
			return i2c_error;
		}
		
		/* Save the data frame counter */
		i2c_error = EEPROM_Save_Data_Frame_Index(&n_frames);	
			
	}
	
	return i2c_error;
}
