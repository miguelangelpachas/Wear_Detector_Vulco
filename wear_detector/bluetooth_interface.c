

/* File inclusion */
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include "project_defines.h"
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "global.h"
#include "MT_uart.h"
#include "MT_twi_master.h"
#include "MT_delay.h"
#include "eeprom_manager.h"
#include "serial_communication.h"
#include "bluetooth_interface.h"
#include "grinding_mill_wearing_out_monitor_node.h"


/* Store constant strings in the Flash Memory */
const char bluetooth_part_number_by_default [] PROGMEM = PART_NUMBER_STR_BY_DEFAULT;
const char bluetooth_emitter_node_id_by_default[] PROGMEM = EMITTER_NODE_ID_STR_BY_DEFAULT;
const char bluetooth_receiver_central_id_by_default[] PROGMEM = RECEIVER_CENTRAL_ID_STR_BY_DEFAULT;
const char bluetooth_part_number_header[] PROGMEM = PART_NUMBER_HEADER_STR;
const char bluetooth_emitter_node_id_header[] PROGMEM = EMITTER_NODE_ID_HEADER_STR;
const char bluetooth_receiver_central_id_header[] PROGMEM = RECEIVER_CENTRAL_ID_HEADER_STR;
const char bluetooth_emitter_measure_period_header[] PROGMEM = MEASURE_PERIOD_HEADER_STR;
const char bluetooth_data_frame_date_header[] PROGMEM = DATA_FRAME_DATE_HEADER_STR;
const char bluetooth_data_frame_time_header[] PROGMEM = DATA_FRAME_TIME_HEADER_STR;
const char bluetooth_wear_out_state_header[] PROGMEM = WEAR_OUT_STATE_HEADER_STR;
const char bluetooth_battery_level_header[] PROGMEM = BATTERY_LEVEL_HEADER_STR;
const char bluetooth_emitter_resistance_1_header[] PROGMEM = EMITTER_OHM1_HEADER_STR;
const char bluetooth_emitter_resistance_2_header[] PROGMEM = EMITTER_OHM2_HEADER_STR;


/* Variable definition */

char bluetooth_data_frame_buffer[BLUETOOTH_DATA_FRAME_BUFF_SIZE];

const char bt_emitter_id_request_cmd[] PROGMEM = "AI2+IDDET?\r\n\n";
const char bt_receiver_id_request_cmd[] PROGMEM = "AI2+IDREC?\r\n\n";
const char bt_part_number_request_cmd[] PROGMEM = "AI2+PN?\r\n\n";
const char bt_measure_data_request_cmd[] PROGMEM = "AI2+DATA?\r\n\n";
const char bt_date_time_request_cmd[] PROGMEM = "AI2+DATETIME?\r\n\n";
const char bt_eeprom_reset_cmd[] PROGMEM = "AI2+EEPROMRESET\r\n\n";
const char bt_eeprom_data_request_cmd[] PROGMEM = "AI2+EEPROMREAD?\r\n\n";
const char bt_disconnect_cmd[] PROGMEM ="AI2+BTOFF\r\n\n";

const char bt_measure_period_set_cmd_header[BT_MEASURE_PERIOD_SET_HEADER_SIZE] PROGMEM = "AI2+PERIOD=";
const char bt_emitter_id_set_cmd_header[BT_EMITTER_ID_SET_HEADER_SIZE] PROGMEM = "AI2+IDDET=";
const char bt_receiver_id_set_cmd_header[BT_RECEIVER_ID_SET_HEADER_SIZE] PROGMEM = "AI2+IDREC=";
const char bt_date_time_set_cmd_header[BT_DATE_TIME_SET_HEADER_SIZE] PROGMEM = "AI2+DATETIME=";
const char bt_number_of_sensors_set_cmd_header[BT_NUM_SENSORS_SET_HEADER_SIZE] PROGMEM = "AI2+NSENSORS=";
const char bt_servo_enable_disable_cmd_header[BT_SERVO_ENABLE_DISABLE_HEADER_SIZE] PROGMEM = "AI2+SERVO=";
const char bt_password_by_default[] PROGMEM = BLUETOOTH_PIN_BY_DEFAULT;


char bt_aux_buffer[30];

  

char mode = '0'; // Operation mode (keep it in slave mode)

