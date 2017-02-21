#include "sx1278.h"
#include "stm32f0xx.h"
#include "Broccoli.h"

#define RF_MID_BAND_THRESH		433000000
#define RSSI_OFFSET_LF                              -164
#define RSSI_OFFSET_HF                              -157

unsigned long   Frequency = 433000000;
unsigned char   SpreadingFactor = 7;    //7-12
unsigned char   CodingRate = 2;        //1-4
unsigned char   Bw_Frequency = 7;      //6-9
unsigned char   powerValue = 7;

unsigned char   power_data[8]={0X80,0X80,0X80,0X83,0X86,0x89,0x8c,0x8f};
const static uint32_t SX1278_Channels[]={SX1278_Channel0, SX1278_Channel1, SX1278_Channel2, SX1278_Channel3, SX1278_Channel4, SX1278_Channel5, SX1278_Channel6, SX1278_Channel7};

volatile int SX1278_TXBUSY = 0;

__weak void SX1278_RxOpen(void)
{
	//TODO
}

__weak void SX1278_TxOpen(void)
{
	//TODO
}

__weak void SX1278_EnOpen(void)
{
	//TODO
}

__weak void SX1278_EnClose(void)
{
	//TODO
}

__weak unsigned char SX1278_ByteWriteReadfunc(unsigned char out)
{
	//TODO
	return 0xFF;
}

void FSK_SEND_PACKET(void)
{
   SX1278LoRaSetOpMode( Stdby_mode );
   SX1278WriteBuffer(REG_LR_DIOMAPPING1,0x01);
   SX1278WriteBuffer(REG_LR_DIOMAPPING2,0x20);
}
	
void SX1278WriteBuffer( unsigned char addr, unsigned char buffer)
{
   SX1278_EnOpen(); //NSS = 0;
   SX1278_ByteWriteReadfunc( addr | 0x80 );
   SX1278_ByteWriteReadfunc( buffer);
   SX1278_EnClose(); //NSS = 1;
}

unsigned char SX1278ReadBuffer(unsigned char addr)
{
  unsigned char Value;
  SX1278_EnOpen(); //NSS = 0;
  SX1278_ByteWriteReadfunc( addr & 0x7f  );
  Value = SX1278_ByteWriteReadfunc(0xFF);
  SX1278_EnClose();//NSS = 1;

  return Value;
}
	
void SX1278LoRaSetOpMode( RFMode_SET opMode )
{
   unsigned char opModePrev;
   opModePrev=SX1278ReadBuffer(REG_LR_OPMODE);
   opModePrev &=0xf8;
   opModePrev |= (unsigned char)opMode;
   SX1278WriteBuffer( REG_LR_OPMODE, opModePrev);
}

void SX1278LoRaFsk( Debugging_fsk_ook opMode )
{
   unsigned char opModePrev;
   opModePrev=SX1278ReadBuffer(REG_LR_OPMODE);
   opModePrev &=0x7F;
   opModePrev |= (unsigned char)opMode;
   SX1278WriteBuffer( REG_LR_OPMODE, opModePrev);
}
	
void SX1278LoRaSetRFFrequency(unsigned long freq)
{
	unsigned long fq = ((double)freq)/61.035;
	SX1278WriteBuffer( REG_LR_FRFMSB, (fq>>16) & 0xFF);
	SX1278WriteBuffer( REG_LR_FRFMID, (fq>>8) & 0xFF);
	SX1278WriteBuffer( REG_LR_FRFLSB, fq & 0xFF);
}
		
void SX1278LoRaSetRFPower(unsigned char power )
{
   SX1278WriteBuffer( REG_LR_PADAC, 0x87);
   SX1278WriteBuffer( REG_LR_PACONFIG,  power_data[power] );
}
	
void SX1278LoRaSetSpreadingFactor(unsigned char factor )
{
   unsigned char RECVER_DAT;
   SX1278LoRaSetNbTrigPeaks( 3 );
   RECVER_DAT=SX1278ReadBuffer( REG_LR_MODEMCONFIG2);
   RECVER_DAT = ( RECVER_DAT & RFLR_MODEMCONFIG2_SF_MASK ) | ( factor << 4 );
   SX1278WriteBuffer( REG_LR_MODEMCONFIG2, RECVER_DAT );
}
	
void SX1278LoRaSetNbTrigPeaks(unsigned char value )
{
   unsigned char RECVER_DAT;
   RECVER_DAT = SX1278ReadBuffer( 0x31);
   RECVER_DAT = ( RECVER_DAT & 0xF8 ) | value;
   SX1278WriteBuffer( 0x31, RECVER_DAT );
}
	
void SX1278LoRaSetErrorCoding(unsigned char value )
{	
   unsigned char RECVER_DAT;
   RECVER_DAT=SX1278ReadBuffer( REG_LR_MODEMCONFIG1);
   RECVER_DAT = ( RECVER_DAT & RFLR_MODEMCONFIG1_CODINGRATE_MASK ) | ( value << 1 );
   SX1278WriteBuffer( REG_LR_MODEMCONFIG1, RECVER_DAT);
}
	
