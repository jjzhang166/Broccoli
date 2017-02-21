#include "wzlbus.h"

volatile static uint8_t remaining_bytes,crc_sum,remaining_count;
static const uint8_t bit_masks[9]={0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};
volatile static uint8_t wbus_datbuf[WBUS_BUFFERSIZE];

void wbus_send_head(void)
{
	uart_send_byte(0x80);
	remaining_count = 0;
	crc_sum = 0;
}

void wbus_send_one_byte(uint8_t dat)
{
	uint8_t bitsToSend,countToSend = 7 - remaining_count;
	crc_sum += dat;
	if(remaining_count)
	{
		if(remaining_count == 7)
		{
			uart_send_byte(remaining_bytes & 0x7F);
			countToSend = 7;
			bitsToSend = 0;
		}
		else
		{
			bitsToSend = remaining_bytes << countToSend;
		}
	}
	else
	{
		bitsToSend = 0;
	}
	remaining_count = 8 - countToSend;
	bitsToSend |= (dat >> remaining_count) & bit_masks[countToSend];
	remaining_bytes = dat & bit_masks[remaining_count];
	uart_send_byte(bitsToSend & 0x7F);
}

void wbus_send_the_end(void)
{
	if(remaining_count)
	{
		uart_send_byte((remaining_bytes << (7 - remaining_count)) & 0x7F);
	}
	uart_send_byte(crc_sum & 0x7F);
	uart_send_byte(0xC0);
	remaining_count = 0;
	crc_sum = 0;
}

void wbus_send_bytes(const uint8_t *dat,int count)
{
	int i;
	for(i=0;i<count;i++) wbus_send_one_byte(dat[i]);
}

void wbus_receive(uint8_t c)
{
	static uint16_t count=0,count_h=0,count_l=0;
	static uint8_t crc,lc,tmp;
	switch(c & 0xC0)
	{
	case 0x80:
		count = 0;
		count_h = 0;
		count_l = 0;
		break;
	case 0xC0:
		if(count == 0) lc = 0;
		crc = 0;
		for(count = 0; count < count_l; count++)
		{
			crc += wbus_datbuf[count];
		}
		count = 0;
		if((crc & 0x7F) == lc)
		{
			wbus_datbuf[count_l]=0;
			wbus_process((const uint8_t*)wbus_datbuf, count_l);
		}
		else
		{
			wbus_crc_error();
		}
		break;
	default:
		if(count > 0)
		{
			tmp = (count - 1) % 8;
			if(tmp != 7)
			{
				if(count_h >= WBUS_BUFFERSIZE) return;
				wbus_datbuf[count_h] = (lc << (tmp + 1));
				count_h ++;
			}
			if(tmp != 0)
			{
				if(count_l >= WBUS_BUFFERSIZE) return;
				wbus_datbuf[count_l] |= (lc >> (7 - tmp));
				count_l ++;
			}
		}
		count ++;
		break;
	}
	lc = c;
}