UART_Baud_Rate_t bt_baud_val = BLUETOOTH_WORKING_MODE_BAUD_RATE;


/* Function definition */

void Bluetooth_Interface_Initialize(void){

	/* Clear the bluetooth key */
	DDR_BT_KEY |= (1 << BT_KEY);
	PORT_BT_KEY &= ~(1 << BT_KEY);

	/* Disable the bluetooth module */
	DDR_BT_ON |= (1 << BT_ON);
	PORT_BT_ON &= ~(1 << BT_ON);
	
	/* Configure UART according to the working parameters */
	Bluetooth_Interface_Working_Mode_UART_Init();
	
}


void Bluetooth_Interface_First_Time_Configuration(void){

	char temp_buff[20];

	/* Configure the UART to configure the bluetooth module */
	Bluetooth_Interface_Config_Mode_UART_Init();

	/* Enable the bluetooth module and wait for it to enter to AT COMMAND MODE */
	PORT_BT_KEY |= (1 << BT_KEY);
	PORT_BT_ON |= (1 << BT_ON);
	Delay_ms(1000);

	/* Configure the device name */
	strcpy_P(temp_buff, bluetooth_part_number_by_default);
	*(temp_buff + 11) = '\0';
	sprintf(bt_aux_buffer, "AT+NAME=%s\r\n", temp_buff + 1);
	UARTn_Tx_String(UART1, bt_aux_buffer);
	Delay_ms(100);

	/* Configure the access PIN (Password) */
	strcpy_P(temp_buff, bt_password_by_default);
	sprintf(bt_aux_buffer, "AT+PSWD=%s\r\n", temp_buff);
	UARTn_Tx_String(UART1, bt_aux_buffer);
	Delay_ms(100);
	
	/* Configure the device in slave mode */
	sprintf(bt_aux_buffer, "AT+ROLE=%c\r\n", mode);
	UARTn_Tx_String(UART1, bt_aux_buffer);
	Delay_ms(100);

	/* Configure UART: baud rate 9600 bps */
	sprintf(bt_aux_buffer, "AT+UART=%lu,0,0\r\n", bt_baud_val);
	UARTn_Tx_String(UART1, bt_aux_buffer);
	Delay_ms(100);
	
	/* Disable the Bluetooth Module */
	PORT_BT_ON &= ~(1 << BT_ON);
	PORT_BT_KEY &= ~(1 << BT_KEY);
	Delay_ms(1000);
	
	/* Configure UART to work with the bluetooth module */
	Bluetooth_Interface_Working_Mode_UART_Init();	
}




void Bluetooth_Interface_Config_Mode_UART_Init(void){
	
	UART_t my_uart;

	my_uart.baud_rate = BLUETOOTH_CONFIG_MODE_BAUD_RATE;
	my_uart.data_bits = UART_8_Data_Bits;
	my_uart.parity_mode = UART_Parity_Disabled;
	my_uart.stop_bits= UART_1_Stop_Bit;
	my_uart.u2x = U2X_Enabled;
	my_uart.tx_status = UART_Tx_Enabled;
	my_uart.rx_status = UART_Rx_Disabled;
	my_uart.interrupt_mask = UART_Interrupts_Disabled;
	
	UART1_Configurar(&my_uart);
}


void Bluetooth_Interface_Working_Mode_UART_Init(void){
	
	UART_t my_uart;

	my_uart.baud_rate = BLUETOOTH_WORKING_MODE_BAUD_RATE;
	my_uart.data_bits = UART_8_Data_Bits;
	my_uart.parity_mode = UART_Parity_Disabled;
	my_uart.stop_bits= UART_1_Stop_Bit;
	my_uart.u2x = U2X_Enabled;
	my_uart.tx_status = UART_Tx_Enabled;
	my_uart.rx_status = UART_Rx_Disabled;
	my_uart.interrupt_mask = UART_Interrupts_Disabled;

	UART1_Configurar(&my_uart);
	
}



void Bluetooth_Interface_On(void){
	PORT_BT_ON |= (1 << BT_ON);
}


void Bluetooth_Interface_Off(void){	
	PORT_BT_ON &= ~(1 << BT_ON);
}


void Bluetooth_Send_String(char *str){
	UARTn_Tx_String(UART1, str);
}


