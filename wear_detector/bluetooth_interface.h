

#ifndef BLUETOOTH_INTERFACE_H_
#define BLUETOOTH_INTERFACE_H_

#include "MT_uart.h" 
#include "grinding_mill_wearing_out_monitor_node.h"

void Bluetooth_Interface_Initialize(void);
void Bluetooth_Interface_First_Time_Configuration(void);
void Bluetooth_Interface_Config_Mode_UART_Init(void);
void Bluetooth_Interface_Working_Mode_UART_Init(void);
void Bluetooth_Interface_On(void);
void Bluetooth_Interface_Off(void);
void Bluetooth_Send_String(char *str);

void Bluetooth_Serial_Rx_Enable(void);
void Bluetooth_Serial_Rx_Disable(void);

void Bluetooth_Build_Data_Frame(data_frame_t *data_frame);
void Bluetooth_Send_Data_Frame(void);
uint8_t Bluetooth_Send_Backup_Data_Frames(void);
uint8_t *Bluetooth_Get_Data_Frame_Buffer_Pointer(void);

uint8_t Bluetooth_Verify_Command_Data_Frame_Request(void);
uint8_t Bluetooth_Verify_Command_Emitter_ID_Request(void);
uint8_t Bluetooth_Verify_Command_Receiver_ID_Request(void);
uint8_t Bluetooth_Verify_Command_Part_Number_Request(void);
uint8_t Bluetooth_Verify_Command_Date_And_Time_Request(void);
uint8_t Bluetooth_Verify_Command_EEPROM_Data_Request(void);
uint8_t Bluetooth_Verify_Command_EEPROM_Reset(void);
uint8_t Bluetooth_Verify_Command_Disconnect(void);

uint8_t Bluetooth_Verify_Command_Measure_Period_Set(void);
uint8_t Bluetooth_Verify_Command_Emitter_ID_Set(void);
uint8_t Bluetooth_Verify_Command_Receiver_ID_Set(void);
uint8_t Bluetooth_Verify_Command_Number_Of_Sensors_Set(void);
uint8_t Bluetooth_Verify_Command_Servo_Enable_Disable(void);
uint8_t Bluetooth_Verify_Command_Date_And_Time_Set(void);

#endif