#ifndef __FIFO_RX_DEFINE_H__
  #define __FIFO_RX_DEFINE_H__

  #define BUFFER_SIZE_FIFO_RX      512
  extern volatile int countFIFO_RX;
  int pushFIFO_RX(unsigned char *data, int length);
  void putchFIFO_RX(unsigned char data);
  int pullFIFO_RX(unsigned char *data, int length);
  int pullFIFO_RX_callback(int (*callback)(unsigned char *, int), int length);
  unsigned char getchFIFO_RX(void);
  int cleanFIFO_RX(void);
  int isEmptyFIFO_RX(void);
  int isFullFIFO_RX(void);
  void processFIFO_RX(void);

#endif //__FIFO_RX_DEFINE_H__
