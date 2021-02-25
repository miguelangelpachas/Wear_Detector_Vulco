/* Inclusión de archivos */

#include <stdint.h>
#include "MT_twi_master.h"
#include "MT_rtc_pcf8523.h"
#include "project_defines.h"
#include <util/delay.h>

/* Variables globales */

uint8_t rtc_time_buffer[4];
uint8_t rtc_date_buffer[3];

uint8_t rtc_alarm_time_buffer[3];
uint8_t rtc_alarm_day;

static uint8_t i2c_tx_buffer[30];
static uint8_t i2c_rx_buffer[30];


/* Definición de funciones */


uint8_t RTC_Set_Mode_24h(void){

	uint8_t salida = I2C_OK;
	uint8_t temp = 0;

	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = CONTROL_1_REG;
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 1, i2c_tx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}
	/* Leer el valor actual del registro */
	salida = TWI0_Master_Read_Data(PCF8523_SLAVE_ADDR, 1, &temp);
	if(salida == I2C_ERROR){
		return salida;
	}
	
	/* Forzar a '0' el bit 12_24 */
	temp &= ~(1 << BIT_12_24);
	
	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = CONTROL_1_REG;
	/* Cargar el valor actualizado */
	i2c_tx_buffer[1] = temp;
	/* Transmitir los 2 bytes al RTC */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 2, i2c_tx_buffer);
	
	return salida;
	
}


uint8_t RTC_Set_Mode_12h(void){
	
	uint8_t salida = I2C_OK;
	uint8_t temp = 0;

	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = CONTROL_1_REG;
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 1, i2c_tx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}
	/* Leer el valor actual del registro */
	salida = TWI0_Master_Read_Data(PCF8523_SLAVE_ADDR, 1, &temp);
	if(salida == I2C_ERROR){
		return salida;
	}
	
	/* Forzar a '0' el bit 12_24 */
	temp |= (1 << BIT_12_24);
	
	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = CONTROL_1_REG;
	/* Cargar el valor actualizado */
	i2c_tx_buffer[1] = temp;
	/* Transmitir los 2 bytes al RTC */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 2, i2c_tx_buffer);
	
	return salida;
}


uint8_t  RTC_Time_Set_12h(uint8_t hh, uint8_t mm, uint8_t ss, meridiem_t mer){

	uint8_t hh_d, hh_u, mm_d, mm_u, ss_d, ss_u;
	uint8_t  salida = I2C_OK;

	/* Extraer los dígitos de las horas, minutos y segundos */
	hh_u = hh % 10;
	hh_d = (hh / 10) % 10;

	mm_u = mm % 10;
	mm_d = (mm / 10) % 10;

	ss_u = ss % 10;
	ss_d = (ss / 10) % 10;

	/* Cargar dirección del registro de segundos en el buffer de transmisión */
	i2c_tx_buffer[0] = SECONDS_REG;
	/* Cargar los dígitos y meridiano al buffer de transmisión */
	i2c_tx_buffer[1] = (ss_d << 4) | ss_u;
	i2c_tx_buffer[2] = (mm_d << 4) | mm_u;
	i2c_tx_buffer[3] = (mer << 5) | ((hh_d & 1) << 4) | hh_u;

	/* Transmitir los 4 bytes al RTC */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 4, i2c_tx_buffer);

	return salida;
}


uint8_t  RTC_Time_Set_24h(uint8_t hh, uint8_t mm, uint8_t ss){

	uint8_t hh_d, hh_u, mm_d, mm_u, ss_d, ss_u;
	uint8_t  salida = I2C_OK;

	/* Extraer los dígitos de las horas, minutos y segundos */
	hh_u = hh % 10;
	hh_d = (hh / 10) % 10;

	mm_u = mm % 10;
	mm_d = (mm / 10) % 10;

	ss_u = ss % 10;
	ss_d = (ss / 10) % 10;

	/* Cargar dirección del registro de segundos en el buffer de transmisión */
	i2c_tx_buffer[0] = SECONDS_REG;
	/* Cargar los dígitos al buffer de transmisión */
	i2c_tx_buffer[1] = (ss_d << 4) | ss_u;
	i2c_tx_buffer[2] = (mm_d << 4) | mm_u;
	i2c_tx_buffer[3] = (hh_d << 4) | hh_u;

	/* Transmitir los 4 bytes al RTC */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 4, i2c_tx_buffer);
	return salida;

}