void SX1278LoRaSetPacketCrcOn(BOOL enable )
{	
   unsigned char RECVER_DAT;
   RECVER_DAT= SX1278ReadBuffer( REG_LR_MODEMCONFIG2);
   RECVER_DAT = ( RECVER_DAT & RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK ) | ( enable << 2 );
   SX1278WriteBuffer( REG_LR_MODEMCONFIG2, RECVER_DAT );
}
	
void SX1278LoRaSetSignalBandwidth(unsigned char bw )
{
   unsigned char RECVER_DAT;
   RECVER_DAT=SX1278ReadBuffer( REG_LR_MODEMCONFIG1);
   RECVER_DAT = ( RECVER_DAT & RFLR_MODEMCONFIG1_BW_MASK ) | ( bw << 4 );
   SX1278WriteBuffer( REG_LR_MODEMCONFIG1, RECVER_DAT );
}
	
void SX1278LoRaSetImplicitHeaderOn(BOOL enable )
{
   unsigned char RECVER_DAT;
   RECVER_DAT=SX1278ReadBuffer( REG_LR_MODEMCONFIG1 );
   RECVER_DAT = ( RECVER_DAT & RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK ) | ( enable );
   SX1278WriteBuffer( REG_LR_MODEMCONFIG1, RECVER_DAT );
}
	
void SX1278LoRaSetSymbTimeout(unsigned int value )
{
   unsigned char RECVER_DAT[2];
   RECVER_DAT[0]=SX1278ReadBuffer( REG_LR_MODEMCONFIG2 );
   RECVER_DAT[1]=SX1278ReadBuffer( REG_LR_SYMBTIMEOUTLSB );
   RECVER_DAT[0] = ( RECVER_DAT[0] & RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK ) | ( ( value >> 8 ) & ~RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK );
   RECVER_DAT[1] = value & 0xFF;
   SX1278WriteBuffer( REG_LR_MODEMCONFIG2, RECVER_DAT[0]);
   SX1278WriteBuffer( REG_LR_SYMBTIMEOUTLSB, RECVER_DAT[1]);
}
	
void SX1278LoRaSetPayloadLength(unsigned char value )
{
   SX1278WriteBuffer( REG_LR_PAYLOADLENGTH, value );
} 
	
void SX1278LoRaSetPreamLength(unsigned int value )
{
   SX1278WriteBuffer( REG_LR_PREAMBLEMSB, (value>>8)&0xFF );
   SX1278WriteBuffer( REG_LR_PREAMBLELSB, value&0xFF );
}
	
void SX1278LoRaSetMobileNode(BOOL enable )
{	 
   unsigned char RECVER_DAT;
   RECVER_DAT=SX1278ReadBuffer( REG_LR_MODEMCONFIG3 );
   RECVER_DAT = ( RECVER_DAT & RFLR_MODEMCONFIG3_MOBILE_NODE_MASK ) | ( enable << 3 );
   SX1278WriteBuffer( REG_LR_MODEMCONFIG3, RECVER_DAT );
}

short SX1278ReadRssi(void)
{
    short rssi = 0;
	if( Frequency > RF_MID_BAND_THRESH )
	{
		rssi = RSSI_OFFSET_HF + SX1278ReadBuffer( REG_LR_RSSIVALUE );
	}
	else
	{
		rssi = RSSI_OFFSET_LF + SX1278ReadBuffer( REG_LR_RSSIVALUE );
	}
    return rssi;
}

void SX1278LORA_INT(void)
{
	SX1278LoRaSetOpMode(Sleep_mode);  //设置睡眠模式
	SX1278LoRaFsk(LORA_mode);	     // 设置扩频模式
	SX1278LoRaSetOpMode(Stdby_mode);  // 设置为普通模式
	SX1278WriteBuffer( REG_LR_DIOMAPPING1, GPIO_VARE_1);
	SX1278WriteBuffer( REG_LR_DIOMAPPING2, GPIO_VARE_2);
	SX1278LoRaSetRFFrequency(Frequency);
	SX1278LoRaSetRFPower(powerValue);
	SX1278LoRaSetSpreadingFactor(SpreadingFactor);	 // 扩频因子设置
	SX1278LoRaSetErrorCoding(CodingRate);		 //有效数据比
	SX1278LoRaSetPacketCrcOn(true);			 //CRC 校验打开
	SX1278LoRaSetSignalBandwidth( Bw_Frequency );	 //设置扩频带宽
	SX1278LoRaSetImplicitHeaderOn(false);		 //同步头是显性模式
	SX1278LoRaSetPayloadLength( 0xFF);
	SX1278LoRaSetSymbTimeout( 0x3FF );
	SX1278LoRaSetMobileNode(true); 			 // 低数据的优化
	SX1278_RxOpen();
}