void Bluetooth_Build_Data_Frame(data_frame_t *data_frame){

	uint16_t index = 0;
	
	/* Print beginning of frame */
	bluetooth_data_frame_buffer[0] = '{'; 
	index++;
	/* Load part number header and value*/
	strcpy_P(bluetooth_data_frame_buffer + index, bluetooth_part_number_header);
	index +=strlen_P(bluetooth_part_number_header);
	*(bluetooth_data_frame_buffer + index) = ':';
	index++;
	strcpy(bluetooth_data_frame_buffer + index, data_frame->part_number_str_ptr);
	index +=strlen(data_frame->part_number_str_ptr);
	*(bluetooth_data_frame_buffer + index) = ',';
	index++;
	/* Load emitter node id header and value */
	strcpy_P(bluetooth_data_frame_buffer + index, bluetooth_emitter_node_id_header);
	index +=strlen_P(bluetooth_emitter_node_id_header);
	*(bluetooth_data_frame_buffer + index) = ':';
	index++;
	strcpy(bluetooth_data_frame_buffer + index, data_frame->emitter_node_id_str_ptr);
	index +=strlen(data_frame->emitter_node_id_str_ptr);
	*(bluetooth_data_frame_buffer + index) = ',';
	index++;	
	/* Load receiver central id header and value */
	strcpy_P(bluetooth_data_frame_buffer + index, bluetooth_receiver_central_id_header);
	index +=strlen_P(bluetooth_receiver_central_id_header);
	*(bluetooth_data_frame_buffer + index) = ':';
	index++;
	strcpy(bluetooth_data_frame_buffer + index, data_frame->receiver_central_id_str_ptr);
	index +=strlen(data_frame->receiver_central_id_str_ptr);
	*(bluetooth_data_frame_buffer + index) = ',';
	index++;
	/* Load measure period header and value */
	strcpy_P(bluetooth_data_frame_buffer + index, bluetooth_emitter_measure_period_header);
	index +=strlen_P(bluetooth_emitter_measure_period_header);
	*(bluetooth_data_frame_buffer + index) = ':';
	index++;
	strcpy(bluetooth_data_frame_buffer + index, data_frame->period_str_ptr);
	index +=strlen(data_frame->period_str_ptr);
	*(bluetooth_data_frame_buffer + index) = ',';
	index++;	
	/* Load date header and value */		
	strcpy_P(bluetooth_data_frame_buffer + index, bluetooth_data_frame_date_header);
	index +=strlen_P(bluetooth_data_frame_date_header);
	*(bluetooth_data_frame_buffer + index) = ':';
	index++;
	strcpy(bluetooth_data_frame_buffer + index, data_frame->date_str_ptr);
	index +=strlen(data_frame->date_str_ptr);
	*(bluetooth_data_frame_buffer + index) = ',';
	index++;	
	/* Load time header and value */		
	strcpy_P(bluetooth_data_frame_buffer + index, bluetooth_data_frame_time_header);
	index +=strlen_P(bluetooth_data_frame_time_header);
	*(bluetooth_data_frame_buffer + index) = ':';
	index++;
	strcpy(bluetooth_data_frame_buffer + index, data_frame->time_str_ptr);
	index +=strlen(data_frame->time_str_ptr);
	*(bluetooth_data_frame_buffer + index) = ',';
	index++;
	/* Load sensor state header and value */	
	strcpy_P(bluetooth_data_frame_buffer + index, bluetooth_wear_out_state_header);
	index +=strlen_P(bluetooth_wear_out_state_header);
	*(bluetooth_data_frame_buffer + index) = ':';
	index++;
	strcpy(bluetooth_data_frame_buffer + index, data_frame->sensor_status_str_ptr);
	index +=strlen(data_frame->sensor_status_str_ptr);
	*(bluetooth_data_frame_buffer + index) = ',';
	index++;	
	/* Load battery level header and value */	
	strcpy_P(bluetooth_data_frame_buffer + index, bluetooth_battery_level_header);
	index +=strlen_P(bluetooth_battery_level_header);
	*(bluetooth_data_frame_buffer + index) = ':';
	index++;
	strcpy(bluetooth_data_frame_buffer + index, data_frame->battery_str_ptr);
	index +=strlen(data_frame->battery_str_ptr);
	*(bluetooth_data_frame_buffer + index) = ',';
	index++;	
	/* Load OHM1 header and value */	
	strcpy_P(bluetooth_data_frame_buffer + index, bluetooth_emitter_resistance_1_header);
	index +=strlen_P(bluetooth_emitter_resistance_1_header);
	*(bluetooth_data_frame_buffer + index) = ':';
	index++;
	strcpy(bluetooth_data_frame_buffer + index, data_frame->ohm1_str_ptr);
	index +=strlen(data_frame->ohm1_str_ptr);
	*(bluetooth_data_frame_buffer + index) = ',';
	index++;
	/* Load OHM2 header and value */
	strcpy_P(bluetooth_data_frame_buffer + index, bluetooth_emitter_resistance_2_header);
	index +=strlen_P(bluetooth_emitter_resistance_2_header);
	*(bluetooth_data_frame_buffer + index) = ':';
	index++;
	strcpy(bluetooth_data_frame_buffer + index, data_frame->ohm2_str_ptr);
	index +=strlen(data_frame->ohm2_str_ptr);
	/* Print ending of frame */
	strcpy(bluetooth_data_frame_buffer + index, "}***");				

}


