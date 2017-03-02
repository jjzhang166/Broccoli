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
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "nrf.h"
#include "nrf_delay.h"

#define PACKET0_S0_SIZE                  (0UL)  //!< S0 size in bits
#define PACKET0_LEN_SIZE								 (8UL)  //!< LENGTH size in bits
#define PACKET0_S1_SIZE                  (0UL)  //!< S1 size in bits
#define PACKET1_BASE_ADDRESS_LENGTH      (4UL)  //!< base address length in bytes
#define PACKET1_MAX_LENGTH               (255UL)  //!< static length in bytes
#define PACKET1_PAYLOAD_SIZE             (255UL)  //!< payload size in bytes

#define RADIO_TXMODE      1
#define RADIO_RXMODE      2
#define RADIO_SLEEPMODE   3

static uint8_t lastmode = 0;

void Radio_ChangeChannel(uint8_t channel)
{
    NRF_RADIO->FREQUENCY = channel;
}

void Radio_TXPower(int8_t po)
{
    NRF_RADIO->TXPOWER = po;
}

void Radio_TXMode(void)
{
    NRF_RADIO->EVENTS_READY = 0U;//收发模式转换完成标志位。复位
    NRF_RADIO->TASKS_TXEN   = 1;//启动无线电为发射模式
    while (NRF_RADIO->EVENTS_READY == 0U){}//等待收发模式转换完成
    lastmode = RADIO_TXMODE;
}

void Radio_RXMode(void)
{
    NRF_RADIO->EVENTS_READY = 0U;//收发模式转换完成标志位。复位
    NRF_RADIO->TASKS_RXEN = 1U;//使能接收
    while(NRF_RADIO->EVENTS_READY == 0U){}//等待接收准备好
    lastmode = RADIO_RXMODE;
}

void Radio_SleepMode(void)
{
    NRF_RADIO->EVENTS_DISABLED = 0U;//无线关闭标志位  复位
    NRF_RADIO->TASKS_DISABLE = 1U;// 关闭无线
    while (NRF_RADIO->EVENTS_DISABLED == 0U){}//等待无线关闭
    lastmode = RADIO_SLEEPMODE;
}

uint8_t Radio_SendPacket(uint32_t addrL, uint8_t addrH, uint8_t *data, uint8_t length, uint32_t timeout)
{
    if(lastmode != RADIO_TXMODE) Radio_TXMode();
    NRF_RADIO->BASE0 = addrL;
    NRF_RADIO->PREFIX0 = addrH;
    NRF_RADIO->PACKETPTR = (uint32_t)data;
    // Packet configuration
    NRF_RADIO->PCNF1 = (RADIO_PCNF1_WHITEEN_Disabled << RADIO_PCNF1_WHITEEN_Pos)    |
                       (RADIO_PCNF1_ENDIAN_Big << RADIO_PCNF1_ENDIAN_Pos)           |
                       (PACKET1_BASE_ADDRESS_LENGTH << RADIO_PCNF1_BALEN_Pos)       |
                       (length << RADIO_PCNF1_STATLEN_Pos)                          |
                       (PACKET1_MAX_LENGTH << RADIO_PCNF1_MAXLEN_Pos);
    NRF_RADIO->EVENTS_END  = 0U;//传输完成标志位，复位
    NRF_RADIO->TASKS_START = 1U;//开始传输
    while (NRF_RADIO->EVENTS_END == 0U)//等待传输完成
    {
        if(timeout == 0) return 0;
        timeout--;
        nrf_delay_ms(1);
    }
    return 1;
}

