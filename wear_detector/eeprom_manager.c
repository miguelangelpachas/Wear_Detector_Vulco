

/* File inclusion */
#include <stdint.h>
#include "project_defines.h"
#include "MT_twi_master.h"
#include "MT_24AAxx.h"
#include "eeprom_manager.h"

/* Variable definition */

uint8_t eeprom_data_frame_index = 0;
uint8_t eeprom_data_frame_counter = 0;
uint8_t eeprom_buffer[EEPROM_DATA_BUFFER_SIZE];

/* Function definition */


void EEPROM_Clear_Buffer(void){
	
	uint16_t index;
	
	for(index = 0; index < EEPROM_DATA_BUFFER_SIZE; index++){
		eeprom_buffer[index] = 0;
	}
	
}


uint8_t EEPROM_Get_Data_Frame_Counter(void){
	return eeprom_data_frame_counter;
}


void EEPROM_Set_Data_Frame_Counter(uint8_t counter){
	eeprom_data_frame_counter = counter;
}


uint8_t EEPROM_Get_Data_Frame_Index(void){
	return eeprom_data_frame_index;
}


void EEPROM_Set_Data_Frame_Index(uint8_t index){
	eeprom_data_frame_index = index;	
}


uint8_t EEPROM_Save_Servo_Indicator_State(uint8_t *data){
	
	uint8_t i2c_error = I2C_OK;

	i2c_error = EEPROM_24AAxx_Write_Extended(EEPROM_SLAVE_ADDRESS, EEPROM_SERVO_INDICATOR_STATE_POSITION, EEPROM_SERVO_INDICATOR_STATE_SIZE_IN_BYTES, data);

	return i2c_error;
	
}


uint8_t EEPROM_Save_Number_Of_Sensors(uint8_t *data){

	uint8_t i2c_error = I2C_OK;
	
	i2c_error = EEPROM_24AAxx_Write_Extended(EEPROM_SLAVE_ADDRESS, EEPROM_NUMBER_OF_SENSORS_POSITION, EEPROM_NUMBER_OF_SENSORS_SIZE_IN_BYTES, data);
	
	return i2c_error;
	
}


uint8_t EEPROM_Save_Main_Communication_Media(uint8_t *data){

	uint8_t i2c_error = I2C_OK;
	
	i2c_error = EEPROM_24AAxx_Write_Extended(EEPROM_SLAVE_ADDRESS, EEPROM_MAIN_COMMUNICATION_MEDIA_POSITION, EEPROM_MAIN_COMMUNICATION_MEDIA_SIZE_IN_BYTES, data);
	
	return i2c_error;	
	
}


uint8_t EEPROM_Save_Measure_Period_In_Days(uint8_t *data){

	uint8_t i2c_error = I2C_OK;
	
	i2c_error = EEPROM_24AAxx_Write_Extended(EEPROM_SLAVE_ADDRESS, EEPROM_MEASURE_PERIOD_POSITION, EEPROM_MEASURE_PERIOD_SIZE_IN_BYTES, data);
	
	return i2c_error;
	
}


uint8_t EEPROM_Save_Part_Number(uint8_t *data){

	uint8_t i2c_error = I2C_OK;
	
	i2c_error = EEPROM_24AAxx_Write_Extended(EEPROM_SLAVE_ADDRESS, EEPROM_PART_NUMBER_POSITION, EEPROM_PART_NUMBER_SIZE_IN_BYTES, data);
	
	return i2c_error;
	
}


uint8_t EEPROM_Save_Emitter_Node_ID(uint8_t *data){

	uint8_t i2c_error = I2C_OK;
	
	i2c_error = EEPROM_24AAxx_Write_Extended(EEPROM_SLAVE_ADDRESS, EEPROM_EMITTER_NODE_ID_POSITION, EEPROM_EMITTER_NODE_ID_SIZE_IN_BYTES, data);
	
	return i2c_error;
	
}


uint8_t EEPROM_Save_Receiver_Central_ID(uint8_t *data){

	uint8_t i2c_error = I2C_OK;
	
	i2c_error = EEPROM_24AAxx_Write_Extended(EEPROM_SLAVE_ADDRESS, EEPROM_RECEIVER_CENTRAL_ID_POSITION, EEPROM_RECEIVER_CENTRAL_ID_SIZE_IN_BYTES, data);
	
	return i2c_error;
	
}


