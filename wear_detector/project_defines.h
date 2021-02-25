

#ifndef PROJECT_DEFINES_H_
#define PROJECT_DEFINES_H_

/* Clock frequency definition */
#define F_CPU	4000000UL

/**********************************/
/********* Pin definition *********/
/**********************************/

// Boost power supply (for the Xbee and the Bluetooth module)
 
#define DDR_BOOST_PSU_ON	DDRB
#define PORT_BOOST_PSU_ON	PORTB
#define BOOST_PSU_ON		1

// External interrupt sources (digital signals)

#define DDR_RTC_EXT_INT		DDRD
#define PIN_RTC_EXT_INT		PIND
#define PORT_RTC_EXT_INT	PORTD
#define RTC_EXT_INT			3

#define DDR_TEST_XBEE		DDRD
#define PIN_TEST_XBEE		PIND
#define PORT_TEST_XBEE		PORTD
#define TEST_XBEE			2

#define DDR_TEST_BT			DDRD	
#define PIN_TEST_BT			PIND
#define PORT_TEST_BT		PORTD
#define TEST_BT				4

// Servomotor pins
 
#define DDR_SERVO_ON		DDRD
#define PORT_SERVO_ON		PORTD
#define SERVO_ON			7

#define DDR_SERVO_PWM		DDRB
#define PORT_SERVO_PWM		PORTB
#define SERVO_PWM			2

// Bluetooth pins

#define DDR_BT_ON			DDRD
#define PORT_BT_ON			PORTD
#define BT_ON				6

#define DDR_BT_TX			DDRB
#define PORT_BT_TX			PORTB
#define BT_TX				3

#define DDR_BT_RX			DDRB
#define PORT_BT_RX			PORTB
#define BT_RX				4

#define DDR_BT_KEY			DDRB
#define PORT_BT_KEY			PORTB
#define BT_KEY				0

// Xbee pins

#define DDR_XBEE_ON			DDRD
#define PORT_XBEE_ON		PORTD
#define XBEE_ON				5

#define DDR_XBEE_TX			DDRD
#define PORT_XBEE_TX		PORTD
#define XBEE_TX				0

#define DDR_XBEE_RX			DDRD
#define PORT_XBEE_RX		PORTD
#define XBEE_RX				1

// I2C Bus pins

#define DDR_I2C_SCL			DDRC			
#define PORT_I2C_SCL		PORTC
#define I2C_SCL				5

#define DDR_I2C_SDA			DDRC
#define PORT_I2C_SDA		PORTC
#define I2C_SDA				4


// Analog resistor sensors pins

#define DDR_ANALOG_MEASURE_ON		DDRC
#define PORT_ANALOG_MEASURE_ON		PORTC
#define ANALOG_MEASURE_ON			1	

#define DDR_RES1					DDRC
#define PIN_RES1					PINC
#define PORT_RES1					PORTC
#define RES1						0

#define DDR_RES2					DDRC
#define PIN_RES2					PINC
#define PORT_RES2					PORTC
#define RES2						2

// Battery level pin

#define DDR_BATERY_LEVEL			DDRE
#define PIN_BATTERY_LEVEL			PINE
#define PORT_BATTERY_LEVEL			PORTE			
#define BATTERY_LEVEL				2

// AUX_PIN_FOR_DEBUG
#define DDR_AUX						DDRE
#define PIN_AUX						PINE
#define PORT_AUX					PORTE
#define AUX							3

/**********************************/
/****** CONSTANTS AND MACROS ******/
/**********************************/

// ----------------------- Defines for analog channels -------------------------

#define RES1_SENSOR_ADC_CHANNEL			0
#define RES2_SENSOR_ADC_CHANNEL			2
#define BATTERY_LEVEL_ADC_CHANNEL		6

// ----------------------- Defines for analog measures -------------------------

#define N_ANALOG_INPUTS					3
#define N_SAMPLES_PER_CHANNEL			32

#define RES1_MEASURE_INDEX				0
#define RES2_MEASURE_INDEX				1
#define BATTERY_MEASURE_INDEX			2

#define RES1_SENSE_THRESHOLD_OHM		300.0
#define RES1_SENSE_MAX_VALUE_OHM		1000.0
#define RES1_SENSE_MIN_VALUE_OHM		0.0
#define RES1_MEASURE_GAIN				1.0

#define RES2_SENSE_THRESHOLD_OHM		300.0
#define RES2_SENSE_MAX_VALUE_OHM		1000.0
#define RES2_SENSE_MIN_VALUE_OHM		0.0
#define RES2_MEASURE_GAIN				1.0

