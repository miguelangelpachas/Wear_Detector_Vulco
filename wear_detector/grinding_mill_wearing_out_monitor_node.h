

#ifndef GRINDING_MILL_WEARING_OUT_MONITOR_NODE_H_
#define GRINDING_MILL_WEARING_OUT_MONITOR_NODE_H_


/* Type definition */

typedef enum{
	Battery_OK = 0,
	Battery_Low = 1
}battery_status_t;

typedef enum{
	Resistor_Open = 0,
	Resistor_Closed = 1
}resistor_status_t;
 
typedef enum{
	Wear_Out_Lower_Than_50 = 0,
	Wear_Out_Between_50_And_75 = 1,
	Wear_Out_Higher_Than_75 = 2
}sensor_status_t;


typedef struct{
	
	char *part_number_str_ptr;
	char *emitter_node_id_str_ptr;
	char *receiver_central_id_str_ptr;
	char *period_str_ptr;
	char *date_str_ptr;
	char *time_str_ptr;
	char *sensor_status_str_ptr;
	char *battery_str_ptr;	
	char *ohm1_str_ptr;
	char *ohm2_str_ptr;
	
}data_frame_t;  


/* Function declaration */

void Ext_Int_Pin_Initialize(void);

void Timer0_Initialize(void);
void Timer0_On(void);
void Timer0_Off(void);

void Boost_PSU_Initialize(void);
void Boost_PSU_Power_On(void);
void Boost_PSU_Power_Off(void);

void System_Initialize(void);
uint8_t System_Date_Update(void);
uint8_t System_Time_Update(void); 

uint8_t Prepare_Data_For_Frame(void);
uint8_t Data_Frame_Build(void);

void Analog_Measure_Task(void);
uint8_t Resistor_Sensor_Status_Calculation(void);
void One_Res_Sensor_Status_Calculation(void);
void Two_Res_Sensor_Status_Calculation(void);
void Battery_Status_Calculation(void);
void Servo_Status_Indicator_Task(void);
void Bluetooth_Communication_Task(void);
void Xbee_Communication_Task(void);

uint8_t Get_Node_ID_Number(void);
uint8_t Xbee_Test_Mode_Cycle(void);
uint8_t Bluetooth_Test_Mode_Cycle(void);
uint8_t Xbee_Periodic_Measure_Mode_Cycle(void);
uint8_t Bluetooth_Measure_And_Get_Data(void);

uint8_t EEPROM_First_Time_Configuration(void);
uint8_t RTC_Boot_Time_Configuration(void);
uint8_t System_First_Time_Configuration(void);
uint8_t RTC_Configure_TimerA_Interrupt(void); 

void Microcontroller_Peripherals_Disable(void);
void Microcontroller_Peripherals_Enable(void); 
void WDT_Off(void);
 
#endif