uint8_t EEPROM_Save_Data_Frame_Counter(uint8_t *data){

	uint8_t i2c_error = I2C_OK;
	
	i2c_error = EEPROM_24AAxx_Write_Extended(EEPROM_SLAVE_ADDRESS, EEPROM_DATA_FRAME_COUNTER_POSITION, EEPROM_DATA_FRAME_COUNTER_SIZE_IN_BYTES, data);
	
	return i2c_error;
	
}


uint8_t EEPROM_Save_Data_Frame_Index(uint8_t *data){
	
	uint8_t i2c_error = I2C_OK;
		
	i2c_error = EEPROM_24AAxx_Write_Extended(EEPROM_SLAVE_ADDRESS, EEPROM_DATA_FRAME_INDEX_POSITION, EEPROM_DATA_FRAME_INDEX_SIZE_IN_BYTES, data);
		
	return i2c_error;
}



uint8_t EEPROM_Load_Servo_Indicator_State(uint8_t *data){

	uint8_t i2c_error = I2C_OK;
	
	i2c_error = EEPROM_24AAxx_Read_Extended(EEPROM_SLAVE_ADDRESS, EEPROM_SERVO_INDICATOR_STATE_POSITION, EEPROM_SERVO_INDICATOR_STATE_SIZE_IN_BYTES, data);
	
	return i2c_error;
}



uint8_t EEPROM_Load_Number_Of_Sensors(uint8_t *data){

	uint8_t i2c_error = I2C_OK;
	
	i2c_error = EEPROM_24AAxx_Read_Extended(EEPROM_SLAVE_ADDRESS, EEPROM_NUMBER_OF_SENSORS_POSITION, EEPROM_NUMBER_OF_SENSORS_SIZE_IN_BYTES, data);
	
	return i2c_error;
}


uint8_t EEPROM_Load_Main_Communication_Media(uint8_t *data){

	uint8_t i2c_error = I2C_OK;
	
	i2c_error = EEPROM_24AAxx_Read_Extended(EEPROM_SLAVE_ADDRESS, EEPROM_MAIN_COMMUNICATION_MEDIA_POSITION, EEPROM_MAIN_COMMUNICATION_MEDIA_SIZE_IN_BYTES, data);
	
	return i2c_error;
}


uint8_t EEPROM_Load_Measure_Period_In_Days(uint8_t *data){

	uint8_t i2c_error = I2C_OK;
	
	i2c_error = EEPROM_24AAxx_Read_Extended(EEPROM_SLAVE_ADDRESS, EEPROM_MEASURE_PERIOD_POSITION, EEPROM_MEASURE_PERIOD_SIZE_IN_BYTES, data);
	
	return i2c_error;	
}


uint8_t EEPROM_Load_Part_Number(uint8_t *data){

	uint8_t i2c_error = I2C_OK;
	
	i2c_error = EEPROM_24AAxx_Read_Extended(EEPROM_SLAVE_ADDRESS, EEPROM_PART_NUMBER_POSITION, EEPROM_PART_NUMBER_SIZE_IN_BYTES, data);
	
	return i2c_error;
	
}



uint8_t EEPROM_Load_Emitter_Node_ID(uint8_t *data){

	uint8_t i2c_error = I2C_OK;
	
	i2c_error = EEPROM_24AAxx_Read_Extended(EEPROM_SLAVE_ADDRESS, EEPROM_EMITTER_NODE_ID_POSITION, EEPROM_EMITTER_NODE_ID_SIZE_IN_BYTES, data);
	
	return i2c_error;
	
}


uint8_t EEPROM_Load_Receiver_Central_ID(uint8_t *data){

	uint8_t i2c_error = I2C_OK;
	
	i2c_error = EEPROM_24AAxx_Read_Extended(EEPROM_SLAVE_ADDRESS, EEPROM_RECEIVER_CENTRAL_ID_POSITION, EEPROM_RECEIVER_CENTRAL_ID_SIZE_IN_BYTES, data);
	
	return i2c_error;
	
}


