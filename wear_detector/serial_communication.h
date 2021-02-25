
#ifndef SERIAL_COMMUNICATION_H_
#define SERIAL_COMMUNICATION_H_

/* Function declaration */
void Serial_Command_Buffer_Flush(void);
void Serial_Rx_Buffer_Flush(void);
void Serial_Rx_Buffer_Store_Byte(uint8_t byte);
void Serial_Rx_Buffer_Load_Command(void);

uint8_t Serial_Check_If_Command_Received(uint16_t flag_mask, uint32_t time_limit_ms);
uint8_t Serial_Verify_Command_Received(char *command_expected);
uint8_t Serial_Verify_Command_Header(char *header_expected);

uint8_t Serial_Get_Measure_Period(uint8_t *data);
void Serial_Get_Emitter_ID(uint8_t *data);
void Serial_Get_Receiver_ID(uint8_t *data);
uint8_t Serial_Get_Number_Of_Sensors(uint8_t *data);
uint8_t Serial_Get_Servo_State(uint8_t *data);
uint8_t Serial_Get_Date_And_Time(uint8_t *data);

void Xbee_Send_Command_Buff(void);
void Bluetooth_Send_Command_Buff(void);

#endif