void Bluetooth_Send_Data_Frame(void){
	UARTn_Tx_String(UART1, bluetooth_data_frame_buffer);
}


uint8_t *Bluetooth_Get_Data_Frame_Buffer_Pointer(void){
	return bluetooth_data_frame_buffer;
}


uint8_t Bluetooth_Send_Backup_Data_Frames(void){
	
	uint16_t frame_address = 0;
	uint8_t counter = 0;
	uint8_t n_frames = 0;
	uint8_t i2c_error = I2C_OK;
	
	i2c_error = EEPROM_Load_Data_Frame_Counter(&n_frames);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}
	
	if(n_frames > EEPROM_N_BACKUP_FRAMES){
		n_frames = EEPROM_N_BACKUP_FRAMES;
	}
	
	
	while(counter < n_frames){
		frame_address = EEPROM_FIRST_DATA_FRAME_POSITION + (counter * SYSTEM_DATA_FRAME_MAX_SIZE_IN_BYTES);
		i2c_error = EEPROM_Load_Data_Frame((uint8_t *)bluetooth_data_frame_buffer, frame_address);
		if(i2c_error){
			break;
		}
		Bluetooth_Send_Data_Frame();
		counter++;
	}
	
	if(n_frames == 0){
		Bluetooth_Send_String("No hay datos para mostrar.");
	}
	
	return i2c_error;
}


void Bluetooth_Serial_Rx_Enable(void){
	
	uint8_t dummy = 0;
	
	/* Clear UART1 rx flag */
	dummy = UDR1;
	/* Enable UART1 receiver */
	UCSR1B |= (1 << RXCIE1);
	/* Enable UART1 rx complete interrupt */
	UCSR1B |= (1 << RXEN1);
}


void Bluetooth_Serial_Rx_Disable(void){
	/* Disable UART0 rx complete interrupt */
	UCSR1B &= ~(1 << RXEN1);
	/* Disable UART0 receiver */
	UCSR1B &= ~(1 << RXCIE1);
}



uint8_t Bluetooth_Verify_Command_Part_Number_Request(void){
	
	uint8_t command_correct = 0;
	
	/* Disable UART rx and interrupt */
	Bluetooth_Serial_Rx_Disable();
	/* Clean the command buffer*/
	Serial_Command_Buffer_Flush();
	/* Load the received string (command) */
	Serial_Rx_Buffer_Load_Command();
	/* Verify if the expected command was received */
	strcpy_P(bt_aux_buffer, bt_part_number_request_cmd);
	if(Serial_Verify_Command_Received(bt_aux_buffer)){
		command_correct = 1;
	}else{
		command_correct = 0;
	}
	
	return command_correct;
	
}


uint8_t Bluetooth_Verify_Command_Emitter_ID_Request(void){
	
	uint8_t command_correct = 0;
	
	/* Disable UART rx and interrupt */
	Bluetooth_Serial_Rx_Disable();
	/* Clean the command buffer*/
	Serial_Command_Buffer_Flush();
	/* Load the received string (command) */
	Serial_Rx_Buffer_Load_Command();
	/* Verify if the expected command was received */
	strcpy_P(bt_aux_buffer, bt_emitter_id_request_cmd);
	if(Serial_Verify_Command_Received(bt_aux_buffer)){
		command_correct = 1;
	}else{
		command_correct = 0;
	}
	
	return command_correct;
	
}