uint8_t Radio_ReceiveData(uint8_t *data, uint8_t length, uint32_t timeout)
{
    if(lastmode != RADIO_RXMODE) Radio_RXMode();
    NRF_RADIO->PREFIX0 = NRF_FICR->DEVICEADDR[0];  // Prefix byte of addresses 3 to 0
		NRF_RADIO->BASE0   = NRF_FICR->DEVICEADDR[1];  // Base address for prefix 0
    NRF_RADIO->PACKETPTR = (uint32_t)data;
    // Packet configuration
    NRF_RADIO->PCNF1 = (RADIO_PCNF1_WHITEEN_Disabled << RADIO_PCNF1_WHITEEN_Pos)    |
                       (RADIO_PCNF1_ENDIAN_Big << RADIO_PCNF1_ENDIAN_Pos)           |
                       (PACKET1_BASE_ADDRESS_LENGTH << RADIO_PCNF1_BALEN_Pos)       |
                       (length << RADIO_PCNF1_STATLEN_Pos)                          |
                       (PACKET1_MAX_LENGTH << RADIO_PCNF1_MAXLEN_Pos);
		NRF_RADIO->EVENTS_END = 0U;//传输完成标志位，复位
		NRF_RADIO->TASKS_START = 1U;//开始传输
		while(NRF_RADIO->EVENTS_END == 0U)//接收完毕
    {
        if(timeout == 0) return 0;
        timeout--;
        nrf_delay_ms(1);
    }
    if (NRF_RADIO->CRCSTATUS == 1U)//CRC校验通过
    {
        return 1;
    }
    return 0;
}

void Radio_Configure(void)
{
    // Radio config
    NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Pos4dBm << RADIO_TXPOWER_TXPOWER_Pos);
    NRF_RADIO->FREQUENCY = 2UL;                // Frequency bin 2, 2402MHz
    NRF_RADIO->MODE = (RADIO_MODE_MODE_Nrf_1Mbit << RADIO_MODE_MODE_Pos);

    // Radio address config
    NRF_RADIO->PREFIX0 = NRF_FICR->DEVICEADDR[0];  // Prefix byte of addresses 3 to 0
    NRF_RADIO->PREFIX1 = 0xFFFFFFFFUL;  // Prefix byte of addresses 7 to 4
    NRF_RADIO->BASE0   = NRF_FICR->DEVICEADDR[1];  // Base address for prefix 0
    NRF_RADIO->BASE1   = 0xFFFFFFFFUL;  // Base address for prefix 1-7
    NRF_RADIO->TXADDRESS = 0x00UL;      // Set device address 0 to use when transmitting
    NRF_RADIO->RXADDRESSES = 0x11UL;    // Enable device address 0 & 4 to use which receiving

    // Packet configuration
    NRF_RADIO->PCNF0 = (PACKET0_S1_SIZE << RADIO_PCNF0_S1LEN_Pos) |
                       (PACKET0_S0_SIZE << RADIO_PCNF0_S0LEN_Pos) |
                       (PACKET0_LEN_SIZE << RADIO_PCNF0_LFLEN_Pos);

    // Packet configuration
    NRF_RADIO->PCNF1 = (RADIO_PCNF1_WHITEEN_Disabled << RADIO_PCNF1_WHITEEN_Pos)    |
                       (RADIO_PCNF1_ENDIAN_Big << RADIO_PCNF1_ENDIAN_Pos)           |
                       (PACKET1_BASE_ADDRESS_LENGTH << RADIO_PCNF1_BALEN_Pos)       |
                       (PACKET1_PAYLOAD_SIZE << RADIO_PCNF1_STATLEN_Pos)           |
                       (PACKET1_MAX_LENGTH << RADIO_PCNF1_MAXLEN_Pos);

    // CRC Config
    NRF_RADIO->CRCCNF = (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos); // Number of checksum bits
    if ((NRF_RADIO->CRCCNF & RADIO_CRCCNF_LEN_Msk) == (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos))
    {
        NRF_RADIO->CRCINIT = 0xFFFFUL;      // Initial value
        NRF_RADIO->CRCPOLY = 0x11021UL;     // CRC poly: x^16+x^12^x^5+1
    }
    else if ((NRF_RADIO->CRCCNF & RADIO_CRCCNF_LEN_Msk) == (RADIO_CRCCNF_LEN_One << RADIO_CRCCNF_LEN_Pos))
    {
        NRF_RADIO->CRCINIT = 0xFFUL;        // Initial value
        NRF_RADIO->CRCPOLY = 0x107UL;       // CRC poly: x^8+x^2^x^1+1
    }

    nrf_delay_ms(3);
}
