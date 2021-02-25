
#ifndef MT_24AAXX_H_
#define MT_24AAXX_H_

/* Inclusión de archivos */
#include <stdint.h>

/* Constants and macros */

#define EEPROM_SLAVE_ADDRESS		0x50

#define EEPROM_SIZE_IN_BITS			256000
#define EEPROM_SIZE_IN_BYTES		(EEPROM_SIZE_IN_BITS / 8)
#define EEPROM_PAGE_SIZE_BYTES		64
#define EEPROM_N_PAGES				(EEPROM_SIZE_IN_BYTES / EEPROM_PAGE_SIZE_BYTES)  
#define EEPROM_MAX_ADDRESS			(EEPROM_N_PAGES*EEPROM_PAGE_SIZE_BYTES - 1)

/* Declaración de funciones (prototipos) */

uint8_t EEPROM_24AAxx_Write(uint8_t slave_addr, uint16_t mem_addr, uint8_t n_bytes, uint8_t *buffer);
uint8_t EEPROM_24AAxx_Read(uint8_t slave_addr, uint16_t mem_addr, uint8_t n_bytes, uint8_t *buffer);
uint8_t EEPROM_24AAxx_Clear(uint8_t slave_addr, uint16_t mem_addr, uint8_t n_bytes);

uint8_t EEPROM_24AAxx_Write_Extended(uint8_t slave_addr, uint16_t mem_addr, uint16_t n_bytes, uint8_t *buffer);
uint8_t EEPROM_24AAxx_Read_Extended(uint8_t slave_addr, uint16_t mem_addr, uint16_t n_bytes, uint8_t *buffer);
uint8_t EEPROM_24AAxx_Clear_Extended(uint8_t slave_addr, uint16_t mem_addr, uint16_t n_bytes);

#endif
