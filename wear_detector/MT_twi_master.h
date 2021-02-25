
#ifndef MT_TWI_MASTER_H_
#define MT_TWI_MASTER_H_

/* Inclusión de archivos */
#include <avr/io.h>
#include <stdint.h>

/* Constantes y macros */
#define	I2C_START				0x08
#define I2C_RESTART				0x10
#define I2C_SLAW_ACK			0x18
#define I2C_SLAW_NACK			0x20
#define I2C_DATA_TX_ACK			0x28
#define I2C_DATA_TX_NACK		0x30
#define I2C_ARBIT_LOST			0x38
#define I2C_SLAR_ACK			0x40
#define I2C_SLAR_NACK			0x48
#define I2C_DATA_RX_ACK			0x50
#define I2C_DATA_RX_NACK		0x58

#define I2C_OK					0x00
#define I2C_ERROR				0x01


/* Definiciones de tipos y estructuras */

typedef enum{
	TWI_Addr_Write_Mode = 0,
	TWI_Addr_Read_Mode = 1
}twi_address_mode_t;


typedef enum{
	SCL_Prescaler_1 = 0,
	SCL_Prescaler_4,
	SCL_Prescaler_16,
	SCL_Prescaler_64
}twi_clock_prescaler_t;


typedef struct{
	uint8_t twbr;
	uint8_t address_mask;
	uint8_t slave_address;
	twi_clock_prescaler_t twps;
}twi_config_t;


/* Declaración de funciones */
void TWI0_Configurar(twi_config_t *twi_param);
uint8_t TWI0_Master_Start_Condition(void);
uint8_t TWI0_Master_Tx_Address(uint8_t slave_address, twi_address_mode_t addr_mode);
uint8_t TWI0_Master_Tx_Byte(uint8_t byte);
uint8_t TWI0_Master_Rx_Byte_ACK(uint8_t *byte_p);
uint8_t TWI0_Master_Rx_Byte_NACK(uint8_t *byte_p);
uint8_t TWI0_Master_Write_Data(uint8_t slave_write_addr, uint8_t n_bytes, uint8_t *output_buffer);
uint8_t TWI0_Master_Read_Data(uint8_t slave_read_addr, uint8_t n_bytes, uint8_t *input_buffer);
void TWI0_Master_Stop_Condition(void);

void TWI1_Configurar(twi_config_t *twi_param);
uint8_t TWI1_Master_Start_Condition(void);
uint8_t TWI1_Master_Tx_Address(uint8_t slave_address, twi_address_mode_t addr_mode);
uint8_t TWI1_Master_Tx_Byte(uint8_t byte);
uint8_t TWI1_Master_Rx_Byte_ACK(uint8_t *byte_p);
uint8_t TWI1_Master_Rx_Byte_NACK(uint8_t *byte_p);
uint8_t TWI1_Master_Write_Data(uint8_t slave_write_addr, uint8_t n_bytes, uint8_t *output_buffer);
uint8_t TWI1_Master_Read_Data(uint8_t slave_read_addr, uint8_t n_bytes, uint8_t *input_buffer);
void TWI1_Master_Stop_Condition(void);

#endif /* TWI_MASTER_H_ */