uint8_t Bluetooth_Verify_Command_Receiver_ID_Request(void){
	
	uint8_t command_correct = 0;
	
	/* Disable UART rx and interrupt */
	Bluetooth_Serial_Rx_Disable();
	/* Clean the command buffer*/
	Serial_Command_Buffer_Flush();
	/* Load the received string (command) */
	Serial_Rx_Buffer_Load_Command();
	/* Verify if the expected command was received */
	strcpy_P(bt_aux_buffer, bt_receiver_id_request_cmd);
	if(Serial_Verify_Command_Received(bt_aux_buffer)){
		command_correct = 1;
	}else{
		command_correct = 0;
	}
	
	return command_correct;
}

uint8_t Bluetooth_Verify_Command_Data_Frame_Request(void){
	
	uint8_t command_correct = 0;
	
	/* Disable UART rx and interrupt */
	Bluetooth_Serial_Rx_Disable();
	/* Clean the command buffer*/
	Serial_Command_Buffer_Flush();
	/* Load the received string (command) */
	Serial_Rx_Buffer_Load_Command();
	/* Verify if the expected command was received */
	strcpy_P(bt_aux_buffer, bt_measure_data_request_cmd);
	if(Serial_Verify_Command_Received(bt_aux_buffer)){
		command_correct = 1;
	}else{
		command_correct = 0;
	}
	
	return command_correct;
}

uint8_t Bluetooth_Verify_Command_Date_And_Time_Request(void){

	uint8_t command_correct = 0;
	
	/* Disable UART rx and interrupt */
	Bluetooth_Serial_Rx_Disable();
	/* Clean the command buffer*/
	Serial_Command_Buffer_Flush();
	/* Load the received string (command) */
	Serial_Rx_Buffer_Load_Command();
	/* Verify if the expected command was received */
	strcpy_P(bt_aux_buffer, bt_date_time_request_cmd);
	if(Serial_Verify_Command_Received(bt_aux_buffer)){
		command_correct = 1;
	}else{
		command_correct = 0;
	}
	
	return command_correct;	
}


uint8_t Bluetooth_Verify_Command_EEPROM_Data_Request(void){

	uint8_t command_correct = 0;
	
	/* Disable UART rx and interrupt */
	Bluetooth_Serial_Rx_Disable();
	/* Clean the command buffer*/
	Serial_Command_Buffer_Flush();
	/* Load the received string (command) */
	Serial_Rx_Buffer_Load_Command();
	/* Verify if the expected command was received */
	strcpy_P(bt_aux_buffer, bt_eeprom_data_request_cmd);
	if(Serial_Verify_Command_Received(bt_aux_buffer)){
		command_correct = 1;
	}else{
		command_correct = 0;
	}
	
	return command_correct;	
}


uint8_t Bluetooth_Verify_Command_EEPROM_Reset(void){

	uint8_t command_correct = 0;
	
	/* Disable UART rx and interrupt */
	Bluetooth_Serial_Rx_Disable();
	/* Clean the command buffer*/
	Serial_Command_Buffer_Flush();
	/* Load the received string (command) */
	Serial_Rx_Buffer_Load_Command();
	/* Verify if the expected command was received */
	strcpy_P(bt_aux_buffer, bt_eeprom_reset_cmd);
	if(Serial_Verify_Command_Received(bt_aux_buffer)){
		command_correct = 1;
	}else{
		command_correct = 0;
	}
	
	return command_correct;
}



uint8_t Bluetooth_Verify_Command_Disconnect(void){
	
	uint8_t command_correct = 0;

	/* Disable UART rx and interrupt */
	Bluetooth_Serial_Rx_Disable();
	/* Clean the command buffer*/
	Serial_Command_Buffer_Flush();
	/* Load the received string (command) */
	Serial_Rx_Buffer_Load_Command();
	/* Verify if the expected command was received */
	strcpy_P(bt_aux_buffer, bt_disconnect_cmd);
	if(Serial_Verify_Command_Received(bt_aux_buffer)){
		command_correct = 1;
	}else{
		command_correct = 0;
	}

	return command_correct;		
}



