

#ifndef XBEE_INTERFACE_H_
#define XBEE_INTERFACE_H_

#include "grinding_mill_wearing_out_monitor_node.h"


/* Type definition */
typedef enum{
	DATA_FRAME_ACCEPTED = 0,
	DATA_FRAME_REJECTED,
	DATA_FRAME_TIMEOUT,
	DATA_FRAME_TRANSFER_ERROR
}data_frame_transfer_status_t;

/* Function declaration */
void Xbee_Interface_Initialize(void);
void Xbee_Interface_UART_Init(void);
void Xbee_Interface_On(void);
void Xbee_Interface_Off(void);
void Xbee_Send_String(char *str);

void Xbee_Build_Data_Accepted_Frame(data_frame_t *data_frame);
void Xbee_Build_Data_Rejected_Frame(data_frame_t *data_frame);
void Xbee_Build_Data_Frame(data_frame_t *data_frame);
void Xbee_Send_Data_Frame(void);
void Xbee_Serial_Rx_Enable(void);
void Xbee_Serial_Rx_Disable(void);
uint8_t *Xbee_Get_Data_Frame_Buffer_Pointer(void);

data_frame_transfer_status_t Xbee_Send_Data_And_Verify(void);
uint8_t Xbee_Send_Backup_Data_Frames(void);

#endif