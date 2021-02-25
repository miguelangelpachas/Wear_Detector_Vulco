/*
 * adc.c
 *
 *  Created on: 19 mar. 2018
 *      Author: MiguelAngel
 */

/* Inclusiones */
#include <stdint.h>
#include <avr/io.h>
#include "MT_adc.h"


/****************************************************************************
* Nombre de la función: ADC_Configurar
* retorna : Nada
* arg : adc_param
* Descripción : Recibe un puntero hacia una estructura de tipo ADC_t (la
* 				cual contiene los parámetros de configuración) y configura
* 				el módulo ADC.
* Notas : Esta función no está diseñada para configurar canales diferenciales
* 		  ni ganancias. Tampoco permite deshabilitar las entradas digitales
* 		  que no se van a utilizar.
*****************************************************************************/
void ADC_Configurar(ADC_t *adc_param){

	/* Inicializar los registros de configuración */
	ADMUX = 0;
	ADCSRB = 0;
	ADCSRA = 0;

	/* Habilitar el ADC */
	ADCSRA |= (1 << ADEN);
	/* Configurar el Pre-Scaler del ADC */
	ADCSRA |= (adc_param->prescaler) << ADPS0;
	/* Configurar el voltaje de referencia */
	ADMUX |= (adc_param->vref) << REFS0;
	/* Cargar los nuevos valores de los bits selectores de canal */
	ADMUX |= (adc_param->channel & 0x07) << MUX0;
	/* Configurar el trigger automático */
	ADCSRA |= (adc_param->auto_trigger) << ADATE;
	ADCSRB |= (adc_param->trigger_source) << ADTS0;
	/* Configurar el habilitador de interrupción */
	ADCSRA |= adc_param->interrupt_mask;
	/* Llevar a cabo una primera conversión */
    ADCSRA |= (1 << ADSC);
	while(!(ADCSRA & (1 << ADIF))){
		/* No hacer nada hasta que la conversión finalice */
	}
	/* Limpiar la bandera de conversión completa */
	ADCSRA |= (1 << ADIF);
}

/****************************************************************************
* Nombre de la función: ADC_Canal_Seleccionar
* retorna : Nada
* arg : canal
* Descripción : Recibe como parámetro el número de canal analógico que
* 				se desea muestrear (de 0 a 15) y configura el módulo
* 				ADC para leer el canal seleccionado.
* Notas : No llamar a esta función mientras una conversión se esté llevando
* 		  a cabo (esperar a que la conversión termine). Esta función solo
* 		  permite seleccionar los canales simples de 0 a 15 (no permite
* 		  seleccionar canales diferenciales con ganancia programable). La
* 		  función cuenta con un "filtro" interno en caso que el parámetro
* 		  ingresado se encuentre fuera del rango de 0 a 15.
*****************************************************************************/
void ADC_Seleccionar_Canal(uint8_t canal){

	ADC_Channel_t selector;

	/* Tabla para la configuración de los canales */
	ADC_Channel_t tabla_canales[] ={
										ADC_Channel_0,
										ADC_Channel_1,
										ADC_Channel_2,
										ADC_Channel_3,
										ADC_Channel_4,
										ADC_Channel_5,
										ADC_Channel_6,
										ADC_Channel_7
									};

	/* Filtrar el parámetro canal para que no apunte fuera de la tabla */
	canal &= 0x07;
	/* Cargar el valor correspondiente al selector */
	selector = tabla_canales[canal];
	/* Limpiar todos los bits selectores de canal del ADC */
	ADMUX &= ~((1 << MUX2) | (1 << MUX1) | (1 << MUX0));
	/* Cargar los nuevos valores de los bits selectores */
	ADMUX |= (selector & 0x07) << MUX0;
}