uint8_t EEPROM_Load_Data_Frame_Counter(uint8_t *data){

	uint8_t i2c_error = I2C_OK;
	
	i2c_error = EEPROM_24AAxx_Read_Extended(EEPROM_SLAVE_ADDRESS, EEPROM_DATA_FRAME_COUNTER_POSITION, EEPROM_DATA_FRAME_COUNTER_SIZE_IN_BYTES, data);
	
	return i2c_error;
	
}


uint8_t EEPROM_Load_Data_Frame_Index(uint8_t *data){

	uint8_t i2c_error = I2C_OK;
	
	i2c_error = EEPROM_24AAxx_Read_Extended(EEPROM_SLAVE_ADDRESS, EEPROM_DATA_FRAME_INDEX_POSITION, EEPROM_DATA_FRAME_INDEX_SIZE_IN_BYTES, data);
	
	return i2c_error;	
	
}



uint8_t EEPROM_Save_Data_Frame(uint8_t *data, uint16_t pos){

	uint8_t i2c_error = I2C_OK;
	
	i2c_error = EEPROM_24AAxx_Write_Extended(EEPROM_SLAVE_ADDRESS, pos, SYSTEM_DATA_FRAME_MAX_SIZE_IN_BYTES, data);
	
	return i2c_error;
	
}


uint8_t EEPROM_Load_Data_Frame(uint8_t *data, uint16_t pos){

	uint8_t i2c_error = I2C_OK;
	
	i2c_error = EEPROM_24AAxx_Read_Extended(EEPROM_SLAVE_ADDRESS, pos, SYSTEM_DATA_FRAME_MAX_SIZE_IN_BYTES, data);
	
	return i2c_error;
	
}


uint8_t EEPROM_Clear_All_Data(void){
	
	uint8_t i2c_error = I2C_OK;

	i2c_error = EEPROM_24AAxx_Clear_Extended(EEPROM_SLAVE_ADDRESS, EEPROM_NUMBER_OF_SENSORS_POSITION, EEPROM_NUMBER_OF_SENSORS_SIZE_IN_BYTES);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}
	
	i2c_error = EEPROM_24AAxx_Clear_Extended(EEPROM_SLAVE_ADDRESS, EEPROM_MAIN_COMMUNICATION_MEDIA_POSITION, EEPROM_MAIN_COMMUNICATION_MEDIA_SIZE_IN_BYTES);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}

	i2c_error = EEPROM_24AAxx_Clear_Extended(EEPROM_SLAVE_ADDRESS, EEPROM_MEASURE_PERIOD_POSITION, EEPROM_MEASURE_PERIOD_SIZE_IN_BYTES);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}
	
	i2c_error = EEPROM_24AAxx_Clear_Extended(EEPROM_SLAVE_ADDRESS, EEPROM_PART_NUMBER_POSITION, EEPROM_PART_NUMBER_SIZE_IN_BYTES);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}
		
	i2c_error = EEPROM_24AAxx_Clear_Extended(EEPROM_SLAVE_ADDRESS, EEPROM_EMITTER_NODE_ID_POSITION, EEPROM_EMITTER_NODE_ID_SIZE_IN_BYTES);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}	
	
	i2c_error = EEPROM_24AAxx_Clear_Extended(EEPROM_SLAVE_ADDRESS, EEPROM_RECEIVER_CENTRAL_ID_POSITION, EEPROM_RECEIVER_CENTRAL_ID_SIZE_IN_BYTES);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}	
	
	i2c_error = EEPROM_24AAxx_Clear_Extended(EEPROM_SLAVE_ADDRESS, EEPROM_DATA_FRAME_COUNTER_POSITION, EEPROM_DATA_FRAME_COUNTER_SIZE_IN_BYTES);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}	
	
	i2c_error = EEPROM_24AAxx_Clear_Extended(EEPROM_SLAVE_ADDRESS, EEPROM_DATA_FRAME_INDEX_POSITION, EEPROM_DATA_FRAME_INDEX_SIZE_IN_BYTES);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}	
	
	i2c_error = EEPROM_Clear_Data_Frames_Only();
	
	return i2c_error;
	
}


