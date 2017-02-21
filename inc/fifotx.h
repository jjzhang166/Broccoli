#ifndef __FIFO_TX_DEFINE_H__
  #define __FIFO_TX_DEFINE_H__

  #define BUFFER_SIZE_FIFO_TX      512
  extern volatile int countFIFO_TX;
  int pushFIFO_TX(unsigned char *data, int length);
  void putchFIFO_TX(unsigned char data);
  int pullFIFO_TX(unsigned char *data, int length);
  int pullFIFO_TX_callback(int (*callback)(unsigned char *, int), int length);
  unsigned char getchFIFO_TX(void);
  int cleanFIFO_TX(void);
  int isEmptyFIFO_TX(void);
  int isFullFIFO_TX(void);
  void processFIFO_TX(void);

#endif //__FIFO_TX_DEFINE_H__
