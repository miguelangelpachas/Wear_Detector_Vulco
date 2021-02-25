
/* Inclusión de archivos */
#include <avr/io.h>
#include <stdint.h> 
#include "MT_twi_master.h"

/* Variables globales */
volatile uint8_t i2c_status = I2C_OK;


/* Definición de funciones */

/****************************************************************************
* Nombre de la función: TWI_Configurar
* retorna : Nada
* arg : twi_param
* Descripción : Recibe un puntero hacia una estructura de tipo twi_config_t,
*               la cual contiene los parámetros de configuración) y configura
* 				el módulo TWI.
* Notas : Esta función no está diseñada para establecer si la interfaz TWI
*         trbajará en modo maestro o esclavo, ya que, a diferencia del bus
*         SPI, el bus I2C/TWI es multi-maestro.
*****************************************************************************/
void TWI0_Configurar(twi_config_t *twi_param){

	TWBR0 = 0;
	TWSR0 = 0;
	TWAR0 = 0;
	TWAMR0 = 0;

	TWBR0 = twi_param->twbr;
	TWSR0 |= twi_param->twps & 0x03;
	TWAR0 |= (twi_param->slave_address << 1) & 0xFE;
	TWAMR0 |= (twi_param->address_mask << 1) & 0xFE;

	/* Asegurarse de limpiar el bit de reducción de consumo de energía */
	PRR0 &= ~(1 << PRTWI0);
}


void TWI1_Configurar(twi_config_t *twi_param){

	TWBR1 = 0;
	TWSR1 = 0;
	TWAR1 = 0;
	TWAMR1 = 0;

	TWBR1 = twi_param->twbr;
	TWSR1 |= twi_param->twps & 0x03;
	TWAR1 |= (twi_param->slave_address << 1) & 0xFE;
	TWAMR1 |= (twi_param->address_mask << 1) & 0xFE;

	/* Asegurarse de limpiar el bit de reducción de consumo de energía */
	PRR1 &= ~(1 << PRTWI1);
}

/****************************************************************************
* Nombre de la función: TWI_Master_Start_Condition
* retorna : el estado del bus I2C/TWI (si hubo algún error en la transmisión)
* arg : Ninguno
* Descripción : Transmite la condición START, necesaria para iniciar la
*               comunicación con un dispositivo esclavo.
* Notas : Al transmitir exitosamente una condición START, el microcontrolador
* 		  asume el rol de dispositivo maestro e inicia la comunicación.
*****************************************************************************/
uint8_t TWI0_Master_Start_Condition(void){

	uint8_t salida = I2C_OK;

	/* Enviar una condición de START */
	TWCR0 = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

	/* Esperar a que la transmisión se complete */
	while (!(TWCR0 & (1<<TWINT))){
		/* No hacer nada */
	}

	/* Verificar la condición de START */
	if ((TWSR0 & 0xF8) != I2C_START){
		i2c_status = TWSR0 & 0xF8; 
		salida = I2C_ERROR;
	}

	return salida;

}


uint8_t TWI1_Master_Start_Condition(void){

	uint8_t salida = I2C_OK;

	/* Enviar una condición de START */
	TWCR1 = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

	/* Esperar a que la transmisión se complete */
	while (!(TWCR1 & (1<<TWINT))){
		/* No hacer nada */
	}

	/* Verificar la condición de START */
	if ((TWSR1 & 0xF8) != I2C_START){
		i2c_status = TWSR0 & 0xF8;
		salida = I2C_ERROR;
	}

	return salida;

}


