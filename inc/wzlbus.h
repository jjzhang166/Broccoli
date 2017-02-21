#ifndef __WZL_BUS_H__
	#define __WZL_BUS_H__
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdint.h>
	#include <string.h>

	#define WBUS_BUFFERSIZE	257	//接收缓冲区大小
	#define WBUS_SX1278_Reset					0xFF
	#define WBUS_SX1278_Sleep					0x00
	#define WBUS_SX1278_Send					0x80
	#define WBUS_SX1278_Receive					0x40

	#define	WBUS_SX1278_SetFrequency			0xC1
	#define	WBUS_SX1278_SetSpreadingFactor		0xC2
	#define	WBUS_SX1278_SetCodingRate			0xC3
	#define	WBUS_SX1278_SetBw_Frequency			0xC4
	#define	WBUS_SX1278_SetPowerValue			0xC5

	#define	WBUS_SX1278_SetTXFrequency			0x81
	#define	WBUS_SX1278_SetTXSpreadingFactor	0x82
	#define	WBUS_SX1278_SetTXCodingRate			0x83
	#define	WBUS_SX1278_SetTXBw_Frequency		0x84
	#define	WBUS_SX1278_SetTXPowerValue			0x85


	#define	WBUS_SX1278_SetRXFrequency			0x41
	#define	WBUS_SX1278_SetRXSpreadingFactor	0x42
	#define	WBUS_SX1278_SetRXCodingRate			0x43
	#define	WBUS_SX1278_SetRXBw_Frequency		0x44
	#define	WBUS_SX1278_SetRXPowerValue			0x45

	void wbus_crc_error(void);
	void uart_send_byte(uint8_t byte);
	void wbus_send_head(void);
	void wbus_send_one_byte(uint8_t dat);
	void wbus_send_the_end(void);
	void wbus_send_bytes(const uint8_t *dat,int count);
	void wbus_receive(uint8_t c);
	void wbus_process(const uint8_t *data, uint16_t length);

#endif //__WZL_BUS_H__