uint8_t EEPROM_Clear_Data_Frames_Only(void){

	uint8_t i2c_error = I2C_OK;
	uint16_t index;
	uint16_t frame_pos;
	uint8_t temp = 0;
	
	i2c_error = EEPROM_Save_Data_Frame_Counter(&temp);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}
	
	i2c_error = EEPROM_Save_Data_Frame_Index(&temp);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}
		
	for(index = 0; index < EEPROM_N_BACKUP_FRAMES; index++){
		frame_pos = EEPROM_FIRST_DATA_FRAME_POSITION + (SYSTEM_DATA_FRAME_MAX_SIZE_IN_BYTES * index);
		i2c_error = EEPROM_24AAxx_Clear_Extended(EEPROM_SLAVE_ADDRESS, frame_pos, SYSTEM_DATA_FRAME_MAX_SIZE_IN_BYTES);
		if(i2c_error == I2C_ERROR){
			break;
		}
	}
	
	return i2c_error;	
}


uint8_t EEPROM_Save_Backup_Data_Frame(uint8_t *data_frame){
	
	uint8_t i2c_error = I2C_OK;
	uint16_t pos;

	/* Load frame counter from EEPROM */	
	i2c_error = EEPROM_Load_Data_Frame_Counter(&eeprom_data_frame_counter);  
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}
	
	/* Load frame index from EEPROM */
	i2c_error = EEPROM_Load_Data_Frame_Index(&eeprom_data_frame_index);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}
	
	/* Determine the current position in memory */
	pos = EEPROM_FIRST_DATA_FRAME_POSITION + (eeprom_data_frame_index * SYSTEM_DATA_FRAME_MAX_SIZE_IN_BYTES);
	
	/* Save backup data frame in EEPROM */
	i2c_error = EEPROM_Save_Data_Frame(data_frame, pos);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}
	
	/* Update data frame index */
	eeprom_data_frame_index++;
	if(eeprom_data_frame_index >= EEPROM_N_BACKUP_FRAMES){
		eeprom_data_frame_index = 0;
	}

	/* Update data frame counter */
	eeprom_data_frame_counter++;
	if(eeprom_data_frame_counter >= EEPROM_N_BACKUP_FRAMES){
		eeprom_data_frame_counter = EEPROM_N_BACKUP_FRAMES; 
	}
	
	/* Save the new value of frame index */
	i2c_error = EEPROM_Save_Data_Frame_Index(&eeprom_data_frame_index);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}	
	
	/* Save the new value of frame counter */		
	i2c_error = EEPROM_Save_Data_Frame_Counter(&eeprom_data_frame_counter);

	return i2c_error;
}



uint8_t EEPROM_Load_Backup_Data_Frame(uint8_t *data_frame){
	
	uint8_t i2c_error = I2C_OK;
	uint16_t pos;

	/* Load frame counter from EEPROM */
	i2c_error = EEPROM_Load_Data_Frame_Counter(&eeprom_data_frame_counter);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}
	
	/* Load frame index from EEPROM */
	i2c_error = EEPROM_Load_Data_Frame_Index(&eeprom_data_frame_index);
	if(i2c_error == I2C_ERROR){
		return i2c_error;
	}

	
	if(eeprom_data_frame_counter > 0){
		
		/* Read backup data frame in EEPROM */
		pos = EEPROM_FIRST_DATA_FRAME_POSITION + ((uint16_t)eeprom_data_frame_index * SYSTEM_DATA_FRAME_MAX_SIZE_IN_BYTES);
		i2c_error = EEPROM_Load_Data_Frame(data_frame, pos);
		if(i2c_error == I2C_ERROR){
			return i2c_error;
		}	
		
		/* Update data frame counter */
		eeprom_data_frame_counter--;
		/* Update data frame index */
		if(eeprom_data_frame_index == 0){
			eeprom_data_frame_index = EEPROM_N_BACKUP_FRAMES - 1;
		}else{
			eeprom_data_frame_index--;
		}
		
		/* Save the new value of frame index */
		i2c_error = EEPROM_Save_Data_Frame_Index(&eeprom_data_frame_index);
		if(i2c_error == I2C_ERROR){
			return i2c_error;
		}
	
		/* Save the new value of frame counter */
		i2c_error = EEPROM_Save_Data_Frame_Counter(&eeprom_data_frame_counter);
		if(i2c_error == I2C_ERROR){
			return i2c_error;
		}		
		
	}
	
	return i2c_error;
}


