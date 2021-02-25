/*
 * MT_rtc_pcf8523.h
 *
 *  Created on: Jan 21, 2020
 *      Author: mtorres
 */

#ifndef MT_RTC_PCF8523_H_
#define MT_RTC_PCF8523_H_

/* Inclusión de archivos */
#include <stdint.h>

/* Dirección del dispositivo */
#define PCF8523_SLAVE_ADDR		0x68

/* Direcciones de los registros */
#define CONTROL_1_REG			0x00
#define CONTROL_2_REG			0x01
#define CONTROL_3_REG			0x02
#define SECONDS_REG				0x03
#define MINUTES_REG				0x04
#define HOURS_REG				0x05
#define DAYS_REG				0x06
#define WEEKDAYS_REG			0x07
#define MONTHS_REG				0x08
#define YEARS_REG				0x09
#define MINUTE_ALARM_REG		0x0A
#define HOUR_ALARM_REG			0x0B
#define DAY_ALARM_REG			0x0C
#define WEEKDAY_ALARM_REG		0x0D
#define OFFSET_REG				0x0E
#define TMR_CLKOUT_CTRL_REG		0x0F
#define TMR_A_FREQ_CTRL_REG		0x10
#define TMR_A_REG				0x11
#define TMR_B_FREQ_CTRL_REG		0x12
#define TMR_B_REG				0x13

/* Bits importantes */
#define BIT_CAP_SEL				7
#define BIT_T					6
#define BIT_STOP				5
#define BIT_SR					4
#define BIT_12_24				3
#define BIT_SIE					2
#define BIT_AIE					1
#define BIT_CIE					0

#define BIT_WTAF				7
#define BIT_CTAF				6
#define BIT_CTBF				5
#define BIT_SF					4
#define BIT_AF					3
#define BIT_WTAIE				2
#define BIT_CTAIE				1
#define BIT_CTBIE				0

#define BIT_PM2					7
#define BIT_PM1					6
#define BIT_PM0					5
#define BIT_BSF					3
#define BIT_BLF					2
#define BIT_BSIE				1
#define BIT_BLIE				0

#define BIT_TAM					7
#define BIT_TBM					6
#define BIT_COF2				5
#define BIT_COF1				4
#define BIT_COF0				3
#define BIT_TAC1				2
#define BIT_TAC0				1
#define BIT_TBC					0

#define BIT_AEN_M				7
#define BIT_AEN_H				7
#define BIT_AEN_D				7
#define BIT_AEN_W				7

	


/* Comando reset */
//#define RESET_CMD

/* Posiciones en el buffer de fecha */
#define DAY						0
#define MONTH					1
#define YEAR					2

/* Posiciones en el buffer de hora */
#define HOUR					0
#define MINUTE					1
#define SECOND					2
#define MERIDIEM				3


/* Posiciones en el buffer de hora de alarma */
#define ALARM_HOUR				0
#define ALARM_MINUTE			1
#define ALARM_MERIDIEM			2


/* Definición de tipos */

typedef enum{
	AM = 0,
	PM
}meridiem_t;


typedef enum{
	SRC_4096_HZ = 0,
	SRC_64_HZ = 1,
	SRC_1_HZ = 2,
	SRC_1_HZ_DIV_60 = 3,
	SRC_1_HZ_DIV_3600 = 4
}rtc_timer_clk_src_t;


typedef enum{
	CLKOUT_32768_HZ = 0,
	CLKOUT_16384_HZ,
	CLKOUT_8192_HZ,
	CLKOUT_4096_HZ,
	CLKOUT_1024_HZ,
	CLKOUT_32_HZ,
	CLKOUT_1_HZ,
	CLKOUT_DISABLE
}rtc_clockout_src_t;


/* Declaración de funciones (Prototipos) */
uint8_t	 RTC_Set_Mode_24h(void);
uint8_t  RTC_Set_Mode_12h(void);
uint8_t  RTC_Time_Set_12h(uint8_t hh, uint8_t mm, uint8_t ss, meridiem_t mer);
uint8_t  RTC_Time_Set_24h(uint8_t hh, uint8_t mm, uint8_t ss);
uint8_t  RTC_Time_Read_12h(void);
uint8_t  RTC_Time_Read_24h(void);
uint8_t  RTC_Date_Set(uint8_t dd, uint8_t mm, uint8_t yy);
uint8_t RTC_Date_Read(void);

void RTC_Get_Time_12h(uint8_t *time_buff);
void RTC_Get_Time_24h(uint8_t *time_buff);
void RTC_Get_Date(uint8_t *date_buff);

uint8_t  RTC_Alarm_Time_Set_12h(uint8_t hh, uint8_t mm, meridiem_t mer);
uint8_t  RTC_Alarm_Time_Set_24h(uint8_t hh, uint8_t mm);
uint8_t  RTC_Alarm_Time_Read_12h(void);
uint8_t  RTC_Alarm_Time_Read_24h(void);
uint8_t  RTC_Alarm_Day_Set(uint8_t dd);
uint8_t RTC_Alarm_Day_Read(void);

void RTC_Get_Alarm_Time_12h(uint8_t *time_buff);
void RTC_Get_Alarm_Time_24h(uint8_t *time_buff);
uint8_t RTC_Get_Alarm_Day(void);

uint8_t RTC_TimerA_Load_Value(uint8_t val);
uint8_t RTC_TimerB_Load_Value(uint8_t val);
uint8_t RTC_TimerA_Enable(void);
uint8_t RTC_TimerA_Disable(void);
uint8_t RTC_TimerB_Enable(void);
uint8_t RTC_TimerB_Disable(void);

uint8_t RTC_Alarm_Int_Enable(void);
uint8_t RTC_Alarm_Int_Disable(void);
uint8_t RTC_Alarm_Int_Flag_Clear(void);

uint8_t RTC_Time_Alarm_Enable(void);
uint8_t RTC_Time_Alarm_Disable(void);
uint8_t RTC_Day_Alarm_Enable(void);
uint8_t RTC_Day_Alarm_Disable(void);

uint8_t RTC_TimerA_Int_Enable(void);
uint8_t RTC_TimerA_Int_Disable(void);
uint8_t RTC_TimerA_Int_Flag_Clear(void);
uint8_t RTC_TimerB_Int_Enable(void);
uint8_t RTC_TimerB_Int_Disable(void);
uint8_t RTC_TimerB_Int_Flag_Clear(void);

//--------------------------------------------------------

uint8_t RTC_Clockout_Source_Select(rtc_clockout_src_t clk_src);
uint8_t RTC_TimerA_Clock_Source_Select(rtc_timer_clk_src_t clk_src);
uint8_t RTC_TimerB_Clock_Source_Select(rtc_timer_clk_src_t clk_src);


#endif /* MT_RTC_PCF8523_H_ */