/****************************************************************************
* Nombre de la función: TWI_Master_Tx_Address
* retorna : el estado del bus I2C/TWI (si hubo algún error en la transmisión)
* arg : slave_address
* Descripción : Recibe como parámetro un número de 8 bits, conformado por la
* 				dirección de 7 bits del dispositivo esclavo y el bit R/W (LSB).
* 				Este número se transmite por el bus I2C/TWI en la forma de una
* 				trama de dirección.
* Notas : Si ningún dispositivo esclavo responde a la transmisión con una
*         condición ACK, entonces la función retornará un valor que indica
*         que ha ocurrido un ERROR.
*****************************************************************************/
uint8_t TWI0_Master_Tx_Address(uint8_t slave_address, twi_address_mode_t addr_mode){

	uint8_t salida = I2C_OK;

	/* Cargar la dirección del esclavo e iniciar la transmisión */
	
	if(addr_mode == TWI_Addr_Read_Mode){
		TWDR0 = (slave_address << 1) | 0x01;	
	}else{
		TWDR0 = (slave_address << 1) & 0xFE;
	}
	
	TWCR0 = (1<<TWINT) | (1<<TWEN);

	/* Esperar a que la transmisión se complete */
	while (!(TWCR0 & (1<<TWINT))){
		/* No hacer nada */
	}

	/* Verificar el envío de la dirección */
	if(addr_mode == TWI_Addr_Read_Mode){
		if ((TWSR0 & 0xF8) != I2C_SLAR_ACK){
			i2c_status = TWSR0 & 0xF8;
			salida = I2C_ERROR;
		}		
	}else{
		if ((TWSR0 & 0xF8) != I2C_SLAW_ACK){
			salida = I2C_ERROR;
		}		
	}
	
	return salida;

}


uint8_t TWI1_Master_Tx_Address(uint8_t slave_address, twi_address_mode_t addr_mode){

	uint8_t salida = I2C_OK;

	/* Cargar la dirección del esclavo e iniciar la transmisión */
	
	if(addr_mode == TWI_Addr_Read_Mode){
		TWDR1 = (slave_address << 1) | 0x01;
	}else{
		TWDR1 = (slave_address << 1) & 0xFE;
	}
	
	TWCR1 = (1<<TWINT) | (1<<TWEN);

	/* Esperar a que la transmisión se complete */
	while (!(TWCR1 & (1<<TWINT))){
		/* No hacer nada */
	}

	/* Verificar el envío de la dirección */
	if(addr_mode == TWI_Addr_Read_Mode){
		if ((TWSR1 & 0xF8) != I2C_SLAR_ACK){
			i2c_status = TWSR1 & 0xF8;
			salida = I2C_ERROR;
		}
	}else{
		if ((TWSR1 & 0xF8) != I2C_SLAW_ACK){
			salida = I2C_ERROR;
		}
	}
	
	return salida;

}


/****************************************************************************
* Nombre de la función: TWI_Master_Tx_Byte
* retorna : el estado del bus I2C/TWI (si hubo algún error en la transmisión)
* arg : byte
* Descripción : Recibe como parámetro un dato de 8 bits y lo transmite por
* 				el bus I2C/TWI en la forma de una trama de datos.
* Notas : Si ningún dispositivo esclavo responde a la transmisión con una
*         condición ACK, entonces la función retornará un valor que indica
*         que ha ocurrido un ERROR.
*****************************************************************************/
uint8_t TWI0_Master_Tx_Byte(uint8_t byte){

	uint8_t salida = I2C_OK;

	/* Cargar el dato e iniciar la transmisión */
	TWDR0 = byte;
	TWCR0 = (1<<TWINT) | (1<<TWEN);

	/* Esperar a que el dato se termine de recibir */
	while (!(TWCR0 & (1<<TWINT))){
		/* No hacer nada */
	}

	/* Verificar la recepción del dato */
	if ((TWSR0 & 0xF8) != I2C_DATA_TX_ACK){
		i2c_status = TWSR0 & 0xF8;
		salida = I2C_ERROR;
	}

	return salida;

}


uint8_t TWI1_Master_Tx_Byte(uint8_t byte){

	uint8_t salida = I2C_OK;

	/* Cargar el dato e iniciar la transmisión */
	TWDR1 = byte;
	TWCR1 = (1<<TWINT) | (1<<TWEN);

	/* Esperar a que el dato se termine de recibir */
	while (!(TWCR1 & (1<<TWINT))){
		/* No hacer nada */
	}

	/* Verificar la recepción del dato */
	if ((TWSR1 & 0xF8) != I2C_DATA_TX_ACK){
		i2c_status = TWSR1 & 0xF8;
		salida = I2C_ERROR;
	}

	return salida;
}