uint8_t  RTC_Time_Read_12h(void){

	uint8_t hh_d, hh_u, mm_d, mm_u, ss_d, ss_u;
	uint8_t salida = I2C_OK;

	/* Cargar dirección del registro de segundos en el buffer de transmisión */
	i2c_tx_buffer[0] = SECONDS_REG;
	/* Escribir la dirección del primer registro a leer (SEGUNDOS) */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 1, i2c_tx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}		
	
	/* Leer los registros */
	salida = TWI0_Master_Read_Data(PCF8523_SLAVE_ADDR, 3, i2c_rx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}
	
	/* Cargar los valores de hora, minuto, segundo y meridiano */
	ss_u = i2c_rx_buffer[0] & 0x0F;
	ss_d = (i2c_rx_buffer[0] >> 4) & 0x07;
	rtc_time_buffer[SECOND] = (10 * ss_d) + ss_u;

	mm_u = i2c_rx_buffer[1] & 0x0F;
	mm_d = (i2c_rx_buffer[1] >> 4) & 0x0F;
	rtc_time_buffer[MINUTE] = (10 * mm_d) + mm_u;

	hh_u = i2c_rx_buffer[2] & 0x0F;
	hh_d = (i2c_rx_buffer[2] >> 4) & 1;
	rtc_time_buffer[HOUR] = (10 * hh_d) + hh_u;

	rtc_time_buffer[MERIDIEM] = (i2c_rx_buffer[2] >> 5) & 1;

	return salida;
}



uint8_t RTC_Time_Read_24h(void){

	uint8_t hh_d, hh_u, mm_d, mm_u, ss_d, ss_u;
	uint8_t salida = I2C_OK;

	/* Cargar dirección del registro de segundos en el buffer de transmisión */
	i2c_tx_buffer[0] = SECONDS_REG;
	/* Escribir la dirección del primer registro a leer (SEGUNDOS) */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 1, i2c_tx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}	
	
	/* Leer los registros */
	salida = TWI0_Master_Read_Data(PCF8523_SLAVE_ADDR, 3, i2c_rx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}
	
	/* Cargar los valores de hora, minuto, segundo y meridiano */
	ss_u = i2c_rx_buffer[0] & 0x0F;
	ss_d = (i2c_rx_buffer[0] >> 4) & 0x07;
	rtc_time_buffer[SECOND] = (10 * ss_d) + ss_u;

	mm_u = i2c_rx_buffer[1] & 0x0F;
	mm_d = (i2c_rx_buffer[1] >> 4) & 0x0F;
	rtc_time_buffer[MINUTE] = (10 * mm_d) + mm_u;

	hh_u = i2c_rx_buffer[2] & 0x0F;
	hh_d = (i2c_rx_buffer[2] >> 4) & 3;
	rtc_time_buffer[HOUR] = (10 * hh_d) + hh_u;

	return salida;
}



uint8_t RTC_Date_Set(uint8_t dd, uint8_t mm, uint8_t yy){

	uint8_t salida = I2C_OK;

	/* Extraer los dígitos del día, mes y año */

	uint8_t yy_d, yy_u, mm_d, mm_u, dd_d, dd_u;

	yy_u = yy % 10;
	yy_d = (yy / 10) % 10;

	mm_u = mm % 10;
	mm_d = ((mm / 10) % 10);

	dd_u = dd % 10;
	dd_d = ((dd / 10) % 10) & 0x03;

	/* Cargar dirección del registro de días en el buffer de transmisión */
	i2c_tx_buffer[0] = DAYS_REG;
	/* Cargar los dígitos al buffer de transmisión */
	i2c_tx_buffer[1] = (dd_d << 4) | dd_u;
	/* Transmitir los 2 bytes al RTC */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 2, i2c_tx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}

	/* Cargar dirección del registro de meses en el buffer de transmisión */
	i2c_tx_buffer[0] = MONTHS_REG;
	/* Cargar los dígitos al buffer de transmisión */
	i2c_tx_buffer[1] = (mm_d << 4) | mm_u;
	i2c_tx_buffer[2] = (yy_d << 4) | yy_u;
	/* Transmitir los 3 bytes al RTC */
	
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 3, i2c_tx_buffer);
		
	return salida;
}


