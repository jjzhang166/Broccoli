#include <fifotx.h>
volatile unsigned char bufferFIFO_TX[BUFFER_SIZE_FIFO_TX];
volatile int countFIFO_TX = 0;
volatile int inFIFO_TX = 0;
volatile int outFIFO_TX = 0;

int pushFIFO_TX(unsigned char *data, int length)
{
  int i,ret = 0;
  int len;
  if((BUFFER_SIZE_FIFO_TX - countFIFO_TX - 1) < length)
  {
    len = (BUFFER_SIZE_FIFO_TX - countFIFO_TX - 1);
    ret = len - length;
  }
  else
  {
    len = length;
  }
  for(i=0;i<len;i++)
  {
    bufferFIFO_TX[(inFIFO_TX+i)%BUFFER_SIZE_FIFO_TX] = data[i];
  }
  countFIFO_TX += len;
  inFIFO_TX = (inFIFO_TX + len) % BUFFER_SIZE_FIFO_TX;
  return ret;
}

void putchFIFO_TX(unsigned char data)
{
    if(countFIFO_TX >= BUFFER_SIZE_FIFO_TX) return;
    bufferFIFO_TX[inFIFO_TX] = data;
  countFIFO_TX++;
  inFIFO_TX = (inFIFO_TX + 1) % BUFFER_SIZE_FIFO_TX;
}

int pullFIFO_TX(unsigned char *data, int length)
{
  int i,len;
  if(length > countFIFO_TX)
  {
    len = countFIFO_TX;
  }
  else
  {
    len = length;
  }
  for(i=0;i<len;i++)
  {
    data[i] = bufferFIFO_TX[(outFIFO_TX+i)%BUFFER_SIZE_FIFO_TX];
  }
  countFIFO_TX -= len;
  outFIFO_TX = (outFIFO_TX + len) % BUFFER_SIZE_FIFO_TX;
  return len;
}

int pullFIFO_TX_callback(int (*callback)(unsigned char *, int), int length)
{
    int i,len;
    if(length > countFIFO_TX)
    {
        len = countFIFO_TX;
    }
    else
    {
        len = length;
    }
    len = callback(bufferFIFO_TX + outFIFO_TX, len);
    if(len > 0)
    {
        countFIFO_TX -= len;
        outFIFO_TX = (outFIFO_TX + len) % BUFFER_SIZE_FIFO_TX;
    }
    return len;
}

unsigned char getchFIFO_TX(void)
{
  unsigned char data = 0;
  data = bufferFIFO_TX[outFIFO_TX];
  countFIFO_TX --;
  outFIFO_TX = (outFIFO_TX + 1) % BUFFER_SIZE_FIFO_TX;
  return data;
}

int cleanFIFO_TX(void)
{
  inFIFO_TX = 0;
  outFIFO_TX = 0;
  return 0;
}

int isEmptyFIFO_TX(void)
{
  if(countFIFO_TX > 0) return 0;
  else return 1;
}

int isFullFIFO_TX(void)
{
  if(countFIFO_TX < BUFFER_SIZE_FIFO_TX) return 0;
  else return 1;
}

void processFIFO_TX(void)
{
	  static unsigned char data = 0;
	  while(countFIFO_TX)
	  {
		  data = bufferFIFO_TX[outFIFO_TX];
		  countFIFO_TX --;
		  outFIFO_TX = (outFIFO_TX + 1) % BUFFER_SIZE_FIFO_TX;
		  putHEX(data);putch(' ');
		  if((data & 0xC0) == 0xC0) break;
	  }
}