/****************************************************************************
* Nombre de la función: TWI_Master_Rx_Byte_ACK
* retorna : el estado del bus I2C/TWI (si hubo algún error en la recepción)
* arg : byte
* Descripción : Recibe como parámetro un puntero a un dato de 8 bits. Este
* 				puntero indica la ubicación de memoria donde se almacenará
* 				el dato que es recibido a través de una trama de datos.
* Notas : Al terminar de recibir el dato de 8 bits, esta función retorna una
* 		  condición ACK, lo cual quiere decir que se espera a que el esclavo
* 		  continúe enviando tramas de datos al maestro.
*****************************************************************************/
uint8_t TWI0_Master_Rx_Byte_ACK(uint8_t *byte_p){

	uint8_t salida = I2C_OK;

	/* Iniciar la recepción de datos */
	TWCR0 = (1<<TWINT) | (1<<TWEN) | (1 << TWEA);

	/* Esperar a que el dato se termine de recibir */
	while (!(TWCR0 & (1<<TWINT))){
		/* No hacer nada */
	}

	/* Verificar la recepción del dato */
	if ((TWSR0 & 0xF8) != I2C_DATA_RX_ACK){
		i2c_status = TWSR0 & 0xF8;
		salida = I2C_ERROR;
	}

	*byte_p = TWDR0;
	return salida;

}


uint8_t TWI1_Master_Rx_Byte_ACK(uint8_t *byte_p){

	uint8_t salida = I2C_OK;

	/* Iniciar la recepción de datos */
	TWCR1 = (1<<TWINT) | (1<<TWEN) | (1 << TWEA);

	/* Esperar a que el dato se termine de recibir */
	while (!(TWCR1 & (1<<TWINT))){
		/* No hacer nada */
	}

	/* Verificar la recepción del dato */
	if ((TWSR1 & 0xF8) != I2C_DATA_RX_ACK){
		i2c_status = TWSR1 & 0xF8;
		salida = I2C_ERROR;
	}

	*byte_p = TWDR1;
	return salida;

}

/****************************************************************************
* Nombre de la función: TWI_Master_Rx_Byte_ACK
* retorna : el estado del bus I2C/TWI (si hubo algún error en la recepción)
* arg : byte
* Descripción : Recibe como parámetro un puntero a un dato de 8 bits. Este
* 				puntero indica la ubicación de memoria donde se almacenará
* 				el dato que es recibido a través de una trama de datos.
* Notas : Al terminar de recibir el dato de 8 bits, esta función retorna una
* 		  condición NACK, lo cual quiere decir el esclavo ya no transmitirá
* 		  más datos al maestro. Luego de enviar la condición NACK, el maestro
* 		  debe transmitir una condición STOP.
*****************************************************************************/
uint8_t TWI0_Master_Rx_Byte_NACK(uint8_t *byte_p){

	uint8_t salida = I2C_OK;

	/* Iniciar la recepción de datos */
	TWCR0 = (1<<TWINT) | (1<<TWEN);

	/* Esperar a que el dato se termine de recibir */
	while (!(TWCR0 & (1<<TWINT))){
		/* No hacer nada */
	}

	/* Verificar la recepción del dato */
	if ((TWSR0 & 0xF8) != I2C_DATA_RX_NACK){
		i2c_status = TWSR0 & 0xF8;
		salida = I2C_ERROR;
	}

	*byte_p = TWDR0;

	return salida;
}


uint8_t TWI1_Master_Rx_Byte_NACK(uint8_t *byte_p){

	uint8_t salida = I2C_OK;

	/* Iniciar la recepción de datos */
	TWCR1 = (1<<TWINT) | (1<<TWEN);

	/* Esperar a que el dato se termine de recibir */
	while (!(TWCR1 & (1<<TWINT))){
		/* No hacer nada */
	}

	/* Verificar la recepción del dato */
	if ((TWSR1 & 0xF8) != I2C_DATA_RX_NACK){
		i2c_status = TWSR1 & 0xF8;
		salida = I2C_ERROR;
	}

	*byte_p = TWDR1;

	return salida;

}