uint8_t RTC_Date_Read(void){

	uint8_t salida = 0;
	uint8_t yy_d, yy_u, mm_d, mm_u, dd_d, dd_u;

	/* Cargar dirección del registro de segundos en el buffer de transmisión */
	i2c_tx_buffer[0] = DAYS_REG;
	/* Escribir la dirección del primer registro a leer (DIAS) */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 1, i2c_tx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}

	/* Leer el registro de DIAS */
	salida = TWI0_Master_Read_Data(PCF8523_SLAVE_ADDR, 1, i2c_rx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}	
	
	/* Cargar dirección del registro de segundos en el buffer de transmisión */
	i2c_tx_buffer[0] = MONTHS_REG;
	/* Escribir la dirección del siguiente registro a leer (MESES) */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 1, i2c_tx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}	
	
	/* Leer el registro de MESES y el registro AÑOS */
	salida = TWI0_Master_Read_Data(PCF8523_SLAVE_ADDR, 2, i2c_rx_buffer + 1);

	/* Cargar los valores de día , mes y año*/
	dd_u = i2c_rx_buffer[0] & 0x0F;
	dd_d = (i2c_rx_buffer[0] >> 4) & 0x03;
	rtc_date_buffer[DAY] = (10 * dd_d) + dd_u;

	mm_u = i2c_rx_buffer[1] & 0x0F;
	mm_d = (i2c_rx_buffer[1] >> 4) & 1;
	rtc_date_buffer[MONTH] = (10 * mm_d) + mm_u;

	yy_u = i2c_rx_buffer[2] & 0x0F;
	yy_d = (i2c_rx_buffer[2] >> 4) & 0x0F;
	rtc_date_buffer[YEAR] = (10 * yy_d) + yy_u;

	return salida;
}


void RTC_Get_Time_12h(uint8_t *time_buff){		
	*(time_buff++) = rtc_time_buffer[0];
	*(time_buff++) = rtc_time_buffer[1];
	*(time_buff++) = rtc_time_buffer[2];
	*(time_buff++) = rtc_time_buffer[3];
}


void RTC_Get_Time_24h(uint8_t *time_buff){
	*(time_buff++) = rtc_time_buffer[0];
	*(time_buff++) = rtc_time_buffer[1];
	*(time_buff++) = rtc_time_buffer[2];	
}


void RTC_Get_Date(uint8_t *date_buff){
	*(date_buff++) = rtc_date_buffer[0];
	*(date_buff++) = rtc_date_buffer[1];
	*(date_buff++) = rtc_date_buffer[2];	
}



/**************************** Functions for alarms ***********************************/

uint8_t  RTC_Alarm_Time_Set_12h(uint8_t hh, uint8_t mm, meridiem_t mer){

	uint8_t hh_d, hh_u, mm_d, mm_u;
	uint8_t  salida = I2C_OK;

	/* Extraer los dígitos de las horas, minutos y segundos */
	hh_u = hh % 10;
	hh_d = (hh / 10) % 10;

	mm_u = mm % 10;
	mm_d = (mm / 10) % 10;

	/* Cargar dirección del registro de segundos en el buffer de transmisión */
	i2c_tx_buffer[0] = MINUTE_ALARM_REG;
	/* Cargar los dígitos y meridiano al buffer de transmisión */
	i2c_tx_buffer[1] = (mm_d << 4) | mm_u;
	i2c_tx_buffer[2] = (mer << 5) | ((hh_d & 1) << 4) | hh_u;
	/* Forzar a 1 los bits de habilitacion de la alarma*/
	i2c_tx_buffer[1] |= (1 << BIT_AEN_M);
	i2c_tx_buffer[2] |= (1 << BIT_AEN_H);

	/* Transmitir los 4 bytes al RTC */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 3, i2c_tx_buffer);

	return salida;
}


uint8_t  RTC_Alarm_Time_Set_24h(uint8_t hh, uint8_t mm){

	uint8_t hh_d, hh_u, mm_d, mm_u;
	uint8_t  salida = I2C_OK;

	/* Extraer los dígitos de las horas, minutos y segundos */
	hh_u = hh % 10;
	hh_d = (hh / 10) % 10;

	mm_u = mm % 10;
	mm_d = (mm / 10) % 10;

	/* Cargar dirección del registro de segundos en el buffer de transmisión */
	i2c_tx_buffer[0] = MINUTE_ALARM_REG;
	/* Cargar los dígitos al buffer de transmisión */
	i2c_tx_buffer[1] = (mm_d << 4) | mm_u;
	i2c_tx_buffer[2] = (hh_d << 4) | hh_u;
	/* Forzar a 1 los bits de habilitacion de la alarma*/
	i2c_tx_buffer[1] |= (1 << BIT_AEN_M);
	i2c_tx_buffer[2] |= (1 << BIT_AEN_H);	

	/* Transmitir los 4 bytes al RTC */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 3, i2c_tx_buffer);
	return salida;

}


