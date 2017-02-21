/*
 The MIT License (MIT)

Copyright (c) 2017 badlyby

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "Broccoli.h"

volatile static uint8_t BusStatus = BROCCOLI_STATUS_IDLE;
volatile static uint8_t DeviceType = BROCCOLI_COORDINATOR;
volatile static uint8_t CurrentChannel	= 0;
volatile static uint8_t Broccoli_SendFlag = 0;
volatile static uint8_t RadioRxBuffer[MaxDataPackSize];
BASE_PACKAGES *pRadioRxBuffer = (BASE_PACKAGES *)RadioRxBuffer;
volatile static uint16_t RadioRxLen = 0;
uint8_t RadioSendBuffer[MaxDataPackSize];
BASE_PACKAGES *pRadioSendBuffer = (BASE_PACKAGES *)RadioSendBuffer;
DEVICE_ADDRESS DeviceAddr;
DEVICE_ADDRESS BroadCastAddr;
DEVICE_ADDRESS HostAddress;
#ifdef BROCCOLI_DATARELAYMODE
ETOCBUFFER EtoCBuffer[BROCCOLI_EtoCBufSize];
CTOEBUFFER CtoEBuffer[BROCCOLI_CtoEBufSize];
#endif
volatile static uint8_t CurrentEndDeviceFlag = 0;
DEVICE_ADDRESS CurrentEndDeviceAddr;

__weak void SystemWaitTime(void)
{
	//TODO
}

__weak void Radio_Send_Package(uint8_t *data, uint16_t length)
{
	//TODO
}

__weak uint16_t Radio_GetRXBuf(volatile uint8_t *data)
{
	return 0;
	//TODO
}

__weak void Radio_RXMode(void)
{
	//TODO
}

__weak void Radio_SleepMode(void)
{
	//TODO
}

__weak void GetDeviceAddress (DEVICE_ADDRESS *addr)
{
	DEVICE_ADDRESS tmp = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88};
	uint8_t *p = 0x1FFFF7AC;
	memcpy(addr, &tmp,sizeof(DEVICE_ADDRESS));
}

__weak void SaveHostAddress (DEVICE_ADDRESS *addr)
{
	//TODO
}

__weak void LoadHostAddress (DEVICE_ADDRESS *addr)
{
	memcpy(addr, &BroadCastAddr,sizeof(DEVICE_ADDRESS));
}

__weak void CleanHostAddress (void)
{
	memcpy(&HostAddress, &BroadCastAddr,sizeof(DEVICE_ADDRESS));
}

__weak void Radio_SetChannel(uint8_t ch)
{
	//TODO
}

__weak void Broccoli_Receive(DEVICE_ADDRESS *addr1, DEVICE_ADDRESS *addr2, uint8_t *data, uint16_t length)
{
	//TODO
}

uint8_t Broccoli_AllowJoinIn(DEVICE_ADDRESS *addr)
{
	//TODO
	return 1;
}

void Broccoli_SendData(uint8_t *data, uint16_t length)
{
	uint16_t i = BROCCOLI_TX_TIMEOUT;
	while(BusStatus & BROCCOLI_STATUS_SNTX);
	BusStatus |= BROCCOLI_STATUS_TXBUSY;
	Radio_Send_Package(data, length);
	BusStatus &= ~(BROCCOLI_STATUS_TXBUSY);
	while((BusStatus & (BROCCOLI_STATUS_TXOK | BROCCOLI_STATUS_TXERR)) == 0)
	{
		if(i == 0)
		{
			BusStatus |= BROCCOLI_STATUS_TXERR;
			break;
		}
		i--;
		SystemWaitTime();
	}
}

void Radio_StartRX(void)
{
	BusStatus |= BROCCOLI_STATUS_RXBUSY;
}

static uint8_t isAddrForMe(DEVICE_ADDRESS *addr)
{
	if(memcmp(addr, &DeviceAddr, sizeof(DEVICE_ADDRESS)) == 0) return 1;
	if(memcmp(addr, &BroadCastAddr, sizeof(DEVICE_ADDRESS)) == 0) return 2;
	return 0;
}

void Radio_RXDone(uint8_t err)
{
	if(BusStatus & BROCCOLI_STATUS_RXOK) return;
	BusStatus &= ~BROCCOLI_STATUS_RXMASK;
	if(err)
	{
		BusStatus |= BROCCOLI_STATUS_RXERR;
		if(DeviceType == BROCCOLI_ENDDEVICE) Radio_SleepMode();
		else Radio_RXMode();
	}
	else
	{
		RadioRxLen = Radio_GetRXBuf(RadioRxBuffer);
		Radio_RXMode();
		if(RadioRxLen == 0) return;
		if(isAddrForMe(&pRadioRxBuffer->dst_addr) == 0) return;
		switch(DeviceType)
		{
		case BROCCOLI_COORDINATOR:

			switch(pRadioRxBuffer->cmd)
			{
			case BROCCOLI_CMD_C_WHO://搜索信道内协调器
				break;
			case BROCCOLI_CMD_CtoROK:
				if((BusStatus & BROCCOLI_STATUS_INIT) == 0) break;
				Broccoli_SendFlag |= BROCCOLI_SENDFLAG_CtoR;
				break;
			case BROCCOLI_CMD_CtoEOK:
				if((BusStatus & BROCCOLI_STATUS_INIT) == 0) break;
				Broccoli_SendFlag |= BROCCOLI_SENDFLAG_CtoE;
				break;
			default:
				if((BusStatus & BROCCOLI_STATUS_INIT) == 0) break;
				BusStatus |= BROCCOLI_STATUS_RXOK;
				break;
			}
			break;
		case BROCCOLI_FREE_ROUTER:
		case BROCCOLI_ROUTER:
			CurrentEndDeviceFlag = 0;
			switch(pRadioRxBuffer->cmd)
			{
			case BROCCOLI_CMD_C_WHO://搜索信道内协调器
				BusStatus |= BROCCOLI_STATUS_RXOK;
				break;
			case BROCCOLI_CMD_C_ALLOW://C允许R加入
				BusStatus |= BROCCOLI_STATUS_INIT;
				if(memcmp(&HostAddress, &BroadCastAddr, sizeof(DEVICE_ADDRESS)) == 0)
				{
					memcpy(&HostAddress, &pRadioRxBuffer->src_addr, sizeof(DEVICE_ADDRESS));
					SaveHostAddress(&pRadioRxBuffer->src_addr);
				}
				break;
			case BROCCOLI_CMD_EtoR://E->R
				if((BusStatus & BROCCOLI_STATUS_INIT) == 0) break;
				memcpy(&CurrentEndDeviceAddr, &pRadioRxBuffer->src_addr, sizeof(DEVICE_ADDRESS));
				CurrentEndDeviceFlag = 1;
				BusStatus |= BROCCOLI_STATUS_RXOK;
				break;
			case BROCCOLI_CMD_EtoC://E->C
				if((BusStatus & BROCCOLI_STATUS_INIT) == 0) break;
				memcpy(&CurrentEndDeviceAddr, &pRadioRxBuffer->src_addr, sizeof(DEVICE_ADDRESS));
				CurrentEndDeviceFlag = 1;
				BusStatus |= BROCCOLI_STATUS_RXOK;
				break;
			case BROCCOLI_CMD_RtoCOK:
				if((BusStatus & BROCCOLI_STATUS_INIT) == 0) break;
				Broccoli_SendFlag |= BROCCOLI_SENDFLAG_RtoC;
				break;
			case BROCCOLI_CMD_RtoEOK:
				if((BusStatus & BROCCOLI_STATUS_INIT) == 0) break;
				Broccoli_SendFlag |= BROCCOLI_SENDFLAG_RtoE;
				memcpy(&CurrentEndDeviceAddr, &pRadioRxBuffer->src_addr, sizeof(DEVICE_ADDRESS));
				CurrentEndDeviceFlag = 1;
				break;
			case BROCCOLI_CMD_EtoCOK:
				if((BusStatus & BROCCOLI_STATUS_INIT) == 0) break;
				if(DeviceType == BROCCOLI_ROUTER) Broccoli_SendFlag |= BROCCOLI_SENDFLAG_EtoC;
				break;
			case BROCCOLI_CMD_CtoEOK:
				if((BusStatus & BROCCOLI_STATUS_INIT) == 0) break;
				if(DeviceType == BROCCOLI_ROUTER) Broccoli_SendFlag |= BROCCOLI_SENDFLAG_CtoE;
				memcpy(&CurrentEndDeviceAddr, &pRadioRxBuffer->src_addr, sizeof(DEVICE_ADDRESS));
				CurrentEndDeviceFlag = 1;
				break;
			default:
				if((BusStatus & BROCCOLI_STATUS_INIT) == 0) break;
				BusStatus |= BROCCOLI_STATUS_RXOK;
				break;
			}
			break;
		case BROCCOLI_ENDDEVICE:
			switch(pRadioRxBuffer->cmd)
			{
			case BROCCOLI_CMD_R_ALLOW://R允许E加入
				BusStatus |= BROCCOLI_STATUS_INIT;
				memcpy(&HostAddress, &pRadioRxBuffer->src_addr, sizeof(DEVICE_ADDRESS));
				SaveHostAddress(&pRadioRxBuffer->src_addr);
				break;
			case BROCCOLI_CMD_EtoCOK:
				if((BusStatus & BROCCOLI_STATUS_INIT) == 0) break;
				Broccoli_SendFlag |= BROCCOLI_SENDFLAG_EtoC;
				break;
			case BROCCOLI_CMD_EtoROK:
				if((BusStatus & BROCCOLI_STATUS_INIT) == 0) break;
				Broccoli_SendFlag |= BROCCOLI_SENDFLAG_EtoR;
				break;
			default:
				if((BusStatus & BROCCOLI_STATUS_INIT) == 0) break;
				BusStatus |= BROCCOLI_STATUS_RXOK;
				break;
			}
			break;
		}
	}
}

void Radio_TXDone(uint8_t err)
{
	BusStatus &= ~(BROCCOLI_STATUS_TXMASK);
	if(err) BusStatus |= BROCCOLI_STATUS_TXERR;
	BusStatus |= BROCCOLI_STATUS_TXOK;
	Radio_RXMode();
}

static int8_t Broccoli_SendData_Packages(uint8_t flag, uint8_t *data, uint16_t length)
{
	int8_t i,ret;
	uint16_t j;
	ret = BROCCOLI_CONBRK;
	for(i = 0; i < 3; i ++)
	{
		Broccoli_SendFlag &= ~flag;
		BusStatus &= ~(BROCCOLI_STATUS_TXMASK);
		Broccoli_SendData(data, length);
		for(j = 0; j < BROCCOLI_RX_TIMEOUT; j ++)
		{
			if(Broccoli_SendFlag & flag)
				ret = BROCCOLI_OK;
			if(BusStatus & BROCCOLI_STATUS_RXOK)
			{
				Broccoli_MainProcess();
				j = 0;
			}
			SystemWaitTime();
		}
		if(ret == BROCCOLI_OK) break;
	}
	if(DeviceType == BROCCOLI_ENDDEVICE) Radio_SleepMode();
	if(ret == BROCCOLI_CONBRK)
	{
		if((DeviceType == BROCCOLI_ROUTER) || (DeviceType == BROCCOLI_ENDDEVICE)) Broccoli_INIT(0);
	}
	return ret;
}

static int8_t Broccoli_SendData_CtoR(DEVICE_ADDRESS *addr, uint8_t *data, uint16_t length)
{
	pRadioSendBuffer->cmd = BROCCOLI_CMD_CtoR;
	memcpy(&pRadioSendBuffer->dst_addr, addr, sizeof(DEVICE_ADDRESS));
	memcpy(&pRadioSendBuffer->src_addr, &DeviceAddr, sizeof(DEVICE_ADDRESS));
	memcpy(pRadioSendBuffer->payload, data, length);
	return Broccoli_SendData_Packages(BROCCOLI_SENDFLAG_CtoR, RadioSendBuffer, length + sizeof(BASE_PACKAGES));
}

static int8_t Broccoli_SendData_CtoE(DEVICE_ADDRESS *addrE, DEVICE_ADDRESS *addrR, uint8_t *data, uint16_t length)
{
	pRadioSendBuffer->cmd = BROCCOLI_CMD_CtoE;
	memcpy(&pRadioSendBuffer->dst_addr, addrR, sizeof(DEVICE_ADDRESS));
	memcpy(&pRadioSendBuffer->src_addr, &DeviceAddr, sizeof(DEVICE_ADDRESS));
	memcpy(pRadioSendBuffer->payload, addrE, sizeof(DEVICE_ADDRESS));
	memcpy(pRadioSendBuffer->payload + sizeof(DEVICE_ADDRESS), data, length);
	return Broccoli_SendData_Packages(BROCCOLI_SENDFLAG_CtoE, RadioSendBuffer, length + sizeof(BASE_PACKAGES) + sizeof(DEVICE_ADDRESS));
}

static int8_t Broccoli_SendData_RtoC(uint8_t *data, uint16_t length)
{
	pRadioSendBuffer->cmd = BROCCOLI_CMD_RtoC;
	memcpy(&pRadioSendBuffer->dst_addr, &HostAddress, sizeof(DEVICE_ADDRESS));
	memcpy(&pRadioSendBuffer->src_addr, &DeviceAddr, sizeof(DEVICE_ADDRESS));
	memcpy(pRadioSendBuffer->payload, data, length);
	return Broccoli_SendData_Packages(BROCCOLI_SENDFLAG_RtoC, RadioSendBuffer, length + sizeof(BASE_PACKAGES));
}

static int8_t Broccoli_SendData_RtoE(DEVICE_ADDRESS *addr, uint8_t *data, uint16_t length)
{
	pRadioSendBuffer->cmd = BROCCOLI_CMD_RtoE;
	memcpy(&pRadioSendBuffer->dst_addr, addr, sizeof(DEVICE_ADDRESS));
	memcpy(&pRadioSendBuffer->src_addr, &DeviceAddr, sizeof(DEVICE_ADDRESS));
	memcpy(pRadioSendBuffer->payload, data, length);
	return Broccoli_SendData_Packages(BROCCOLI_SENDFLAG_RtoE, RadioSendBuffer, length + sizeof(BASE_PACKAGES));
}

static int8_t Broccoli_SendData_EtoC(uint8_t *data, uint16_t length)
{
	pRadioSendBuffer->cmd = BROCCOLI_CMD_EtoC;
	memcpy(&pRadioSendBuffer->dst_addr, &HostAddress, sizeof(DEVICE_ADDRESS));
	memcpy(&pRadioSendBuffer->src_addr, &DeviceAddr, sizeof(DEVICE_ADDRESS));
	memcpy(pRadioSendBuffer->payload, data, length);
	return Broccoli_SendData_Packages(BROCCOLI_SENDFLAG_EtoC, RadioSendBuffer, length + sizeof(BASE_PACKAGES));
}

static int8_t Broccoli_SendData_EtoR(uint8_t *data, uint16_t length)
{
	pRadioSendBuffer->cmd = BROCCOLI_CMD_EtoR;
	memcpy(&pRadioSendBuffer->dst_addr, &HostAddress, sizeof(DEVICE_ADDRESS));
	memcpy(&pRadioSendBuffer->src_addr, &DeviceAddr, sizeof(DEVICE_ADDRESS));
	memcpy(pRadioSendBuffer->payload, data, length);
	return Broccoli_SendData_Packages(BROCCOLI_SENDFLAG_EtoR, RadioSendBuffer, length + sizeof(BASE_PACKAGES));
}
#ifdef BROCCOLI_DATARELAYMODE
int8_t Broccoli_CtoE_Bridge(void)//R中转 C->E数据
{
	uint16_t i;
	if(CurrentEndDeviceFlag)
	{
		CurrentEndDeviceFlag = 0;
		for(i=0;i<BROCCOLI_CtoEBufSize;i++)
		{
			if(CtoEBuffer[i].length && (memcmp(&CtoEBuffer[i].addr, &CurrentEndDeviceAddr, sizeof(DEVICE_ADDRESS)) == 0))
			{
				pRadioSendBuffer->cmd = BROCCOLI_CMD_CtoE;
				memcpy(&pRadioSendBuffer->dst_addr, &CtoEBuffer[i].addr, sizeof(DEVICE_ADDRESS));
				memcpy(&pRadioSendBuffer->src_addr, &DeviceAddr, sizeof(DEVICE_ADDRESS));
				memcpy(pRadioSendBuffer->payload, CtoEBuffer[i].data, CtoEBuffer[i].length);
				return Broccoli_SendData_Packages(BROCCOLI_SENDFLAG_CtoE, RadioSendBuffer, CtoEBuffer[i].length + sizeof(BASE_PACKAGES));
			}
		}
	}
	return BROCCOLI_OK;
}

int8_t Broccoli_EtoC_Bridge(void)//R中转 E->C数据
{
	uint16_t i;
	for(i=0;i<BROCCOLI_EtoCBufSize;i++)
	{
		if(EtoCBuffer[i].length)
		{
			Broccoli_SendData_EtoC(EtoCBuffer[i].data, EtoCBuffer[i].length);
		}
	}
	return BROCCOLI_OK;
}
#endif

int8_t Broccoli_UpLink(uint8_t *data, uint16_t length)
{
	if((length + sizeof(BASE_PACKAGES)) > MaxDataPackSize) return BROCCOLI_OUTOFDATA;
	if(DeviceType == BROCCOLI_ROUTER) return Broccoli_SendData_RtoC(data, length);
	if(DeviceType == BROCCOLI_ENDDEVICE) return Broccoli_SendData_EtoR(data, length);
	return BROCCOLI_INVOPC;
}

int8_t Broccoli_TopLink(uint8_t *data, uint16_t length)
{
	if((length + sizeof(BASE_PACKAGES)) > MaxDataPackSize) return BROCCOLI_OUTOFDATA;
	if(DeviceType == BROCCOLI_ROUTER) return Broccoli_SendData_RtoC(data, length);
	if(DeviceType == BROCCOLI_ENDDEVICE) return Broccoli_SendData_EtoC(data, length);
	return BROCCOLI_INVOPC;
}

int8_t Broccoli_DownLink(DEVICE_ADDRESS *addr, uint8_t *data, uint16_t length)
{
	if((length + sizeof(BASE_PACKAGES)) > MaxDataPackSize) return BROCCOLI_OUTOFDATA;
	if(DeviceType == BROCCOLI_COORDINATOR) return Broccoli_SendData_CtoR(addr, data, length);
	if(DeviceType == BROCCOLI_ROUTER) return Broccoli_SendData_RtoE(addr, data, length);
	if(DeviceType == BROCCOLI_FREE_ROUTER) return Broccoli_SendData_RtoE(addr, data, length);
	return BROCCOLI_INVOPC;
}

int8_t Broccoli_EndLink(DEVICE_ADDRESS *addrE, DEVICE_ADDRESS *addrR, uint8_t *data, uint16_t length)
{
	if((length + sizeof(BASE_PACKAGES) + sizeof(DEVICE_ADDRESS)) > MaxDataPackSize) return BROCCOLI_OUTOFDATA;
	if(DeviceType == BROCCOLI_COORDINATOR) return Broccoli_SendData_CtoE(addrE, addrR, data, length);
	if(DeviceType == BROCCOLI_FREE_ROUTER) return Broccoli_SendData_RtoE(addrE, data, length);
	return BROCCOLI_INVOPC;
}

static void Broccoli_Process_C(void)//处理协调器接收的数据
{
	BASE_PACKAGES buf;
	switch(pRadioRxBuffer->cmd)
	{
	case BROCCOLI_CMD_C_WHO://搜索信道内协调器
		if((BusStatus & BROCCOLI_STATUS_INIT) == 0) break;
		buf.cmd = BROCCOLI_CMD_NOP;
		Broccoli_SendData((uint8_t *)&buf, 1);
		break;
	case BROCCOLI_CMD_R_REQ://R请求加入C
		if(Broccoli_AllowJoinIn(&pRadioRxBuffer->src_addr))
		{
			buf.cmd = BROCCOLI_CMD_C_ALLOW;
			memcpy(&buf.dst_addr, &pRadioRxBuffer->src_addr, sizeof(DEVICE_ADDRESS));
			memcpy(&buf.src_addr, &DeviceAddr, sizeof(DEVICE_ADDRESS));
			Broccoli_SendData((uint8_t *)&buf, sizeof(BASE_PACKAGES));
		}
		break;
	case BROCCOLI_CMD_RtoC://R->C
		buf.cmd = BROCCOLI_CMD_RtoCOK;
		memcpy(&buf.dst_addr, &pRadioRxBuffer->src_addr, sizeof(DEVICE_ADDRESS));
		memcpy(&buf.src_addr, &DeviceAddr, sizeof(DEVICE_ADDRESS));
		Broccoli_SendData((uint8_t *)&buf, sizeof(BASE_PACKAGES));

		Broccoli_Receive(&pRadioRxBuffer->src_addr, NULL, pRadioRxBuffer->payload, RadioRxLen - sizeof(BASE_PACKAGES));
		break;
	case BROCCOLI_CMD_EtoC://E->C
		buf.cmd = BROCCOLI_CMD_EtoCOK;
		memcpy(&buf.dst_addr, &pRadioRxBuffer->src_addr, sizeof(DEVICE_ADDRESS));
		memcpy(&buf.src_addr, &DeviceAddr, sizeof(DEVICE_ADDRESS));
		Broccoli_SendData((uint8_t *)&buf, sizeof(BASE_PACKAGES));

		Broccoli_Receive((DEVICE_ADDRESS *)pRadioRxBuffer->payload, &pRadioRxBuffer->src_addr, pRadioRxBuffer->payload + sizeof(DEVICE_ADDRESS), RadioRxLen - sizeof(BASE_PACKAGES) - sizeof(DEVICE_ADDRESS));
		break;
	default:
		break;
	}
}

static void Broccoli_Process_R(void)//处理路由器接收的数据
{
#ifdef BROCCOLI_DATARELAYMODE
	uint16_t i;
#endif
	BASE_PACKAGES buf;
	switch(pRadioRxBuffer->cmd)
	{
	case BROCCOLI_CMD_C_WHO://搜索信道内协调器
		if(DeviceType != BROCCOLI_FREE_ROUTER) break;
		if((BusStatus & BROCCOLI_STATUS_INIT) == 0) break;
		buf.cmd = BROCCOLI_CMD_NOP;
		Broccoli_SendData((uint8_t *)&buf, 1);
		break;
	case BROCCOLI_CMD_CtoR://C->R
		buf.cmd = BROCCOLI_CMD_CtoROK;
		memcpy(&buf.dst_addr, &pRadioRxBuffer->src_addr, sizeof(DEVICE_ADDRESS));
		memcpy(&buf.src_addr, &DeviceAddr, sizeof(DEVICE_ADDRESS));
		Broccoli_SendData((uint8_t *)&buf, sizeof(BASE_PACKAGES));

		Broccoli_Receive(NULL, NULL, pRadioRxBuffer->payload, RadioRxLen - sizeof(BASE_PACKAGES));
		break;
	case BROCCOLI_CMD_E_REQ://E请求加入R
		if(Broccoli_AllowJoinIn(&pRadioRxBuffer->src_addr))
		{
			buf.cmd = BROCCOLI_CMD_R_ALLOW;
			memcpy(&buf.dst_addr, &pRadioRxBuffer->src_addr, sizeof(DEVICE_ADDRESS));
			memcpy(&buf.src_addr, &DeviceAddr, sizeof(DEVICE_ADDRESS));
			Broccoli_SendData((uint8_t *)&buf, sizeof(BASE_PACKAGES));
		}
		break;
	case BROCCOLI_CMD_EtoR://E->R
		buf.cmd = BROCCOLI_CMD_EtoROK;
		memcpy(&buf.dst_addr, &pRadioRxBuffer->src_addr, sizeof(DEVICE_ADDRESS));
		memcpy(&buf.src_addr, &DeviceAddr, sizeof(DEVICE_ADDRESS));
		Broccoli_SendData((uint8_t *)&buf, sizeof(BASE_PACKAGES));

		Broccoli_Receive(&pRadioRxBuffer->src_addr, NULL, pRadioRxBuffer->payload, RadioRxLen - sizeof(BASE_PACKAGES));
		break;
	case BROCCOLI_CMD_EtoC://E->C
		buf.cmd = BROCCOLI_CMD_EtoCOK;
		memcpy(&buf.dst_addr, &pRadioRxBuffer->src_addr, sizeof(DEVICE_ADDRESS));
		memcpy(&buf.src_addr, &DeviceAddr, sizeof(DEVICE_ADDRESS));
		Broccoli_SendData((uint8_t *)&buf, sizeof(BASE_PACKAGES));

		if(DeviceType == BROCCOLI_FREE_ROUTER)
		{
			Broccoli_Receive(&pRadioRxBuffer->src_addr, NULL, pRadioRxBuffer->payload, RadioRxLen - sizeof(BASE_PACKAGES));
		}
		else
		{
#ifdef BROCCOLI_DATARELAYMODE
			if(length > sizeof(BASE_PACKAGES))
			{
				for(i=0;i<BROCCOLI_EtoCBufSize;i++)
				{
					if(EtoCBuffer[i].length == 0)
					{
						EtoCBuffer[i].length = length - sizeof(BASE_PACKAGES) + sizeof(DEVICE_ADDRESS);
						memcpy(EtoCBuffer[i].data, &p->src_addr, sizeof(DEVICE_ADDRESS));
						memcpy(EtoCBuffer[i].data + sizeof(DEVICE_ADDRESS), &p->payload, length - sizeof(BASE_PACKAGES));
					}
				}
			}
#endif
		}
		break;
	case BROCCOLI_CMD_CtoE://C->R->E
		buf.cmd = BROCCOLI_CMD_CtoEOK;
		memcpy(&buf.dst_addr, &pRadioRxBuffer->src_addr, sizeof(DEVICE_ADDRESS));
		memcpy(&buf.src_addr, &DeviceAddr, sizeof(DEVICE_ADDRESS));
		Broccoli_SendData((uint8_t *)&buf, sizeof(BASE_PACKAGES));
#ifdef BROCCOLI_DATARELAYMODE
		if(length > (sizeof(DEVICE_ADDRESS) + sizeof(BASE_PACKAGES)))
		{
			for(i=0;i<BROCCOLI_CtoEBufSize;i++)
			{
				if(CtoEBuffer[i].length == 0)
				{
					CtoEBuffer[i].length = length - sizeof(BASE_PACKAGES) - sizeof(DEVICE_ADDRESS);
					memcpy(&CtoEBuffer[i].addr, &p->payload, sizeof(DEVICE_ADDRESS));
					memcpy(CtoEBuffer[i].data, &p->payload + sizeof(DEVICE_ADDRESS), EtoCBuffer[i].length);
				}
			}
		}
#endif
		break;
	default:
		break;
	}
}

static void Broccoli_Process_E(void)//处理终端节点接收的数据
{
	BASE_PACKAGES buf;
	switch(pRadioRxBuffer->cmd)
	{
	case BROCCOLI_CMD_CtoE://C->R->E
		buf.cmd = BROCCOLI_CMD_CtoEOK;
		memcpy(&buf.dst_addr, &pRadioRxBuffer->src_addr, sizeof(DEVICE_ADDRESS));
		memcpy(&buf.src_addr, &DeviceAddr, sizeof(DEVICE_ADDRESS));
		Broccoli_SendData((uint8_t *)&buf, sizeof(BASE_PACKAGES));

		Broccoli_Receive(NULL, NULL, pRadioRxBuffer->payload, RadioRxLen - sizeof(BASE_PACKAGES));
		break;
	case BROCCOLI_CMD_RtoE://R->E
		buf.cmd = BROCCOLI_CMD_RtoEOK;
		memcpy(&buf.dst_addr, &pRadioRxBuffer->src_addr, sizeof(DEVICE_ADDRESS));
		memcpy(&buf.src_addr, &DeviceAddr, sizeof(DEVICE_ADDRESS));
		Broccoli_SendData((uint8_t *)&buf, sizeof(BASE_PACKAGES));

		Broccoli_Receive(NULL, NULL, pRadioRxBuffer->payload, RadioRxLen - sizeof(BASE_PACKAGES));
		break;
	default:
		break;
	}
}

#ifdef BROCCOLI_DATARELAYMODE
void Broccoli_BufInit(void)
{
	uint16_t i;
	for(i=0;i<BROCCOLI_EtoCBufSize;i++) EtoCBuffer[i].length = 0;
	for(i=0;i<BROCCOLI_CtoEBufSize;i++) CtoEBuffer[i].length = 0;
}
#endif
void Broccoli_INIT(uint8_t type)
{
	BASE_PACKAGES buf;
	int32_t i;
	uint16_t j;
	if(type)
	{
		DeviceType = type;
#ifdef BROCCOLI_DATARELAYMODE
		Broccoli_BufInit();
#endif
	}
	BusStatus = BROCCOLI_STATUS_IDLE;
	memset(&BroadCastAddr, 0xFF, sizeof(DEVICE_ADDRESS));
	GetDeviceAddress(&DeviceAddr);
	Radio_SetChannel(CurrentChannel);
	LoadHostAddress(&HostAddress);
	if((DeviceType == BROCCOLI_COORDINATOR) || (DeviceType == BROCCOLI_FREE_ROUTER))//建立网络
	{
		buf.cmd = BROCCOLI_CMD_C_WHO;
		memcpy(&buf.dst_addr, &BroadCastAddr, sizeof(DEVICE_ADDRESS));
		memcpy(&buf.src_addr, &DeviceAddr, sizeof(DEVICE_ADDRESS));
		i = 0;
		while(1)
		{
			BusStatus = BROCCOLI_STATUS_IDLE;
			Broccoli_SendData((uint8_t *)&buf, sizeof(buf));
			//Radio_RXMode();
			for(j=0;j<BROCCOLI_SCAN_TIMEOUT;j++)
			{
				if(BusStatus & BROCCOLI_STATUS_RXMASK)
				{
					CurrentChannel++;
					if(CurrentChannel > 7) CurrentChannel = 0;
					Radio_SetChannel(CurrentChannel);
					break;
				}
			}
			if((BusStatus & BROCCOLI_STATUS_RXMASK) == 0)
			{
				//Radio_RXMode();
				BusStatus = BROCCOLI_STATUS_INIT;
				return;
			}
			i++;
			if(i > 16) break;
		}
		//Radio_RXMode();
		BusStatus = BROCCOLI_STATUS_INIT;
	}
	else //搜索网络
	{
		LoadHostAddress(&HostAddress);
		if(DeviceType == BROCCOLI_ROUTER)
			buf.cmd = BROCCOLI_CMD_R_REQ;
		if(DeviceType == BROCCOLI_ENDDEVICE)
			buf.cmd = BROCCOLI_CMD_E_REQ;
		memcpy(&buf.dst_addr, &HostAddress, sizeof(DEVICE_ADDRESS));
		memcpy(&buf.src_addr, &DeviceAddr, sizeof(DEVICE_ADDRESS));
		while(1)
		{
			Broccoli_SendData((uint8_t *)&buf, sizeof(buf));
			if(DeviceType == BROCCOLI_ROUTER)
			{
				for(j=0;j<BROCCOLI_SCAN_TIMEOUT;j++)
				{
					if(BusStatus & BROCCOLI_STATUS_INIT)
					{
						BusStatus = BROCCOLI_STATUS_INIT;
						return;
					}
					SystemWaitTime();
				}
			}
			else
			{
				for(j=0;j<BROCCOLI_RX_TIMEOUT;j++)
				{
					if(BusStatus & BROCCOLI_STATUS_INIT)
					{
						Radio_SleepMode();
						BusStatus = BROCCOLI_STATUS_INIT;
						return;
					}
					SystemWaitTime();
				}
				Radio_SleepMode();
			}
			CurrentChannel++;
			if(CurrentChannel > 7) CurrentChannel = 0;
			Radio_SetChannel(CurrentChannel);
		}
	}
}

void Broccoli_MainProcess(void)
{
	if(BusStatus & BROCCOLI_STATUS_RXOK)
	{
		BusStatus &= ~BROCCOLI_STATUS_RXMASK;
		switch(DeviceType)
		{
		case BROCCOLI_COORDINATOR:
			Broccoli_Process_C();
			break;
		case BROCCOLI_FREE_ROUTER:
			Broccoli_Process_R();
			break;
		case BROCCOLI_ROUTER:
			Broccoli_Process_R();
			break;
		case BROCCOLI_ENDDEVICE:
			Broccoli_Process_E();
			break;
		default:
			break;
		}
	}
#ifdef BROCCOLI_DATARELAYMODE
	Broccoli_CtoE_Bridge();
	Broccoli_EtoC_Bridge();
#endif
}