/****************************************************************************
* Nombre de la función: TWI_Master_Stop_Condition
* retorna : nada
* arg : Ninguno
* Descripción : Transmite una condición STOP, la cual completa un proceso de
* 		 		comunicación en el bus I2C.
* Notas : Esta función es utilizada para indicar la finalización del proceso
* 		  de comunicación.
*****************************************************************************/
void TWI0_Master_Stop_Condition(void){
	/* Transmitir la condición de STOP */
	TWCR0 = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

void TWI1_Master_Stop_Condition(void){
	/* Transmitir la condición de STOP */
	TWCR1 = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

/****************************************************************************
* Nombre de la función: TWI_Master_Restart_Condition
* retorna : el estado del bus I2C/TWI (si hubo algún error en la transmisión)
* arg : Ninguno
* Descripción : Transmite una condición START, sin necesidad de haber enviado
* 				antes una condición STOP.
* Notas : Esta función es utilizada cuando el bus TWI trabaja en modo multi-
* 		  maestro. Con esta función es posible iniciar una nueva comunicación
* 		  sin necesidad de haber transmitido una condición STOP.
*****************************************************************************/
uint8_t TWI0_Master_Restart_Condition(void){

	uint8_t salida = I2C_OK;

	/* Enviar una condición de START */
	TWCR0 = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	/* Esperar a que la condición de START se termine de enviar */
	while (!(TWCR0 & (1<<TWINT))){
		/* No hacer nada */
	}
	/* Verificar la condición de RESTART */
	if ((TWSR0 & 0xF8) != I2C_RESTART){
		i2c_status = TWSR0 & 0xF8;
		salida = I2C_ERROR;
	}
	return salida;
}


uint8_t TWI1_Master_Restart_Condition(void){

	uint8_t salida = I2C_OK;

	/* Enviar una condición de START */
	TWCR1 = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	/* Esperar a que la condición de START se termine de enviar */
	while (!(TWCR1 & (1<<TWINT))){
		/* No hacer nada */
	}
	/* Verificar la condición de RESTART */
	if ((TWSR1 & 0xF8) != I2C_RESTART){
		i2c_status = TWSR1 & 0xF8;
		salida = I2C_ERROR;
	}
	return salida;
}

/****************************************************************************
* Nombre de la función: TWI_Master_Write_Data
* retorna : el estado del bus I2C/TWI (si hubo algún error en la transmisión)
* arg : slave_addr
* arg: n_bytes
* arg: output_buffer
* Descripción : Primero,  transmite el valor slave_write_addr (SLA + 0) en una
* 				trama de dirección. Luego, transmite 'n' bytes consecutivos,
* 				en forma de tramas de datos.
* Notas : Los 'n' bytes que serán transmitidos se encuentran almacenados en el
* 		  arreglo output_buffer. Puede aprovechar el arreglo i2c_tx_buffer y
* 		  pasarlo como parámetro output_buffer.
*****************************************************************************/
uint8_t TWI0_Master_Write_Data(uint8_t slave_addr, uint8_t n_bytes, uint8_t *output_buffer){
	
	uint8_t salida = I2C_OK;
	uint8_t index;

	/* Generar la condición de start */
	salida = TWI0_Master_Start_Condition();
	if(salida == I2C_ERROR){
		return salida;
	}

	/* Transmitir dirección del esclavo y modo (lectura o escritura) */
	salida = TWI0_Master_Tx_Address(slave_addr, TWI_Addr_Write_Mode);
	if(salida == I2C_ERROR){
		return salida;
	}

	/* Transmitir los datos */
	for(index = 0; index < n_bytes; index++){
		salida = TWI0_Master_Tx_Byte(*(output_buffer + index));
		if(salida == I2C_ERROR){
			break;
		}
	}
	
	if(salida == I2C_ERROR){
		return salida;	
	}

	TWI0_Master_Stop_Condition();

	return salida;

}



uint8_t TWI1_Master_Write_Data(uint8_t slave_addr, uint8_t n_bytes, uint8_t *output_buffer){
	
	uint8_t salida = I2C_OK;
	uint8_t index;

	/* Generar la condición de start */
	salida = TWI1_Master_Start_Condition();
	if(salida == I2C_ERROR){
		return salida;
	}

	/* Transmitir dirección del esclavo y modo (lectura o escritura) */
	salida = TWI1_Master_Tx_Address(slave_addr, TWI_Addr_Write_Mode);
	if(salida == I2C_ERROR){
		return salida;
	}

	/* Transmitir los datos */
	for(index = 0; index < n_bytes; index++){
		salida = TWI1_Master_Tx_Byte(*(output_buffer + index));
		if(salida == I2C_ERROR){
			break;
		}
	}
	
	if(salida == I2C_ERROR){
		return salida;
	}

	TWI1_Master_Stop_Condition();

	return salida;

}


/****************************************************************************
* Nombre de la función: TWI_Master_Read_Data
* retorna : el estado del bus I2C/TWI (si hubo algún error en la transmisión)
* arg : slave_addr
* arg: n_bytes
* arg: intput_buffer
* Descripción : Primero,  transmite el valor slave_read_addr (SLA + 1) en una
* 				trama de dirección. Luego, recibe 'n' bytes consecutivos,
* 				en forma de tramas de datos.
* Notas : Los 'n' bytes que serán recibidos se almacenarán en el arreglo
*  		  input_buffer. Puede aprovechar el arreglo i2c_tx_buffer y pasarlo
*  		  como parámetro input_buffer.
*****************************************************************************/
uint8_t TWI0_Master_Read_Data(uint8_t slave_addr, uint8_t n_bytes, uint8_t *input_buffer){

	uint8_t salida = I2C_OK;
	uint8_t index;

	/* Generar la condición de start */
	salida = TWI0_Master_Start_Condition();
	if(salida == I2C_ERROR){
		return salida;
	}
	
	/* Transmitir dirección del esclavo y modo (lectura) */
	salida = TWI0_Master_Tx_Address(slave_addr, TWI_Addr_Read_Mode);
	if(salida == I2C_ERROR){
		return salida;
	}

	/* Transmitir los datos */
	for(index = 0; index < n_bytes; index++){

		if(index < (n_bytes - 1)){
			salida = TWI0_Master_Rx_Byte_ACK(input_buffer + index);
		}else{
			salida = TWI0_Master_Rx_Byte_NACK(input_buffer + index);
		}

		if(salida == I2C_ERROR){
			break;
		}
	}
	
	if(salida == I2C_ERROR){
		return salida;
	}

	TWI0_Master_Stop_Condition();

	return salida;

}



uint8_t TWI1_Master_Read_Data(uint8_t slave_addr, uint8_t n_bytes, uint8_t *input_buffer){

	uint8_t salida = I2C_OK;
	uint8_t index;

	/* Generar la condición de start */
	salida = TWI1_Master_Start_Condition();
	if(salida == I2C_ERROR){
		return salida;
	}
	
	/* Transmitir dirección del esclavo y modo (lectura) */
	salida = TWI1_Master_Tx_Address(slave_addr, TWI_Addr_Read_Mode);
	if(salida == I2C_ERROR){
		return salida;
	}

	/* Transmitir los datos */
	for(index = 0; index < n_bytes; index++){

		if(index < (n_bytes - 1)){
			salida = TWI1_Master_Rx_Byte_ACK(input_buffer + index);
			}else{
			salida = TWI1_Master_Rx_Byte_NACK(input_buffer + index);
		}

		if(salida == I2C_ERROR){
			break;
		}
	}
	
	if(salida == I2C_ERROR){
		return salida;
	}

	TWI1_Master_Stop_Condition();

	return salida;

}