void Radio_SetChannel(uint8_t ch)
{
	SX1278LoRaSetOpMode(Stdby_mode);  // 设置为普通模式
	SX1278LoRaSetRFFrequency(SX1278_Channels[ch]);
}

void Radio_Send_Package(uint8_t *data, uint16_t length)
{
	static unsigned int i;
	SX1278_TxOpen();
	SX1278LoRaSetOpMode( Stdby_mode );
	SX1278WriteBuffer( REG_LR_HOPPERIOD, 0 );	//不做频率跳变
	SX1278WriteBuffer( REG_LR_IRQFLAGSMASK, IRQN_TXD_Value);	//打开发送中断
	SX1278WriteBuffer( REG_LR_PAYLOADLENGTH, length + 1);	 //最大数据包
	SX1278WriteBuffer( REG_LR_FIFOTXBASEADDR, 0);
	SX1278WriteBuffer( REG_LR_FIFOADDRPTR, 0 );
	SX1278_EnOpen();
	SX1278_ByteWriteReadfunc( 0x80 );
	SX1278_ByteWriteReadfunc( 0xE1 );
	for( i = 0; i < length; i++ ){
	 SX1278_ByteWriteReadfunc( data[i] );
	}
	SX1278_EnClose();
	SX1278WriteBuffer(REG_LR_DIOMAPPING1,0x40);
	SX1278WriteBuffer(REG_LR_DIOMAPPING2,0x00);
	SX1278LoRaSetOpMode( Transmitter_mode );
}


void Radio_RXMode(void)
{
	SX1278LoRaSetOpMode(Stdby_mode);
	SX1278WriteBuffer(REG_LR_IRQFLAGSMASK, IRQN_RXD_Value);  //打开中断
	SX1278WriteBuffer(REG_LR_HOPPERIOD, PACKET_MIAX_Value);
	SX1278WriteBuffer( REG_LR_DIOMAPPING1, 0x00);
	SX1278WriteBuffer( REG_LR_DIOMAPPING2, 0X00);
	SX1278LoRaSetOpMode(Receiver_mode);
	SX1278_RxOpen();
}

void Radio_CADMode(void)
{
	SX1278LoRaSetOpMode( Stdby_mode );
	SX1278WriteBuffer(REG_LR_IRQFLAGSMASK,  IRQN_CAD_Value);	//打开中断
	SX1278WriteBuffer( REG_LR_DIOMAPPING1, 0X80 );
	SX1278WriteBuffer( REG_LR_DIOMAPPING2, 0X00 );
	SX1278LoRaSetOpMode( CAD_mode );
	SX1278_RxOpen();
}

void Radio_SleepMode(void)
{
	SX1278LoRaSetOpMode( Stdby_mode );
	SX1278WriteBuffer(REG_LR_IRQFLAGSMASK,  IRQN_SEELP_Value);  //打开中断
	SX1278WriteBuffer( REG_LR_DIOMAPPING1, 0X00 );
	SX1278WriteBuffer( REG_LR_DIOMAPPING2, 0X00 );
	SX1278LoRaSetOpMode( Sleep_mode );
}

uint16_t Radio_GetRXBuf(uint8_t *data)
{
	int16_t i,length = 0;
	SX1278WriteBuffer(REG_LR_FIFOADDRPTR, 0x00);
	length = SX1278ReadBuffer(REG_LR_NBRXBYTES) - 1;
	if(length > 0)
	{
		SX1278_EnOpen();
		SX1278_ByteWriteReadfunc(0x00);
		if(SX1278_ByteWriteReadfunc(0xFF) == 0xE1)
		{
			for (i = 0; i < length; i++) {
				data[i] = SX1278_ByteWriteReadfunc(0xFF);
			}
		}
		else length = 0;
		SX1278_EnClose();
	}
	else
		return 0;
	return length;
}

void SX1278_Interupt(void) {
	static unsigned char RF_EX0_STATUS;
	static unsigned char CRC_Value;
	RF_EX0_STATUS = SX1278ReadBuffer( REG_LR_IRQFLAGS);
	if ((RF_EX0_STATUS & 0x40) == 0x40) {
		Radio_StartRX();
		CRC_Value = SX1278ReadBuffer( REG_LR_MODEMCONFIG2);
		if ((CRC_Value & 0x04) == 0x04) {
			Radio_RXDone(0);
		} else {
			Radio_RXDone(1);
		}
	} else if ((RF_EX0_STATUS & 0x08) == 0x08) {
		Radio_TXDone(0);
	} else if ((RF_EX0_STATUS & 0x04) == 0x04) {
		if ((RF_EX0_STATUS & 0x01) == 0x01) {
			Radio_RXMode();
		} else {
			Radio_SleepMode();
		}
	} else {
		Radio_RXDone(1);
	}
	SX1278WriteBuffer( REG_LR_IRQFLAGS, 0xff);
}