uint8_t  RTC_Alarm_Time_Read_12h(void){

	uint8_t hh_d, hh_u, mm_d, mm_u;
	uint8_t salida = I2C_OK;

	/* Cargar dirección del registro de segundos en el buffer de transmisión */
	i2c_tx_buffer[0] = MINUTE_ALARM_REG;
	/* Escribir la dirección del primer registro a leer (SEGUNDOS) */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 1, i2c_tx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}
	
	/* Leer los registros */
	salida = TWI0_Master_Read_Data(PCF8523_SLAVE_ADDR, 2, i2c_rx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}
	
	/* Cargar los valores de hora, minuto y meridiano */

	mm_u = i2c_rx_buffer[0] & 0x0F;
	mm_d = (i2c_rx_buffer[0] >> 4) & 0x07;
	rtc_alarm_time_buffer[ALARM_MINUTE] = (10 * mm_d) + mm_u;

	hh_u = i2c_rx_buffer[1] & 0x0F;
	hh_d = (i2c_rx_buffer[1] >> 4) & 1;
	rtc_alarm_time_buffer[ALARM_HOUR] = (10 * hh_d) + hh_u;

	rtc_alarm_time_buffer[ALARM_MERIDIEM] = (i2c_rx_buffer[1] >> 5) & 1;

	return salida;
}



uint8_t RTC_Alarm_Time_Read_24h(void){

	uint8_t hh_d, hh_u, mm_d, mm_u;
	uint8_t salida = I2C_OK;

	/* Cargar dirección del registro de segundos en el buffer de transmisión */
	i2c_tx_buffer[0] = MINUTE_ALARM_REG;
	/* Escribir la dirección del primer registro a leer (SEGUNDOS) */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 1, i2c_tx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}
	
	/* Leer los registros */
	salida = TWI0_Master_Read_Data(PCF8523_SLAVE_ADDR, 2, i2c_rx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}
	
	/* Cargar los valores de hora, minuto, segundo y meridiano */

	mm_u = i2c_rx_buffer[0] & 0x0F;
	mm_d = (i2c_rx_buffer[0] >> 4) & 0x07;
	rtc_time_buffer[ALARM_MINUTE] = (10 * mm_d) + mm_u;

	hh_u = i2c_rx_buffer[1] & 0x0F;
	hh_d = (i2c_rx_buffer[1] >> 4) & 3;
	rtc_time_buffer[ALARM_HOUR] = (10 * hh_d) + hh_u;

	return salida;
}


uint8_t RTC_Alarm_Day_Set(uint8_t dd){

	uint8_t salida = I2C_OK;

	/* Extraer los dígitos del día */

	uint8_t dd_d, dd_u;

	dd_u = dd % 10;
	dd_d = ((dd / 10) % 10) & 0x03;

	/* Cargar dirección del registro de días en el buffer de transmisión */
	i2c_tx_buffer[0] = DAYS_REG;
	/* Cargar los dígitos al buffer de transmisión */
	i2c_tx_buffer[1] = (dd_d << 4) | dd_u;
	/* Forzar a 1 la bandera que habilita la alarma */
	 i2c_tx_buffer[1] |= (1 << BIT_AEN_D); 
	/* Transmitir los 2 bytes al RTC */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 2, i2c_tx_buffer);
		
	return salida;
}


uint8_t RTC_Alarm_Day_Read(void){

	uint8_t salida = 0;
	uint8_t dd_d, dd_u;

	/* Cargar dirección del registro de segundos en el buffer de transmisión */
	i2c_tx_buffer[0] = DAYS_REG;
	/* Escribir la dirección del primer registro a leer (DIAS) */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 1, i2c_tx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}

	/* Leer el registro de DIAS */
	salida = TWI0_Master_Read_Data(PCF8523_SLAVE_ADDR, 1, i2c_rx_buffer);

	if(salida == I2C_ERROR){
		return salida;
	}

	/* Cargar los valores de día , mes y año*/
	dd_u = i2c_rx_buffer[0] & 0x0F;
	dd_d = (i2c_rx_buffer[0] >> 4) & 0x03;
	rtc_alarm_day = (10 * dd_d) + dd_u;

	return salida;
}


