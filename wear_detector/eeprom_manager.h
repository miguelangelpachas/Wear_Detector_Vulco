
#ifndef EEPROM_MANAGER_H_
#define EEPROM_MANAGER_H_

/* File inclusion */
#include <stdint.h>
#include "project_defines.h"

/* Constants and macros */

#define EEPROM_DATA_BUFFER_SIZE							100			

#define EEPROM_SERVO_INDICATOR_STATE_SIZE_IN_BYTES		SERVO_INDICATOR_STATE_SIZE_IN_BYTES
#define EEPROM_NUMBER_OF_SENSORS_SIZE_IN_BYTES			NUMBER_OF_SENSORS_SIZE_IN_BYTES
#define EEPROM_MAIN_COMMUNICATION_MEDIA_SIZE_IN_BYTES	MAIN_COMMUNICATION_MEDIA_SIZE_IN_BYTES
#define EEPROM_MEASURE_PERIOD_SIZE_IN_BYTES				MEASURE_PERIOD_SIZE_IN_BYTES		
#define EEPROM_PART_NUMBER_SIZE_IN_BYTES				PART_NUMBER_SIZE_IN_BYTES
#define EEPROM_EMITTER_NODE_ID_SIZE_IN_BYTES			EMITTER_NODE_ID_SIZE_IN_BYTES
#define EEPROM_RECEIVER_CENTRAL_ID_SIZE_IN_BYTES		RECEIVER_CENTRAL_ID_SIZE_IN_BYTES
				
#define EEPROM_DATA_FRAME_COUNTER_SIZE_IN_BYTES			1
#define EEPROM_DATA_FRAME_INDEX_SIZE_IN_BYTES			1

#define EEPROM_SERVO_INDICATOR_STATE_POSITION			0x00
#define EEPROM_NUMBER_OF_SENSORS_POSITION				(EEPROM_SERVO_INDICATOR_STATE_POSITION + EEPROM_SERVO_INDICATOR_STATE_SIZE_IN_BYTES)
#define EEPROM_MAIN_COMMUNICATION_MEDIA_POSITION		(EEPROM_NUMBER_OF_SENSORS_POSITION + EEPROM_NUMBER_OF_SENSORS_SIZE_IN_BYTES)
#define EEPROM_MEASURE_PERIOD_POSITION					(EEPROM_MAIN_COMMUNICATION_MEDIA_POSITION + EEPROM_MAIN_COMMUNICATION_MEDIA_SIZE_IN_BYTES)				
#define EEPROM_PART_NUMBER_POSITION						(EEPROM_MEASURE_PERIOD_POSITION + EEPROM_MEASURE_PERIOD_SIZE_IN_BYTES)
#define EEPROM_EMITTER_NODE_ID_POSITION					(EEPROM_PART_NUMBER_POSITION + EEPROM_PART_NUMBER_SIZE_IN_BYTES)
#define EEPROM_RECEIVER_CENTRAL_ID_POSITION				(EEPROM_EMITTER_NODE_ID_POSITION + EEPROM_EMITTER_NODE_ID_SIZE_IN_BYTES)

#define EEPROM_DATA_FRAME_COUNTER_POSITION				(EEPROM_RECEIVER_CENTRAL_ID_POSITION + EEPROM_RECEIVER_CENTRAL_ID_SIZE_IN_BYTES)
#define EEPROM_DATA_FRAME_INDEX_POSITION				(EEPROM_DATA_FRAME_COUNTER_POSITION + EEPROM_DATA_FRAME_COUNTER_SIZE_IN_BYTES)
#define EEPROM_FIRST_DATA_FRAME_POSITION				(EEPROM_DATA_FRAME_INDEX_POSITION + EEPROM_DATA_FRAME_INDEX_SIZE_IN_BYTES)

#define EEPROM_N_BACKUP_FRAMES							20


/* Fiunction declaration */

void EEPROM_Clear_Buffer(void);
uint8_t EEPROM_Get_Data_Frame_Counter(void);
void EEPROM_Set_Data_Frame_Counter(uint8_t counter);
uint8_t EEPROM_Get_Data_Frame_Index(void);
void EEPROM_Set_Data_Frame_Index(uint8_t index);

uint8_t EEPROM_Save_Servo_Indicator_State(uint8_t *data);
uint8_t EEPROM_Save_Number_Of_Sensors(uint8_t *data);
uint8_t EEPROM_Save_Main_Communication_Media(uint8_t *data);
uint8_t EEPROM_Save_Measure_Period_In_Days(uint8_t *data);
uint8_t EEPROM_Save_Part_Number(uint8_t *data);
uint8_t EEPROM_Save_Emitter_Node_ID(uint8_t *data);
uint8_t EEPROM_Save_Receiver_Central_ID(uint8_t *data);
uint8_t EEPROM_Save_Data_Frame_Counter(uint8_t *data);
uint8_t EEPROM_Save_Data_Frame_Index(uint8_t *data);

uint8_t EEPROM_Load_Servo_Indicator_State(uint8_t *data);
uint8_t EEPROM_Load_Number_Of_Sensors(uint8_t *data);
uint8_t EEPROM_Load_Main_Communication_Media(uint8_t *data);
uint8_t EEPROM_Load_Measure_Period_In_Days(uint8_t *data);
uint8_t EEPROM_Load_Part_Number(uint8_t *data);
uint8_t EEPROM_Load_Emitter_Node_ID(uint8_t *data);
uint8_t EEPROM_Load_Receiver_Central_ID(uint8_t *data);
uint8_t EEPROM_Load_Data_Frame_Counter(uint8_t *data);
uint8_t EEPROM_Load_Data_Frame_Index(uint8_t *data);

uint8_t EEPROM_Save_Data_Frame(uint8_t *data, uint16_t pos);
uint8_t EEPROM_Load_Data_Frame(uint8_t *data, uint16_t pos);
uint8_t EEPROM_Clear_All_Data(void);
uint8_t EEPROM_Clear_Data_Frames_Only(void);

uint8_t EEPROM_Save_Backup_Data_Frame(uint8_t *data_frame);
uint8_t EEPROM_Load_Backup_Data_Frame(uint8_t *data_frame);

#endif
