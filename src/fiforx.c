#include <fiforx.h>
#include "wzlbus.h"
#include "sx1278.h"
volatile unsigned char bufferFIFO_RX[BUFFER_SIZE_FIFO_RX];
volatile int countFIFO_RX = 0;
volatile int inFIFO_RX = 0;
volatile int outFIFO_RX = 0;

int pushFIFO_RX(unsigned char *data, int length)
{
  int i,ret = 0;
  int len;
  if((BUFFER_SIZE_FIFO_RX - countFIFO_RX - 1) < length)
  {
    len = (BUFFER_SIZE_FIFO_RX - countFIFO_RX - 1);
    ret = len - length;
  }
  else
  {
    len = length;
  }
  for(i=0;i<len;i++)
  {
    bufferFIFO_RX[(inFIFO_RX+i)%BUFFER_SIZE_FIFO_RX] = data[i];
  }
  countFIFO_RX += len;
  inFIFO_RX = (inFIFO_RX + len) % BUFFER_SIZE_FIFO_RX;
  return ret;
}

void putchFIFO_RX(unsigned char data)
{
    if(countFIFO_RX >= BUFFER_SIZE_FIFO_RX) return;
    bufferFIFO_RX[inFIFO_RX] = data;
	countFIFO_RX++;
	inFIFO_RX = (inFIFO_RX + 1) % BUFFER_SIZE_FIFO_RX;
}

int pullFIFO_RX(unsigned char *data, int length)
{
  int i,len;
  if(length > countFIFO_RX)
  {
    len = countFIFO_RX;
  }
  else
  {
    len = length;
  }
  for(i=0;i<len;i++)
  {
    data[i] = bufferFIFO_RX[(outFIFO_RX+i)%BUFFER_SIZE_FIFO_RX];
  }
  countFIFO_RX -= len;
  outFIFO_RX = (outFIFO_RX + len) % BUFFER_SIZE_FIFO_RX;
  return len;
}

int pullFIFO_RX_callback(int (*callback)(unsigned char *, int), int length)
{
    int i,len;
    if(length > countFIFO_RX)
    {
        len = countFIFO_RX;
    }
    else
    {
        len = length;
    }
    len = callback(bufferFIFO_RX + outFIFO_RX, len);
    if(len > 0)
    {
        countFIFO_RX -= len;
        outFIFO_RX = (outFIFO_RX + len) % BUFFER_SIZE_FIFO_RX;
    }
    return len;
}

unsigned char getchFIFO_RX(void)
{
  unsigned char data = 0;
  data = bufferFIFO_RX[outFIFO_RX];
  countFIFO_RX --;
  outFIFO_RX = (outFIFO_RX + 1) % BUFFER_SIZE_FIFO_RX;
	return data;
}

int cleanFIFO_RX(void)
{
  inFIFO_RX = 0;
  outFIFO_RX = 0;
  return 0;
}

int isEmptyFIFO_RX(void)
{
	if(countFIFO_RX > 0) return 0;
	else return 1;
}

int isFullFIFO_RX(void)
{
	if(countFIFO_RX < BUFFER_SIZE_FIFO_RX) return 0;
	else return 1;
}

void processFIFO_RX(void)
{
	  unsigned char data = 0;
	  if(SX1278_TXBUSY) return;
	  while(countFIFO_RX)
	  {
		  data = bufferFIFO_RX[outFIFO_RX];
		  countFIFO_RX --;
		  outFIFO_RX = (outFIFO_RX + 1) % BUFFER_SIZE_FIFO_RX;
		  wbus_receive(data);
		  if((data & 0xC0) == 0xC0) break;
	  }
}