void RTC_Alarm_Get_Time_12h(uint8_t *time_buff){
	*(time_buff++) = rtc_alarm_time_buffer[0];
	*(time_buff++) = rtc_alarm_time_buffer[1];
	*(time_buff++) = rtc_alarm_time_buffer[2];
}


void RTC_Alarm_Get_Time_24h(uint8_t *time_buff){
	*(time_buff++) = rtc_time_buffer[0];
	*(time_buff++) = rtc_time_buffer[1];
}


uint8_t RTC_Get_Day(uint8_t *date_buff){
	return rtc_alarm_day;
}


uint8_t RTC_TimerA_Load_Value(uint8_t val){

	uint8_t salida = I2C_OK;

	/* Cargar dirección del registro TimerA */
	i2c_tx_buffer[0] = TMR_A_REG;
	/* Cargar el valor deseado */
	i2c_tx_buffer[1] = val;
	/* Transmitir los 2 bytes al RTC */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 2, i2c_tx_buffer);
	
	return salida;
	
}



uint8_t RTC_TimerB_Load_Value(uint8_t val){

	uint8_t salida = I2C_OK;

	/* Cargar dirección del registro TimerA */
	i2c_tx_buffer[0] = TMR_B_REG;
	/* Cargar el valor deseado */
	i2c_tx_buffer[1] = val;
	/* Transmitir los 2 bytes al RTC */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 2, i2c_tx_buffer);
	
	return salida;

}


uint8_t RTC_TimerA_Enable(void){

	uint8_t salida = I2C_OK;
	uint8_t temp = 0;

	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = TMR_CLKOUT_CTRL_REG;
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 1, i2c_tx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}
	/* Leer el valor actual del registro */
	salida = TWI0_Master_Read_Data(PCF8523_SLAVE_ADDR, 1, &temp);
	if(salida == I2C_ERROR){
		return salida;
	}
	
	/* Habilitar TimerA en modo countdown */
	temp &= ~(1 << BIT_TAC1);
	temp |= (1 << BIT_TAC0);
	/* Que la señal de interrupcion sea un pulso */
	temp |= (1 << BIT_TAM);
	
	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = TMR_CLKOUT_CTRL_REG;
	/* Cargar el valor actualizado */
	i2c_tx_buffer[1] = temp;
	/* Transmitir los 2 bytes al RTC */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 2, i2c_tx_buffer);
	
	return salida;
}

uint8_t RTC_TimerA_Disable(void){

	uint8_t salida = I2C_OK;
	uint8_t temp = 0;

	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = TMR_CLKOUT_CTRL_REG;
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 1, i2c_tx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}
	/* Leer el valor actual del registro */
	salida = TWI0_Master_Read_Data(PCF8523_SLAVE_ADDR, 1, &temp);
	if(salida == I2C_ERROR){
		return salida;
	}
	
	/* Forzar a '0' los bits de habilitar TimerA */
	temp &= ~(1 << BIT_TAC1);
	temp &= ~(1 << BIT_TAC0);
	
	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = TMR_CLKOUT_CTRL_REG;
	/* Cargar el valor actualizado */
	i2c_tx_buffer[1] = temp;
	/* Transmitir los 2 bytes al RTC */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 2, i2c_tx_buffer);
	
	return salida;	
	
}


uint8_t RTC_TimerB_Enable(void){

	uint8_t salida = I2C_OK;
	uint8_t temp = 0;

	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = TMR_CLKOUT_CTRL_REG;
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 1, i2c_tx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}
	/* Leer el valor actual del registro */
	salida = TWI0_Master_Read_Data(PCF8523_SLAVE_ADDR, 1, &temp);
	if(salida == I2C_ERROR){
		return salida;
	}
	
	/* Forzar a '1' el bit de habilitar TimerB */
	temp |= (1 << BIT_TBC);
	/* Que la señal de interrupcion sea un pulso */
	temp |= (1 << BIT_TBM);
	
	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = TMR_CLKOUT_CTRL_REG;
	/* Cargar el valor actualizado */
	i2c_tx_buffer[1] = temp;
	/* Transmitir los 2 bytes al RTC */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 2, i2c_tx_buffer);
	
	return salida;
	
}


