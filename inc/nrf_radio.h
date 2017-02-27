#ifndef __WZL_NRF_RADIO_H__
#define __WZL_NRF_RADIO_H__
    #include <stdint.h>
    #include <stdbool.h>
    #include <string.h>

    #define PACKET0_S1_SIZE                  (0UL)  //!< S1 size in bits
    #define PACKET0_S0_SIZE                  (0UL)  //!< S0 size in bits
    #define PACKET0_PAYLOAD_SIZE             (0UL)  //!< payload size in bits
    #define PACKET1_BASE_ADDRESS_LENGTH      (4UL)  //!< base address length in bytes
    #define PACKET1_STATIC_LENGTH            (255UL)  //!< static length in bytes
    #define PACKET1_PAYLOAD_SIZE             (255UL)  //!< payload size in bytes

    #define RADIO_TXMODE      1
    #define RADIO_RXMODE      2
    #define RADIO_SLEEPMODE   3

    void Radio_ChangeChannel(uint8_t channel);
    void Radio_TXPower(int8_t po);
    void Radio_TXMode(void);
    void Radio_RXMode(void);
    void Radio_SleepMode(void);
    uint8_t Radio_SendPacket(uint32_t addrL, uint32_t addrH, uint8_t *data, uint8_t length, uint32_t timeout);
    uint8_t Radio_ReceiveData(uint8_t *data, uint8_t length, uint32_t timeout);
    void Radio_Configure(void);
#endif //__WZL_NRF_RADIO_H__