#define BATT_SENSE_MAX_VALUE_VOLT		4.6
#define BATT_SENSE_MIN_VALUE_VOLT		3.0
#define BATT_CIRCUIT_GAIN				0.20408

// ------------------------- Defines for frame construction ---------------------------

#define SYSTEM_DATA_FRAME_MAX_SIZE_IN_BYTES			400

#define SERVO_INDICATOR_STATE_SIZE_IN_BYTES			1
#define NUMBER_OF_SENSORS_SIZE_IN_BYTES				1
#define MAIN_COMMUNICATION_MEDIA_SIZE_IN_BYTES		1
#define MEASURE_PERIOD_SIZE_IN_BYTES				1				
#define PART_NUMBER_SIZE_IN_BYTES					11//13					
#define EMITTER_NODE_ID_SIZE_IN_BYTES				10//11
#define RECEIVER_CENTRAL_ID_SIZE_IN_BYTES			11
#define DATA_FRAME_DATE_SIZE_IN_BYTES				13
#define DATA_FRAME_TIME_SIZE_IN_BYTES				14
#define WEAR_OUT_STATE_SIZE_IN_BYTES				4	
#define BATTERY_LEVEL_SIZE_IN_BYTES					6	
#define EMITTER_OHM1_SIZE_IN_BYTES					6
#define EMITTER_OHM2_SIZE_IN_BYTES					6
#define MEASURE_PERIOD_STR_SIZE_IN_BYTES			5

#define PART_NUMBER_HEADER_STR						"\"part number Weir\""
#define EMITTER_NODE_ID_HEADER_STR					"\"id del emisor\""
#define RECEIVER_CENTRAL_ID_HEADER_STR				"\"id del receptor(alfanumerico)\""
#define DATA_FRAME_DATE_HEADER_STR					"\"fecha de data(dd/mm/yyyy)\""
#define DATA_FRAME_TIME_HEADER_STR					"\"hora de data (hh:mm a.m./p.m.)\""
#define WEAR_OUT_STATE_HEADER_STR					"\"Estado de desgaste\""
#define BATTERY_LEVEL_HEADER_STR					"\"Nivel de Bateria\""
#define EMITTER_OHM1_HEADER_STR						"\"Ohmiaje 1 Emisor\""
#define EMITTER_OHM2_HEADER_STR						"\"Ohmiaje 2 Emisor\""
#define MEASURE_PERIOD_HEADER_STR					"\"Periodo (dias)\""


// ------------------ Defines for serial communication (Xbee and Bluetooth) ---------------------------

// Baud rate
#define	XBEE_SERIAL_BAUD_RATE						Baud_38400_bps
#define BLUETOOTH_CONFIG_MODE_BAUD_RATE				Baud_38400_bps
#define BLUETOOTH_WORKING_MODE_BAUD_RATE			Baud_38400_bps

// Time limit and number of tries
#define BLUETOOTH_TIMEOUT_PERIOD_MS					60000 // Bluetooth waits 1 minute without activity before auto shut-down
#define XBEE_RESPONSE_TIME_MS						15000 // Xbee waits 15 seconds for the central to response 
#define XBEE_COMMUNICATION_TRIES_NUMBER				3

// Buffers size
#define XBEE_DATA_FRAME_BUFF_SIZE					SYSTEM_DATA_FRAME_MAX_SIZE_IN_BYTES
#define BLUETOOTH_DATA_FRAME_BUFF_SIZE				SYSTEM_DATA_FRAME_MAX_SIZE_IN_BYTES
#define SERIAL_RX_BUFFER_SIZE						40
#define SERIAL_COMMAND_BUFFER_SIZE					40

#define XBEE_DATA_ACCEPTED_FRAME_SIZE				28
#define XBEE_DATA_REJECTED_FRAME_SIZE				29

#define BT_MEASURE_PERIOD_SET_HEADER_SIZE		12
#define BT_EMITTER_ID_SET_HEADER_SIZE			11
#define BT_RECEIVER_ID_SET_HEADER_SIZE			11
#define BT_DATE_TIME_SET_HEADER_SIZE			14
#define BT_NUM_SENSORS_SET_HEADER_SIZE			14
#define BT_SERVO_ENABLE_DISABLE_HEADER_SIZE		11


// ----------------------------------- Values by default ----------------------------------------------