uint8_t RTC_TimerB_Disable(void){
	
	uint8_t salida = I2C_OK;
	uint8_t temp = 0;

	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = TMR_CLKOUT_CTRL_REG;
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 1, i2c_tx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}
	/* Leer el valor actual del registro */
	salida = TWI0_Master_Read_Data(PCF8523_SLAVE_ADDR, 1, &temp);
	if(salida == I2C_ERROR){
		return salida;
	}
	
	/* Forzar a '0' el bit de habilitar TimerB */
	temp &= ~(1 << BIT_TBC);
	
	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = TMR_CLKOUT_CTRL_REG;
	/* Cargar el valor actualizado */
	i2c_tx_buffer[1] = temp;
	/* Transmitir los 2 bytes al RTC */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 2, i2c_tx_buffer);
	
	return salida;	
}


uint8_t RTC_Alarm_Int_Enable(void){

	uint8_t salida = I2C_OK;
	uint8_t temp = 0;

	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = CONTROL_1_REG;
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 1, i2c_tx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}
	/* Leer el valor actual del registro */
	salida = TWI0_Master_Read_Data(PCF8523_SLAVE_ADDR, 1, &temp);
	if(salida == I2C_ERROR){
		return salida;
	}
	
	/* Forzar a '1' el bit de habilitar la interrupción por alarma */
	temp |= (1 << BIT_AIE);
	
	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = CONTROL_1_REG;
	/* Cargar el valor actualizado */
	i2c_tx_buffer[1] = temp;
	/* Transmitir los 2 bytes al RTC */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 2, i2c_tx_buffer);
	
	return salida;	
}


uint8_t RTC_Alarm_Int_Disable(void){

	uint8_t salida = I2C_OK;
	uint8_t temp = 0;

	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = CONTROL_1_REG;
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 1, i2c_tx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}
	/* Leer el valor actual del registro */
	salida = TWI0_Master_Read_Data(PCF8523_SLAVE_ADDR, 1, &temp);
	if(salida == I2C_ERROR){
		return salida;
	}
	
	/* Forzar a '1' el bit de habilitar la interrupción por alarma */
	temp &= ~(1 << BIT_AIE);
	
	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = CONTROL_1_REG;
	/* Cargar el valor actualizado */
	i2c_tx_buffer[1] = temp;
	/* Transmitir los 2 bytes al RTC */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 2, i2c_tx_buffer);
	
	return salida;
	
}


uint8_t RTC_Alarm_Int_Flag_Clear(void){
	
	uint8_t salida = I2C_OK;
	uint8_t temp = 0;

	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = CONTROL_2_REG;
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 1, i2c_tx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}
	/* Leer el valor actual del registro */
	salida = TWI0_Master_Read_Data(PCF8523_SLAVE_ADDR, 1, &temp);
	if(salida == I2C_ERROR){
		return salida;
	}
	
	/* Forzar a '0' la bandera de interrupcion por alarma */
	temp &= ~(1 << BIT_AF);
	
	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = CONTROL_2_REG;
	/* Cargar el valor actualizado */
	i2c_tx_buffer[1] = temp;
	/* Transmitir los 2 bytes al RTC */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 2, i2c_tx_buffer);
	
	return salida;
}


uint8_t RTC_Time_Alarm_Enable(void){

	uint8_t salida = I2C_OK;
	uint8_t temp[2] = {0, 0};

	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = MINUTE_ALARM_REG;
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 1, i2c_tx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}
	/* Leer el valor actual de los minutos y las horas */
	salida = TWI0_Master_Read_Data(PCF8523_SLAVE_ADDR, 2, temp);
	if(salida == I2C_ERROR){
		return salida;
	}
	
	/* Forzar a '0' los bits de habilitación de alarma */
	temp[0] &= ~(1 << BIT_AEN_M);
	temp[1] &= ~(1 << BIT_AEN_H);
	
	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = MINUTE_ALARM_REG;
	/* Cargar los valores actualizados */
	i2c_tx_buffer[1] = temp[0];
	i2c_tx_buffer[2] = temp[1];
	/* Transmitir los 2 bytes al RTC */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 3, i2c_tx_buffer);
	
	return salida;
}


