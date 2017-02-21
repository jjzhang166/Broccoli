#ifndef __WZL_BROCCOLI_H__
#define __WZL_BROCCOLI_H__

#include <stdint.h>
#include <string.h>

#if  defined ( __GNUC__ )
  #ifndef __weak
    #define __weak   __attribute__((weak))
  #endif /* __weak */
  #ifndef __packed
    #define __packed __attribute__((__packed__))
  #endif /* __packed */
#endif /* __GNUC__ */

//基础设置定义
#define MaxDataPackSize			255
#define BROCCOLI_TX_TIMEOUT		100
#define BROCCOLI_RX_TIMEOUT		100
#define BROCCOLI_SCAN_TIMEOUT	1000
#define	BROCCOLI_EtoCBufSize	10
#define	BROCCOLI_CtoEBufSize	10
//#define BROCCOLI_DATARELAYMODE

//设备类型
#define BROCCOLI_COORDINATOR		0x01//协调器
#define BROCCOLI_ROUTER				0x02//路由器
#define BROCCOLI_FREE_ROUTER		0x03//独立工作路由器
#define BROCCOLI_ENDDEVICE			0x04//终端设备

//总线状态
#define BROCCOLI_STATUS_IDLE	0x00//空闲状态
#define BROCCOLI_STATUS_INIT	0x80//总线初始化状态
#define BROCCOLI_STATUS_TXBUSY	0x01//发送忙
#define BROCCOLI_STATUS_TXOK	0x02//发送完成
#define BROCCOLI_STATUS_TXERR	0x04//发送错误
#define BROCCOLI_STATUS_RXBUSY	0x08//接收忙
#define BROCCOLI_STATUS_RXOK	0x10//接收完成
#define BROCCOLI_STATUS_RXERR	0x20//接收错误
#define BROCCOLI_STATUS_TXMASK	(BROCCOLI_STATUS_TXBUSY | BROCCOLI_STATUS_TXOK | BROCCOLI_STATUS_TXERR)
#define BROCCOLI_STATUS_RXMASK	(BROCCOLI_STATUS_RXBUSY | BROCCOLI_STATUS_RXOK | BROCCOLI_STATUS_RXERR)
#define BROCCOLI_STATUS_SNTX	(BROCCOLI_STATUS_TXBUSY | BROCCOLI_STATUS_RXBUSY)//不能发送数据状态

//指令
#define BROCCOLI_CMD_NOP		0x00//空指令
#define BROCCOLI_CMD_C_WHO		0x01//搜索信道内协调器
#define BROCCOLI_CMD_C_ALLOW	0x02//C允许R加入
#define BROCCOLI_CMD_CtoR		0x03//C->R
#define BROCCOLI_CMD_CtoROK		0x04//C->R OK
#define BROCCOLI_CMD_CtoE		0x05//C->R->E
#define BROCCOLI_CMD_CtoEOK		0x06//C->R->E OK
#define BROCCOLI_CMD_R_REQ		0x10//R请求加入C
#define BROCCOLI_CMD_R_ALLOW	0x11//R允许E加入
#define BROCCOLI_CMD_RtoC		0x12//R->C
#define BROCCOLI_CMD_RtoCOK		0x13//R->C OK
#define BROCCOLI_CMD_RtoE		0x14//R->E
#define BROCCOLI_CMD_RtoEOK		0x15//R->E OK
#define BROCCOLI_CMD_E_REQ		0x20//E请求加入R
#define BROCCOLI_CMD_EtoC		0x21//E->C
#define BROCCOLI_CMD_EtoCOK		0x22//E->C OK
#define BROCCOLI_CMD_EtoR		0x23//E->R
#define BROCCOLI_CMD_EtoROK		0x24//E->R OK

//发送标志
#define BROCCOLI_SENDFLAG_CtoR	0x01//C->R OK
#define BROCCOLI_SENDFLAG_CtoE	0x02//C->E OK
#define BROCCOLI_SENDFLAG_RtoC	0x04//R->C OK
#define BROCCOLI_SENDFLAG_RtoE	0x08//R->E OK
#define BROCCOLI_SENDFLAG_EtoC	0x10//E->C OK
#define BROCCOLI_SENDFLAG_EtoR	0x20//E->R OK

//发送指令返回值
#define BROCCOLI_OK			0//发送完成
#define BROCCOLI_INVOPC		-1//无效操作
#define BROCCOLI_CONBRK		-2//连接断开
#define BROCCOLI_OUTOFDATA	-3//数据太多无法发送

#pragma pack (1)
typedef struct {
	uint8_t addr[8];
} DEVICE_ADDRESS;

typedef struct {
	uint8_t cmd;
	DEVICE_ADDRESS dst_addr;
	DEVICE_ADDRESS src_addr;
	uint8_t payload[];
} BASE_PACKAGES;

typedef struct{
	uint16_t length;
	uint8_t data[MaxDataPackSize - sizeof(BASE_PACKAGES) + sizeof(DEVICE_ADDRESS)];
}ETOCBUFFER;

typedef struct{
	uint16_t length;
	DEVICE_ADDRESS addr;
	uint8_t data[MaxDataPackSize - sizeof(BASE_PACKAGES)];
}CTOEBUFFER;
#pragma pack ()

void Radio_StartRX(void);//开始无线接收
void Radio_RXDone(uint8_t err);//无线接收完成,err：1有错误，0没错误
void Radio_TXDone(uint8_t err);//无线发送数据结束,err：1有错误，0没错误
void Radio_RXCAD(uint8_t get);

void Broccoli_INIT(uint8_t type);//初始化
void Broccoli_MainProcess(void);//主过程

int8_t Broccoli_UpLink(uint8_t *data, uint16_t length);//向上层设备传送数据
int8_t Broccoli_TopLink(uint8_t *data, uint16_t length);//向顶层设备传送数据
int8_t Broccoli_DownLink(DEVICE_ADDRESS *addr, uint8_t *data, uint16_t length);//向下层设备传送数据
int8_t Broccoli_EndLink(DEVICE_ADDRESS *addrE, DEVICE_ADDRESS *addrR, uint8_t *data, uint16_t length);//向底层设备传送数据

#endif//__WZL_BROCCOLI_H__