uint8_t Bluetooth_Verify_Command_Measure_Period_Set(void){

	uint8_t command_correct = 0;
	
	/* Disable UART rx and interrupt */
	Bluetooth_Serial_Rx_Disable();
	/* Clean the command buffer*/
	Serial_Command_Buffer_Flush();
	/* Load the received string (command) */
	Serial_Rx_Buffer_Load_Command();
	/* Verify if the expected command was received */
	strcpy_P(bt_aux_buffer, bt_measure_period_set_cmd_header);
	if(Serial_Verify_Command_Header(bt_aux_buffer)){
		command_correct = 1;
	}else{
		command_correct = 0;
	}
	
	return command_correct;
}



uint8_t Bluetooth_Verify_Command_Emitter_ID_Set(void){

	uint8_t command_correct = 0;
	
	/* Disable UART rx and interrupt */
	Bluetooth_Serial_Rx_Disable();
	/* Clean the command buffer*/
	Serial_Command_Buffer_Flush();
	/* Load the received string (command) */
	Serial_Rx_Buffer_Load_Command();
	/* Verify if the expected command was received */
	strcpy_P(bt_aux_buffer, bt_emitter_id_set_cmd_header);
	if(Serial_Verify_Command_Header(bt_aux_buffer)){
		command_correct = 1;
	}else{
		command_correct = 0;
	}
	
	return command_correct;
	
}


uint8_t Bluetooth_Verify_Command_Receiver_ID_Set(void){

	uint8_t command_correct = 0;
	
	/* Disable UART rx and interrupt */
	Bluetooth_Serial_Rx_Disable();
	/* Clean the command buffer*/
	Serial_Command_Buffer_Flush();
	/* Load the received string (command) */
	Serial_Rx_Buffer_Load_Command();
	/* Verify if the expected command was received */
	strcpy_P(bt_aux_buffer, bt_receiver_id_set_cmd_header);
	if(Serial_Verify_Command_Header(bt_aux_buffer)){
		command_correct = 1;
	}else{
		command_correct = 0;
	}
	
	return command_correct;	
	
}



uint8_t Bluetooth_Verify_Command_Number_Of_Sensors_Set(void){

	uint8_t command_correct = 0;
	
	/* Disable UART rx and interrupt */
	Bluetooth_Serial_Rx_Disable();
	/* Clean the command buffer*/
	Serial_Command_Buffer_Flush();
	/* Load the received string (command) */
	Serial_Rx_Buffer_Load_Command();
	/* Verify if the expected command was received */
	strcpy_P(bt_aux_buffer, bt_number_of_sensors_set_cmd_header);
	if(Serial_Verify_Command_Header(bt_aux_buffer)){
		command_correct = 1;
	}else{
		command_correct = 0;
	}
	
	return command_correct;
	
}


uint8_t Bluetooth_Verify_Command_Servo_Enable_Disable(void){

	uint8_t command_correct = 0;
	
	/* Disable UART rx and interrupt */
	Bluetooth_Serial_Rx_Disable();
	/* Clean the command buffer*/
	Serial_Command_Buffer_Flush();
	/* Load the received string (command) */
	Serial_Rx_Buffer_Load_Command();
	/* Verify if the expected command was received */
	strcpy_P(bt_aux_buffer, bt_servo_enable_disable_cmd_header);
	if(Serial_Verify_Command_Header(bt_aux_buffer)){
		command_correct = 1;
	}else{
		command_correct = 0;
	}
	
	return command_correct;
	
	
}


uint8_t Bluetooth_Verify_Command_Date_And_Time_Set(void){
	
	uint8_t command_correct = 0;
	
	/* Disable UART rx and interrupt */
	Bluetooth_Serial_Rx_Disable();
	/* Clean the command buffer*/
	Serial_Command_Buffer_Flush();
	/* Load the received string (command) */
	Serial_Rx_Buffer_Load_Command();
	/* Verify if the expected command was received */
	strcpy_P(bt_aux_buffer, bt_date_time_set_cmd_header);
	if(Serial_Verify_Command_Header(bt_aux_buffer)){
		command_correct = 1;
	}else{
		command_correct = 0;
	}
	
	return command_correct;
	
}