uint8_t RTC_Time_Alarm_Disable(void){
	
	uint8_t salida = I2C_OK;
	uint8_t temp[2] = {0, 0};

	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = MINUTE_ALARM_REG;
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 1, i2c_tx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}
	/* Leer el valor actual de los minutos y las horas */
	salida = TWI0_Master_Read_Data(PCF8523_SLAVE_ADDR, 2, temp);
	if(salida == I2C_ERROR){
		return salida;
	}
	
	/* Forzar a '1' los bits de habilitación de alarma */
	temp[0] |= (1 << BIT_AEN_M);
	temp[1] |= (1 << BIT_AEN_H);
	
	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = MINUTE_ALARM_REG;
	/* Cargar los valores actualizados */
	i2c_tx_buffer[1] = temp[0];
	i2c_tx_buffer[2] = temp[1];
	/* Transmitir los 2 bytes al RTC */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 3, i2c_tx_buffer);
	
	return salida;	
	
}


uint8_t RTC_Day_Alarm_Enable(void){

	uint8_t salida = I2C_OK;
	uint8_t temp = 0;

	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = DAY_ALARM_REG;
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 1, i2c_tx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}
	/* Leer el valor actual del día */
	salida = TWI0_Master_Read_Data(PCF8523_SLAVE_ADDR, 1, &temp);
	if(salida == I2C_ERROR){
		return salida;
	}
	
	/* Forzar a '0' el bit de habilitacion de alarma */
	temp &= ~(1 << BIT_AEN_D);
		
	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = DAY_ALARM_REG;
	/* Cargar los valores actualizados */
	i2c_tx_buffer[1] = temp;
	/* Transmitir los 2 bytes al RTC */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 3, i2c_tx_buffer);
	
	return salida;
	
}


uint8_t RTC_Day_Alarm_Disable(void){

	uint8_t salida = I2C_OK;
	uint8_t temp = 0;

	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = DAY_ALARM_REG;
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 1, i2c_tx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}
	/* Leer el valor actual del día */
	salida = TWI0_Master_Read_Data(PCF8523_SLAVE_ADDR, 1, &temp);
	if(salida == I2C_ERROR){
		return salida;
	}
	
	/* Forzar a '1' el bit de habilitacion de alarma */
	temp |= (1 << BIT_AEN_D);
	
	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = DAY_ALARM_REG;
	/* Cargar los valores actualizados */
	i2c_tx_buffer[1] = temp;
	/* Transmitir los 2 bytes al RTC */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 3, i2c_tx_buffer);
	
	return salida;
}


uint8_t RTC_TimerA_Int_Enable(void){

	uint8_t salida = I2C_OK;
	uint8_t temp = 0;

	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = CONTROL_2_REG;
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 1, i2c_tx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}
	/* Leer el valor actual del registro */
	salida = TWI0_Master_Read_Data(PCF8523_SLAVE_ADDR, 1, &temp);
	if(salida == I2C_ERROR){
		return salida;
	}
	
	/* Forzar a '1' la bandera de interrupcion por TimerA */
	temp |= (1 << BIT_CTAIE);
	
	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = CONTROL_2_REG;
	/* Cargar el valor actualizado */
	i2c_tx_buffer[1] = temp;
	/* Transmitir los 2 bytes al RTC */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 2, i2c_tx_buffer);
	
	return salida;
	
}


uint8_t RTC_TimerA_Int_Disable(void){

	uint8_t salida = I2C_OK;
	uint8_t temp = 0;

	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = CONTROL_2_REG;
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 1, i2c_tx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}
	/* Leer el valor actual del registro */
	salida = TWI0_Master_Read_Data(PCF8523_SLAVE_ADDR, 1, &temp);
	if(salida == I2C_ERROR){
		return salida;
	}
	
	/* Forzar a '0' la bandera de interrupcion por TimerA */
	temp &= ~(1 << BIT_CTAIE);
	
	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = CONTROL_2_REG;
	/* Cargar el valor actualizado */
	i2c_tx_buffer[1] = temp;
	/* Transmitir los 2 bytes al RTC */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 2, i2c_tx_buffer);
	
	return salida;
	
}


uint8_t RTC_TimerB_Int_Enable(void){

	uint8_t salida = I2C_OK;
	uint8_t temp = 0;

	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = CONTROL_2_REG;
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 1, i2c_tx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}
	/* Leer el valor actual del registro */
	salida = TWI0_Master_Read_Data(PCF8523_SLAVE_ADDR, 1, &temp);
	if(salida == I2C_ERROR){
		return salida;
	}
	
	/* Forzar a '1' la bandera de interrupcion por TimerB */
	temp |= (1 << BIT_CTBIE);
	
	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = CONTROL_2_REG;
	/* Cargar el valor actualizado */
	i2c_tx_buffer[1] = temp;
	/* Transmitir los 2 bytes al RTC */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 2, i2c_tx_buffer);
	
	return salida;
	
}