#define SERVO_INDICATOR_STATE_BY_DEFAULT			SERVO_INDICATOR_ENABLED
#define NUMBER_OF_SENSORS_BY_DEFAULT				1
#define MEASURE_PERIOD_IN_DAYS_BY_DEFAULT			4
#define	DATE_DAY_BY_DEFAULT							1
#define DATE_MONTH_BY_DEFAULT						1
#define DATE_YEAR_BY_DEFAULT						20
#define TIME_HOUR_BY_DEFAULT						11
#define TIME_MINUTE_BY_DEFAULT						59
#define TIME_SECOND_BY_DEFAULT						0
#define TIME_MERIDIEN_BY_DEFAULT					AM
#define NODE_ID_NUMBER_BY_DEFAULT					1// Number from 1 to 99

#define MEASURE_PERIOD_MIN_VALUE					1
#define MEASURE_PERIOD_MAX_VALUE					10

#define SERVO_INDICATOR_DISABLED					0
#define SERVO_INDICATOR_ENABLED						1 
 
#define BATTERY_LOW_LEVEL_THRESHOLD_PERCENT			30
 
#define RES_SENSOR_HYSTERESIS_VALUE					5
#define BATTERY_HYSTERESIS_VALUE					5
#define SERVO_HYSTERESIS_VALUE						2

// ------------------------ Defines for operation modes ---------------------------

#define SYSTEM_STAND_BY_MODE						0
#define SYSTEM_PERIODIC_MEASURE_MODE				1
#define SYSTEM_XBEE_TEST_MODE						2
#define SYSTEM_BLUETOOTH_TEST_MODE					3
#define SYSTEM_OPERATION_MODE_BY_DEFAULT			SYSTEM_STAND_BY_MODE

// --------------------- Defines for main communication media ----------------------

#define MAIN_COMMUNICATION_MEDIA_XBEE				0
#define MAIN_COMMMUNICATION_MEDIA_BLUETOOTH			1
#define	MAIN_COMM_MEDIA_BY_DEFAULT					MAIN_COMMUNICATION_MEDIA_XBEE 

// --------------------- Defines for safe time when shutting down a device ----------------------

#define BOOST_PSU_SETUP_TIME_MS						1000 // Wait 1 second after turning on/off the Boost PSU
#define XBEE_POWER_SETUP_TIME_MS					500 // Wait 0.5 second after turning on/off the Xbee module
#define BLUETOOTH_POWER_SETUP_TIME_MS				500 // Wait 0.5 second after turning on/off the Bluetooth module
#define SERVO_POWER_SETUP_TIME_MS					500 // Wait 0.5 second after turning on/off the servomotor
#define ANALOG_POWER_SETUP_TIME_MS					500 // // Wait 0.5 second after enabling/disabling the analog inputs

// ------------------------ Defines for system flags ---------------------------

#define EEPROM_I2C_COMM_ERROR_FLAG					0
#define RTC_I2C_COMM_ERROR_FLAG						1
#define SERVO_POS_COMPLETE_FLAG						2
#define BT_CONN_DETECT_TIMEOUT_FLAG					4
#define BT_CONN_TIMEOUT_COUNT_ENABLE_FLAG			5
#define BT_SERIAL_RX_ENABLE_FLAG					6
#define XBEE_SERIAL_RX_ENABLE_FLAG					7
#define TIME_OUT_FLAG								8
#define XBEE_COMM_RESPONSE_FLAG						9
#define BT_COMM_RESPONSE_FLAG						10

// --------------------------------------------------------------------------------

#define NODO_EN_MODO_TEST_TIEMPO_CORTO				0
#define NODO_EN_MODO_TRABAJO_TIEMPO_NORMAL			1

// -------------------------- TRABAJAR CON ESTAS DEFINICIONES CADA VEZ QUE SE PROGRAME UN NUEVO NODO --------------------------------------

#define MODO_DE_TRABAJO_DE_NODO						NODO_EN_MODO_TRABAJO_TIEMPO_NORMAL	// Tiempo normal (días) o tiempo corto (intervalor de 24 segundos)	
#define PRIMERA_PROGRAMACION						0 // Colocar 1 la primeravez que se programa el nodo; luego, colocar 0 y volver a programar
#define PART_NUMBER_STR_BY_DEFAULT					"\"DET-0022\"" // Part number definido en fabrica (EPLIMIN). Define el nombre de la conexión Bluetooth.
#define EMITTER_NODE_ID_STR_BY_DEFAULT				"\"MANG-02\"" // ID del emisor por defecto (puede dejarse como está y cambiarlo luego por Bluetooth)
#define RECEIVER_CENTRAL_ID_STR_BY_DEFAULT			"\"REC-0003\"" // ID del receptor por defecto (puede dejarse como está y cambiarlo luego por Bluetooth)
#define BLUETOOTH_PIN_BY_DEFAULT					"611535" // PIN para vincularse por Bluetooth (no es necesario modificarlo)

// -----------------------------------------------------------------------------------------------------------------------------------

#endif