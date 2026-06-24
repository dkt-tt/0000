/*
*Copyright (C),2023 , NANOCHAP
*File name:   ENS1_UART.H          
*Author:      
*Version:     V1.0 
*Date:        2023-11-                  
*Description:  
*Function List: 
1	void UART_Init(CMSDK_UART_TypeDef *CMSDK_UART, UART_InitStructure* uart_paraX);
2	void UART_ITConfig(CMSDK_UART_TypeDef *CMSDK_UART, UART_ITStructure* uart_paraX);
3	unsigned char UartPutc(CMSDK_UART_TypeDef *CMSDK_UART ,unsigned char my_ch);

History: 
1.V1.0
Date:          
Author: 
Modification: 初版
*/
#ifndef _ENS1_UART_H_
#define _ENS1_UART_H_

#include "MY_HEADER.h"
#include "CMSDK_CM0.h"
#include "ENS1_CLOCK.h"
#include "math.h"

#define UART0_RX   GPIO_2
#define UART0_TX   GPIO_3
#define UART0_RTS  GPIO_4
#define UART0_CTS  GPIO_5
#define UART1_RX  GPIO_12
#define UART1_TX  GPIO_13 
#define UART1_RTS  GPIO_14
#define UART1_CTS  GPIO_15 

#define InterruptDisable __disable_irq
#define InterruptEnable __enable_irq
typedef enum uartRcvfifoTrigger{
byte_1=0 , 
bytes_4=1,
bytes_8=2,
bytes_14=3
}uartRcvTrigger;

typedef enum {
OverSamp_16 = 0,
OverSamp_13
}UART_OverSamp;

typedef enum {
StopLen_1 = 0,
StopLen_Other
}UART_StopLen;

typedef enum {
WordLen_5,
WordLen_6,
WordLen_7,
WordLen_8
}UART_WordLen;

typedef enum {
Stick_RESET = 0,
Stick_SET
}UART_Stick_EN;

typedef enum {
Odd_SET = 0,
Even_SET
}UART_Even_EN;

typedef enum {
Parity_RESET = 0,
Parity_SET
}UART_Parity_EN;

/*uart参数设置结构体*/

typedef struct uart_fifo{
	uartRcvTrigger  level;
	bool DMA_Enable  ;
	bool FIFO_Enable ;	
}UART_FifoStructrue;

typedef struct uart_para{
	uint32_t UART_BaudRate;	
	bool UART_HardwareFlowControl;		    //硬件流控是否开启
	UART_FifoStructrue*  FifoSetting ;
	UART_StopLen stop_len;
	UART_WordLen Word_len;
	UART_Stick_EN Stick_EN;
	UART_Even_EN Even_EN;
	UART_Parity_EN Parity_EN;
	
}UART_InitStructure;

extern UART_FifoStructrue UART0_Fifo;
extern UART_InitStructure UART0_Init;
extern UART_FifoStructrue UART1_Fifo;
extern UART_InitStructure UART1_Init;

/*
中断资源说明：
1、THREINT THR发送寄存器或发送FIFO空
2、RDAINT  接收寄存器数据可用 ，FIFO模式下触发接收阈值
3、RTOINT  FIFO模式下接收超时：在最后四个字节时间内没有字节移除或者输入，在这个时间内至少有一个字节在接收FIFO中
4、RLSINT  接收线路状态：发生了溢出错误，校验错误，帧错误或者中断
5、MSIINT  调制解调器状态：CTS更改状态（禁用autoflow），DSR/RI/DCD更改状态
*/
/*中断使能*/
#define  MSI_EN   (uint8_t)0X8    
#define  RLSI_EN  (uint8_t)0X4  
#define  THRE_EN  (uint8_t)0X2 
#define  RDAI_EN  (uint8_t)0X1
/*中断信息*/
#define INT_MODEM_STATUS  (uint8_t)0X0
#define INT_THR_EMPTY     (uint8_t)0X1
#define INT_RCV_DATA_AVAILABLE   (uint8_t)0X2
#define INT_RCV_LINE_STATUS    (uint8_t)0X3
#define INT_CHAR_TIMEOUT_INDICATION  (uint8_t)0X6

typedef struct uart_it_para{
	uint8_t UartIntModel ;   //选择中断的模式
	uint8_t UartDMAEnable;   //选择是否开启DMA
}UART_ITStructure;

extern UART_ITStructure UART0_ITSet;
extern UART_ITStructure UART1_ITSet;


/*中断信息读取*/
/*中断挂起*/
uint8_t UART_INT_PEND(CMSDK_UART_TypeDef* UARTx);  //为0时有UART的中断挂起 
/*中断状态判断*/
uint8_t UART_INT_TYPE(CMSDK_UART_TypeDef* UARTx);
/*fifo 使用指示器*/
uint8_t UART_FIFO_USE(CMSDK_UART_TypeDef* UARTx);  //0:非fifo模式  1：fifo 使能
/*清除传输FIFO*/
void UART_TXCLR(CMSDK_UART_TypeDef* UARTx);
/*清除接收FIFO*/
void UART_RXCLR(CMSDK_UART_TypeDef* UARTx);
/*FIFO使能*/
void UART_FIFOEnable(CMSDK_UART_TypeDef* UARTx);
/*FIFO关闭*/
void UART_FIFODisable(CMSDK_UART_TypeDef* UARTx);

/*接收数据*/
uint8_t READ_UART_RCVBuff(CMSDK_UART_TypeDef* UARTx );
/*发送数据*/
void WRITE_UART_THRBuff(CMSDK_UART_TypeDef* UARTx ,uint8_t data);

/*FIFO状态获取*/
uint8_t UART_RX_FIFO_LEN(CMSDK_UART_TypeDef* UARTx) ;
uint8_t UART_TX_FIFO_LEN(CMSDK_UART_TypeDef* UARTx)  ;
uint8_t UART_RX_FIFO_FULL(CMSDK_UART_TypeDef* UARTx) ;	
uint8_t UART_RX_FIFO_EMPTY(CMSDK_UART_TypeDef* UARTx) ;
uint8_t UART_TX_FIFO_FULL(CMSDK_UART_TypeDef* UARTx)  ;
uint8_t UART_TX_FIFO_EMPTY(CMSDK_UART_TypeDef* UARTx) ;

/*收发线状态获取*/
uint8_t UARTLine_RCVError(CMSDK_UART_TypeDef* UARTx);

/*判断发送缓存是否为空*/
uint8_t UARTLine_TRANSEmpty(CMSDK_UART_TypeDef* UARTx);

/*判断THR是否为空*/
uint8_t UARTLine_THREmpty(CMSDK_UART_TypeDef* UARTx);

/*UART初始化*/
void UART_Init(CMSDK_UART_TypeDef *CMSDK_UART, UART_InitStructure* uart_paraX);
void UART_ITConfig(CMSDK_UART_TypeDef *CMSDK_UART, UART_ITStructure* uart_paraX);
unsigned char UartPutc2(CMSDK_UART_TypeDef *CMSDK_UART ,unsigned char my_ch);
void Uart_Send(CMSDK_UART_TypeDef *CMSDK_UART ,uint8_t *data, uint8_t len);
void ParseCompletePacket(uint8_t *packet, uint8_t length);
void CheckCompletePacket(void);
void ResetPacketBuffer(void);
void ProcessUARTData(void);


#endif