uint8_t RTC_TimerB_Int_Disable(void){

	uint8_t salida = I2C_OK;
	uint8_t temp = 0;

	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = CONTROL_2_REG;
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 1, i2c_tx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}
	/* Leer el valor actual del registro */
	salida = TWI0_Master_Read_Data(PCF8523_SLAVE_ADDR, 1, &temp);
	if(salida == I2C_ERROR){
		return salida;
	}
	
	/* Forzar a '0' la bandera de interrupcion por TimerB */
	temp &= ~(1 << BIT_CTBIE);
	
	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = CONTROL_2_REG;
	/* Cargar el valor actualizado */
	i2c_tx_buffer[1] = temp;
	/* Transmitir los 2 bytes al RTC */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 2, i2c_tx_buffer);
	
	return salida;
	
}


uint8_t RTC_TimerA_Int_Flag_Clear(void){

	uint8_t salida = I2C_OK;
	uint8_t temp = 0;

	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = CONTROL_2_REG;
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 1, i2c_tx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}
	/* Leer el valor actual del registro */
	salida = TWI0_Master_Read_Data(PCF8523_SLAVE_ADDR, 1, &temp);
	if(salida == I2C_ERROR){
		return salida;
	}
	
	/* Forzar a '0' la bandera de interrupcion por TimerA */
	temp &= ~(1 << BIT_CTAF);
	
	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = CONTROL_2_REG;
	/* Cargar el valor actualizado */
	i2c_tx_buffer[1] = temp;
	/* Transmitir los 2 bytes al RTC */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 2, i2c_tx_buffer);
	
	return salida;
}


uint8_t RTC_TimerB_Int_Flag_Clear(void){
	
	uint8_t salida = I2C_OK;
	uint8_t temp = 0;

	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = CONTROL_2_REG;
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 1, i2c_tx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}
	/* Leer el valor actual del registro */
	salida = TWI0_Master_Read_Data(PCF8523_SLAVE_ADDR, 1, &temp);
	if(salida == I2C_ERROR){
		return salida;
	}
	
	/* Forzar a '0' la bandera de interrupcion por TimerA */
	temp &= ~(1 << BIT_CTBF);
	
	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = CONTROL_2_REG;
	/* Cargar el valor actualizado */
	i2c_tx_buffer[1] = temp;
	/* Transmitir los 2 bytes al RTC */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 2, i2c_tx_buffer);
	
	return salida;		
}


uint8_t RTC_TimerA_Clock_Source_Select(rtc_timer_clk_src_t clk_src){

	uint8_t salida = I2C_OK;

	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = TMR_A_FREQ_CTRL_REG;
	/* Cargar el valor de la fuente de reloj */
	i2c_tx_buffer[1] = clk_src;
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 2, i2c_tx_buffer);

	return salida;
}


uint8_t RTC_TimerB_Clock_Source_Select(rtc_timer_clk_src_t clk_src){

	uint8_t salida = I2C_OK;

	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = (uint8_t)TMR_B_FREQ_CTRL_REG;
	/* Cargar el valor de la fuente de reloj */
	i2c_tx_buffer[1] = clk_src;
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 2, i2c_tx_buffer);
	
	return salida;
	
}


uint8_t RTC_Clockout_Source_Select(rtc_clockout_src_t clk_src){

	uint8_t salida = I2C_OK;
	uint8_t temp = 0;

	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = TMR_CLKOUT_CTRL_REG;
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 1, i2c_tx_buffer);
	if(salida == I2C_ERROR){
		return salida;
	}
	/* Leer el valor actual del registro */
	salida = TWI0_Master_Read_Data(PCF8523_SLAVE_ADDR, 1, &temp);
	if(salida == I2C_ERROR){
		return salida;
	}

	/* Forzar a 1 os bits correspondientes */
	temp &= ~((1 << BIT_COF0) | (1 << BIT_COF1) |(1 <<BIT_COF2));
	temp |= ((clk_src & 0x07) << BIT_COF0);

	/* Cargar dirección del registro de configuracion */
	i2c_tx_buffer[0] = TMR_CLKOUT_CTRL_REG;
	/* Cargar el valor actualizado */
	i2c_tx_buffer[1] = temp;
	/* Transmitir los 2 bytes al RTC */
	salida = TWI0_Master_Write_Data(PCF8523_SLAVE_ADDR, 2, i2c_tx_buffer);

	return salida;
}


