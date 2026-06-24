/*
*Copyright (C),2023 , NANOCHAP
*File name:   ENS1_UART.C          
*Author:      
*Version:     V1.0 
*Date:        2023-11-                  
*Description:  
*Function List: 

History: 
1.V1.0
Date:          
Author: 
Modification: 初版
*/

/*
ENS1 uart特性说明
1 符合AMBA APB规范
2 最高115200bps波特率(可以有更高的设置)
3 发送 接收 分离FIFO （16字节）
4 支持自动流控
5 标准异步通信位(开始、停止、奇偶校验)
6 DMA
7 支持回环测试
8 中断
9 全可编程串行接口特点:
  数据位可设置： 5，6，7，8
  偶、奇或非奇偶位的生成和检测
  可产生1、1.5或2位停止位

*/
#include "my_header.h"
#include "ENS1_UART.h"
#include "ENS1_GPIO.h"
#include <string.h>
#include <stdint.h>
#include <stddef.h>
UART_FifoStructrue UART1_Fifo = {
	.level = bytes_8 ,
	.DMA_Enable = false ,
	.FIFO_Enable = true ,
};
UART_InitStructure UART1_Init = {
	.UART_BaudRate = 110592,        //计算出来的DLL DLH不为整数， 1、需要校准RC精确度，2、需要调整此处的波特率
	.UART_HardwareFlowControl = false,
	.FifoSetting = 	&UART1_Fifo ,	
	.stop_len = StopLen_1,		//固定停止位1位
	.Word_len = WordLen_8,		//数据位8位
	.Stick_EN = Stick_RESET,	//禁用固定奇偶校验
	.Even_EN = Even_SET,		//奇偶校验选择
	.Parity_EN = Parity_RESET,	//禁用奇偶检验
};

UART_ITStructure UART1_ITSet = {
	.UartIntModel = RLSI_EN | RDAI_EN ,
};

/*判断是否有中断挂起*/
uint8_t UART_INT_PEND(CMSDK_UART_TypeDef* UARTx)  //为0时有UART的中断挂起 
{
	return (uint8_t)(UARTx->IIR & 0x1);
}
/*中断类型判断*/
uint8_t UART_INT_TYPE(CMSDK_UART_TypeDef* UARTx)
{
	return (uint8_t)((UARTx->IIR >> 1) & 0x7 );
}
/*fifo 使用指示器*/
uint8_t UART_FIFO_USE(CMSDK_UART_TypeDef* UARTx)  //0:非fifo模式  1：fifo 使能
{
	return (uint8_t)((UARTx->IIR >> 6) & 0x3);
}

/*清除传输FIFO*/
void UART_TXCLR(CMSDK_UART_TypeDef* UARTx)
{
	UARTx->FCR |= (1<<2) ;
}
/*清除接收FIFO*/
void UART_RXCLR(CMSDK_UART_TypeDef* UARTx)
{
	UARTx->FCR |= (1<<1);
}
/*FIFO使能*/
void UART_FIFOEnable(CMSDK_UART_TypeDef* UARTx)
{
	UARTx->FCR |= (1);
}
/*FIFO关闭*/
void UART_FIFODisable(CMSDK_UART_TypeDef* UARTx)
{
	UARTx->FCR &=~ 1;
}

/*接收数据*/
uint8_t READ_UART_RCVBuff(CMSDK_UART_TypeDef* UARTx )
{
	return (uint8_t)(UARTx->RBR & 0xff);
}

/*发送数据*/
void WRITE_UART_THRBuff(CMSDK_UART_TypeDef* UARTx ,uint8_t data)
{
	UARTx->THR = data;
}

/*FIFO 状态读取*/
uint8_t UART_RX_FIFO_LEN(CMSDK_UART_TypeDef* UARTx)  
{
	return (uint8_t)((UARTx->FSR & 0x001f0000)>>16);        //读取当前接收FIFO数据长度
}

uint8_t UART_TX_FIFO_LEN(CMSDK_UART_TypeDef* UARTx)    
{
	return (uint8_t)((UARTx->FSR & 0x00001f00)>>8);	       //读取当前发送FIFO数据长度
}	

uint8_t  UART_RX_FIFO_FULL(CMSDK_UART_TypeDef* UARTx) 
{
	return 	(uint8_t)((((UARTx->FSR & 0x8)>> 3 )==1) ?  (1) : (0)) ;          //当前读取FIFO是否为满？
}	

uint8_t UART_RX_FIFO_EMPTY(CMSDK_UART_TypeDef* UARTx) 
{	
	return 	(uint8_t)((((UARTx->FSR & 0x4) >> 2)==1) ?    (1) : (0)  ) ;          //当前读取FIFO是否为空？
}	

uint8_t UART_TX_FIFO_FULL(CMSDK_UART_TypeDef* UARTx)  
{
	return 	(uint8_t)((((UARTx->FSR & 0x2) >> 1)==1) ?  (1) : (0)) ;         //当前发送FIFO是否为满？
}	

uint8_t UART_TX_FIFO_EMPTY(CMSDK_UART_TypeDef* UARTx)  
{
	return (uint8_t)(((UARTx->FSR & 0x1)==1) ? (1) : (0)) ;          //当前发送FIFO是否为空？
}


/*收发线状态获取*/
uint8_t UARTLine_RCVError(CMSDK_UART_TypeDef* UARTx)
{
	return (uint8_t)((UARTx->LSR>>7) & 0x1);      //返回0 没有错误， 返回1 在fifo和非fifo状态下都有一个校验/帧/或接收缓存或fifo 的中断指示（默认不使能break）
}

/*判断发送缓存是否为空*/
uint8_t UARTLine_TRANSEmpty(CMSDK_UART_TypeDef* UARTx)
{
	return (uint8_t)((UARTx->LSR>>6) & 0x1) ;
}

/*判断THR是否为空*/
uint8_t UARTLine_THREmpty(CMSDK_UART_TypeDef* UARTx)
{
	return (uint8_t)((UARTx->LSR>>5) & 0x1) ;
}

/*
初始化UART需要以下步骤:
1. 执行必要的设备引脚多路复用设置。
2. 通过将适当的时钟除数值写入除数锁存寄存器(DLL和DLH)来设置所需的波特率。
3. 如果要使用FIFO，请选择所需的触发等级，并通过将适当的值写入(FCR)来启用FIFO。
   在FCR寄存器的 FIFOEN 位需首先配置。
4. 通过向行控制寄存器(LCR)写入适当的值来选择所需的协议设置。
5. 如果需要自动流量控制，则将适当的值写入调制解调器控制寄存器(MCR)。
   请注意，并非所有uart都支持自动流量控制。
6. 对挂起事件的选择所需响应通过配置FREE位，
   通过在(PMU)寄存器中设置TXRST和RXRST位来释放位并使能UART
*/


//uart初始化，参数1：uart0/1，参数2：uart参数设置结构体
void UART_Init(CMSDK_UART_TypeDef *CMSDK_UART, UART_InitStructure* uart_paraX){
	uint32_t baudRate;
	uint16_t divisor_value;
	uint8_t overSamp_mode;
	/* 1 GPIO - alt_function*/
	if(CMSDK_UART == CMSDK_UART0){
		PCLK_Enable(UART0_PCLK_EN);	
		GPIO_AltFunction(UART0_RX , ALT_FUNC1);   //rx
		GPIO_AltFunction(UART0_TX , ALT_FUNC1);   //tx
	}
	else if(CMSDK_UART == CMSDK_UART1)
	{
		PCLK_Enable(UART1_PCLK_EN);	
		GPIO_AltFunction(UART1_RX , ALT_FUNC1);  //RX
		GPIO_AltFunction(UART1_TX , ALT_FUNC1);  //TX
	}
	/* 2 波特率设置，过采样模式选择，无需自行配置，使用默认即可*/
	if(((CMSDK_UART->MDR)&0x00000001) == 0)
	{
		overSamp_mode = 16;  //默认0  ，期望波特率=输入时钟频率/(({DLH，DLL}+1) × 16)
	}
	else
	{
		overSamp_mode = 13;		//期望波特率=输入时钟频率/(({DLH，DLL}+1) × 13)
	}
	
	//波特率校准，反推处DLL和DLH，（外设时钟主频/设置波特率/过采样模式）-1，
	baudRate = 38400;
	divisor_value = (uint16_t)(uint32_t)((32000000/baudRate/16)-1);
	CMSDK_UART1->DLL = (uint8_t)(divisor_value & 0x00FF);
	CMSDK_UART1->DLH = (uint8_t)((divisor_value & 0xFF00) >> 8);
//	CMSDK_GPIO->ALTFL = (1<<26);
	
//	divisor_value = (uint16_t)((uint32_t)(APB_Clock_Freq / uart_paraX->UART_BaudRate / overSamp_mode) - 1);
//	CMSDK_UART->DLL = (uint8_t)(divisor_value & 0x00FF);
//	CMSDK_UART->DLH = (uint8_t)((divisor_value & 0xFF00)>>8);
	
	/*FCR配置 FIFO control*/
	CMSDK_UART->FCR |= (uart_paraX->FifoSetting->level <<6);//接收多少字节后触发中断
	if(uart_paraX->FifoSetting->FIFO_Enable == true)
	{
		CMSDK_UART->FCR |= (1);
	}
	//配置停止位
	CMSDK_UART->LCR &= ~(0x01 << 2);
	CMSDK_UART->LCR |= uart_paraX->stop_len<<2;
	
	//配置数据位
	CMSDK_UART->LCR &= ~(0x03 << 0);
	CMSDK_UART->LCR |= uart_paraX->Word_len<<0;
	
	//配置是否启用校验位
	CMSDK_UART->LCR &= ~(0x01 << 3);
	CMSDK_UART->LCR |= uart_paraX->Parity_EN<<3;
	
	//启用校验位
	if(uart_paraX->Parity_EN==Parity_SET)
	{
		CMSDK_UART->LCR &= ~(0x03 << 4);
		CMSDK_UART->LCR |= uart_paraX->Even_EN<<4;
		CMSDK_UART->LCR |= uart_paraX->Stick_EN<<5;
	}
	
	/*MCR自动流控配置*/
	if(uart_paraX->UART_HardwareFlowControl == true)
	{
		CMSDK_UART->MCR |= (1<<5);
	}

	/*电源管理寄存器PMU 的TXRST和RXRST设置,不需要设置*/
}

/*UART 的中断设置*/
void UART_ITConfig(CMSDK_UART_TypeDef *CMSDK_UART, UART_ITStructure* uart_paraX)
{
	if(CMSDK_UART == CMSDK_UART1){
		NVIC_DisableIRQ(UART1_IRQn);//Disable NVIC interrupt
		NVIC_ClearPendingIRQ(UART1_IRQn);//Clear Pending NVINVIC_EnableIRQ(UART1_IRQn);//Enable NVIC interrupt	
	}
	else if(CMSDK_UART == CMSDK_UART0){
		NVIC_DisableIRQ(UART0_IRQn);//Disable NVIC interrupt
		NVIC_ClearPendingIRQ(UART0_IRQn);//Clear Pending NVIC interrupt
	}
	/*IER配置，中断使能寄存器*/
	CMSDK_UART->IER |= (uart_paraX->UartIntModel);
	
	if(CMSDK_UART == CMSDK_UART1){
		NVIC_EnableIRQ(UART1_IRQn);
	}
	else if(CMSDK_UART == CMSDK_UART0){
		NVIC_EnableIRQ(UART0_IRQn);
	}
}


unsigned char UartPutc2(CMSDK_UART_TypeDef *CMSDK_UART ,unsigned char my_ch)
{
	while (UARTLine_THREmpty(CMSDK_UART) == 0x0); 
	WRITE_UART_THRBuff(CMSDK_UART,my_ch);
	return (my_ch);
}
//串口发送一定字节的数据
void Uart_Send(CMSDK_UART_TypeDef *CMSDK_UART ,uint8_t *data, uint8_t len)
{
	uint8_t t;
	for(t=0;t<len;t++)		//循环发送数据
	{
		while(UARTLine_THREmpty(CMSDK_UART) == 0x0);//等待发送结束		  
		WRITE_UART_THRBuff(CMSDK_UART,data[t]);
	}	 
	while(UARTLine_THREmpty(CMSDK_UART) == 0x0);//等待发送结束
	
}


/*
中断与中断状态清除
1   接收线状态：INT_RCV_LINE_STATUS
    溢出、校验、帧错误或检测到break
    中断清除方式：对于溢出错误，读取LSR 可清除中断，对于校验、帧错误或break，中断需要等待所有的错误数据被读取后才被清除
2   接收数据就绪： INT_RCV_DATA_AVAILABLE
    非FIFO模式下，接收数据准备就绪， fifo模式下，达到触发阈值，如果四个字节时间内没有访问FIFO ,就再次触发
    中断清除方式：非FIFO模式下，RBR被读取 ，  FIFO模式下，FIFO低于触发阈值被清除
3   接收超时：INT_CHAR_TIMEOUT_INDICATION 
    仅仅FIFO模式有效 ， 在最后四个字节的时间内，没有字符从接收器FIFO 中移除或者输入，并且在此期间接收器FIFO中至少有一个字符
    中断清除方式：（1）1个字节从接收FIFO中被读出  （2）一个新的字节到达接收FIFO   （3）PMU寄存器中的URRST 位置1
4   传输保持寄存器空（THR）  INT_THR_EMPTY
    非FIFO模式下： THR空。 FIFO模式下，传输器FIFO空 
    中断清除方式：一个字节被写到 THR
*/

void UART0_Handler(void) {
	uint8_t rev_data = 0;
	NVIC_ClearPendingIRQ(UART0_IRQn);
	//接收线中断 有错误或者break
    if(UART_INT_TYPE(CMSDK_UART0) == INT_RCV_LINE_STATUS) {
    	CMSDK_UART0->IER &= ~CMSDK_UART_IER_RLSI_EN_Msk;
    }
	
	//数据就绪中断
	//接收超时中断
    if((UART_INT_TYPE(CMSDK_UART0) == INT_RCV_DATA_AVAILABLE) || (UART_INT_TYPE(CMSDK_UART0) == INT_CHAR_TIMEOUT_INDICATION)) 
	{
    CMSDK_UART0->IER &= ~CMSDK_UART_IER_RDAI_EN_Msk;
		rev_data = CMSDK_UART0->RBR; 
		UartPutc2(CMSDK_UART0,rev_data);
		CMSDK_UART0->IER |= CMSDK_UART_IER_RDAI_EN_Msk;
    }
    return;
}

//  ---------------- 分析和处理数据 ----------------
typedef enum {
    CRC8,
    CRC16,
    CRC32
} CRC_Type;

// ---------------- CRC8 (多项式 0x07, 初始值 0x00) ----------------
static uint8_t CalculateCRC8(const uint8_t *data, size_t length)
{
    uint8_t crc = 0x00;
    for (size_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x07;
            else
                crc <<= 1;
        }
    }
    return crc;
}

// ---------------- CRC16 (Modbus, 多项式 0xA001, 初始值 0xFFFF) ----------------
static uint16_t CalculateCRC16(const uint8_t *data, size_t length)
{
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

// ---------------- CRC32 (多项式 0xEDB88320, 初始值 0xFFFFFFFF) ----------------
static uint32_t CalculateCRC32(const uint8_t *data, size_t length)
{
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc >>= 1;
        }
    }
    return ~crc; // 取反输出
}

// ---------------- 统一接口 ----------------
uint32_t CalculateCRC(CRC_Type type, const uint8_t *data, size_t length)
{
    switch (type) {
        case CRC8:
            return (uint32_t)CalculateCRC8(data, length);
        case CRC16:
            return (uint32_t)CalculateCRC16(data, length);
        case CRC32:
            return (uint32_t)CalculateCRC32(data, length);
        default:
            return 0; // 类型错误
    }
}



void ParseReceivedPacket(uint8_t *packet, uint8_t length);
void ProcessSerialPackets(void);

// ====================== 全局调试变量 ====================== //
volatile uint8_t uart_raw_buffer[64];       // 原始数据接收区
volatile uint16_t uart_wr_index = 0;        // 写指针
volatile uint16_t uart_rd_index = 0;        // 读指针
volatile uint8_t uart_overrun_flag = 0;     // 溢出标志

volatile uint8_t packet_buffer[32];         // 完整包缓冲区
volatile uint8_t packet_length = 0;         // 当前包长度
volatile uint8_t packet_received = 0;       // 包就绪标志
volatile uint8_t last_cmd_type = 0;         // 最后命令类型

// debug变量，后续可删除
//uint8_t	debug_cla_crc = 0;
//uint8_t	debug_rx_crc = 0;

// 定义设备状态结构体
typedef struct {
	volatile uint8_t intensity;         // 强度 0-100
    volatile uint8_t wave_mode;       // 模式 0-3
    volatile _Bool power;             // 开关状态

    volatile uint8_t start_delay;     // 启动延迟 (0.1s)
    volatile uint8_t rising_time;     // 上升延迟 (0.1s)
    volatile uint8_t holding_time;    // 保持延迟 (0.1s)
    volatile uint8_t falling_time;    // 下降延迟 (0.1s)
    volatile uint8_t control_gap;     // 控制间距 (0.1s)
} DeviceStatus;

// 声明并初始化一个全局的设备状态变量
static DeviceStatus device_status = {
    .intensity = 50,
    .wave_mode = 0,
    .power = 0,

    .start_delay = 30,
    .rising_time = 50,
    .holding_time = 50,
    .falling_time = 0,
    .control_gap = 5
};

// 检查变量是否被正确写入，后续删除
 uint8_t intensity;      // 强度 0-100
 uint8_t wave_mode;      // 模式 0-3
 _Bool power;            // 开关状态
 uint8_t start_delay;    // 启动延迟 (0.1s)
 uint8_t rising_time;    // 上升延迟 (0.1s)
 uint8_t holding_time;   // 保持延迟 (0.1s)
 uint8_t falling_time;   // 下降延迟 (0.1s)
 uint8_t control_gap;    // 控制间距 (0.1s)

// ====================================================== //

// UART中断处理程序 - 仅接收原始数据
void UART1_Handler(void) {
    uint8_t rev_data = 0;
    uint32_t fifo_count = 0;
    
    NVIC_ClearPendingIRQ(UART1_IRQn);
    
    if(UART_INT_TYPE(CMSDK_UART1) == INT_RCV_LINE_STATUS) {
        CMSDK_UART1->IER &= ~CMSDK_UART_IER_RLSI_EN_Msk;
    }
    
    if((UART_INT_TYPE(CMSDK_UART1) == INT_RCV_DATA_AVAILABLE) || 
       (UART_INT_TYPE(CMSDK_UART1) == INT_CHAR_TIMEOUT_INDICATION)) {
        
        CMSDK_UART1->IER &= ~CMSDK_UART_IER_RDAI_EN_Msk;
        fifo_count = (CMSDK_UART1->FSR >> 16) & 0x1F;
        
        for(uint32_t i = 0; i < fifo_count; i++) {
            rev_data = CMSDK_UART1->RBR;
            
            // 存储到环形缓冲区
            if((uart_wr_index + 1) % sizeof(uart_raw_buffer) != uart_rd_index) {
                uart_raw_buffer[uart_wr_index] = rev_data;
                uart_wr_index = (uart_wr_index + 1) % sizeof(uart_raw_buffer);
            } else {
                uart_overrun_flag = 1; // 设置溢出标志
            }
        }
    }
    
    CMSDK_UART1->IER |= CMSDK_UART_IER_RDAI_EN_Msk;
}

// 每次中断仅处理少量字节，降低阻塞时间
#define UART_PROCESS_BUDGET 8

// 包处理函数 (在主循环调用)
void ProcessUARTData(void) {
    uint8_t processed = 0;
    uint8_t appended  = 0;

    // 分时处理，避免一次性吃完全部数据占用过长时间
    while((uart_rd_index != uart_wr_index) && (processed < UART_PROCESS_BUDGET)) {
        uint8_t data = uart_raw_buffer[uart_rd_index];
        uart_rd_index = (uart_rd_index + 1) % sizeof(uart_raw_buffer);
        processed++;
				
        // 添加到包缓冲区
        if(packet_length < sizeof(packet_buffer)) {
            packet_buffer[packet_length++] = data;
            appended = 1;
        } else {
            // 缓冲区溢出，重置
            ResetPacketBuffer();
        }
    }
    
    // 只有在本次确实拼接了新数据时才检查包，减少无谓扫描
    if(appended) {
        CheckCompletePacket();
        if(packet_received) {
            packet_received = 0; // 清零发送标识符
        }
    }
}


// 在缓冲区中查找完整包
// 修改 CheckCompletePacket 函数
void CheckCompletePacket(void) {
    uint16_t start_index = 0;
    uint16_t end_index = 0;
    
    // 查找包头 (0x05)
    for(uint16_t i = 0; i < packet_length; i++) {
        if(packet_buffer[i] == 0x05) {
            start_index = i;
            break;
        }
    }
    
    // 查找包尾 (0x5F)
    for(uint16_t i = start_index + 1; i < packet_length; i++) {
        if(packet_buffer[i] == 0x5F) {
            end_index = i;
            uint8_t packet_size = end_index - start_index + 1;
            
            // 创建非volatile临时缓冲区
            uint8_t non_vol_packet[64];
            
            // 复制数据到非volatile缓冲区
            for(uint16_t j = 0; j < packet_size; j++) {
                non_vol_packet[j] = packet_buffer[start_index + j];
            }
            
            // 解析包
            ParseCompletePacket(non_vol_packet, packet_size);
            
            // 移除已处理部分
            uint16_t bytes_to_keep = packet_length - (end_index + 1);
            if(bytes_to_keep > 0) {
                // 直接使用循环复制而不是memcpy
                for(uint16_t k = 0; k < bytes_to_keep; k++) {
                    packet_buffer[k] = packet_buffer[end_index + 1 + k];
                }
                packet_length = bytes_to_keep;
            } else {
                ResetPacketBuffer();
            }
            
            return;
        }
    }
}

// 修改 ResetPacketBuffer 函数
void ResetPacketBuffer(void) {
    // 使用循环清零而不是memset
    for(uint16_t i = 0; i < sizeof(packet_buffer); i++) {
        packet_buffer[i] = 0;
    }
    packet_length = 0;
}

// 解析完整的数据包
// 修改后的解析函数
void ParseCompletePacket(uint8_t *packet, uint8_t length) {
    // 最小包长度检查 (包头1 + 长度1 + 数据0 + CRC1 + 包尾1 = 5字节)
    if(length < 5) {
        return; // 无效包
    }
    
    // 长度字段检查
    uint8_t data_len = packet[1]; // 数据长度
    
    // 验证长度一致性 (包头1+长度1+数据data_len+CRC1+包尾1)
    if(length != (data_len + 4)) {
        return; // 长度不匹配
    }
    
		// 检查包头
		if(packet[0] != 0x05) {
        return; // 包尾错误
    }
    // 检查包尾
    if(packet[length-1] != 0x5F) {
        return; // 包尾错误
    }
    
    // 提取命令类型和数据
    uint8_t *data_start = &packet[2];          // 数据起始位置 (跳过包头和长度)
    uint8_t crc_value = packet[length-2];      // CRC位置
//		debug_rx_crc = packet[length-2];
    
		/***** CRC校验（CRC校验类型可选，需与发送端一同调整）*****/
		// 1. 先对data长度进行CRC8校验
		// 2. 再对data内容进行CRC8校验
		// 3. 将前两个crc数据进行'或'运算
//		uint8_t crc = CalculateCRC8(&data_len, 1);  			// 先对 lebgth 做CRC
//		crc = CalculateCRC8(data_start, data_len) ^ crc;  // 再对 data 做 CRC (合并计算)
////		debug_cla_crc = crc; 															// 存入全局变量，用来debug
//		if(crc != crc_value){
//				return;
//		}
		
    
    // 根据数据长度提取不同的参数
    if(data_len >= 1) { // 电刺激开关
        uint8_t stim_switch = data_start[0];
        // 应用电刺激开关控制
        device_status.power = stim_switch;
				power = stim_switch;										// debug时实用，后续可删除
    }
    
    if(data_len >= 2) { // 波形类型
        uint8_t wave_type = data_start[1];
        // 设置波形模式
        device_status.wave_mode = wave_type;
				wave_mode = wave_type;										// debug时实用，后续可删除
    }
    
    if(data_len >= 3) { // 电压强度
        uint8_t voltage_level = data_start[2];
        // 设置电压强度
        device_status.intensity = voltage_level;
				intensity = voltage_level;			    			// debug时实用，后续可删除
    }
		
    if(data_len >= 4) { // 启动延时
        uint8_t start_delay_0 = data_start[3];
        // 设置启动延时
				 if(start_delay_0 <= 4){
					 start_delay_0 = 4;
				 }
        device_status.start_delay = start_delay_0;
        start_delay = start_delay_0;			    			// debug时实用，后续可删除
    }
		
    if(data_len >= 5) { // 上升时间
        uint8_t rising_time_0 = data_start[4];
        // 设置上升时间
        device_status.rising_time = rising_time_0;
        rising_time = rising_time_0;			    			// debug时实用，后续可删除
    }
		
    if(data_len >= 6) { // 保持时间
        uint8_t holding_time_0 = data_start[5];
        // 设置保持时间
        device_status.holding_time = holding_time_0;
        holding_time = holding_time_0;			    			// debug时实用，后续可删除
    }
		
    if(data_len >= 7) { // 下降时间
        uint8_t falling_time_0 = data_start[6];
        // 设置下降时间
        device_status.falling_time = falling_time_0;
        falling_time = falling_time_0;			    			// debug时实用，后续可删除
    }
		
		if(data_len >= 8) { // 控制间隔
        uint8_t control_gap_0 = data_start[7];
        // 设置下降时间
        device_status.control_gap = control_gap_0;
        control_gap = control_gap_0;			    			// debug时实用，后续可删除
    }
		
    // 标记包接收完成
    packet_received = 1;
    last_cmd_type = packet[1]; // 数据长度也可表示命令类型？
}



//void UART1_Handler(void) {
//	
//    uint32_t fifo_count = 0;
//    uint8_t new_data[32]; // 临时存储从FIFO读取的数据
//    uint16_t bytes_to_read = 0;
//	
//	uint8_t rev_data = 0;
//	uint32_t ParamNumber = 0;
//	NVIC_ClearPendingIRQ(UART1_IRQn);
//	//接收线中断 有错误或者break
//    if(UART_INT_TYPE(CMSDK_UART1) == INT_RCV_LINE_STATUS) {
//    	CMSDK_UART1->IER &= ~CMSDK_UART_IER_RLSI_EN_Msk;//禁用接收线路状态中断
//    }
//	
//	
//	//数据就绪中断													//接收超时中断
//    if((UART_INT_TYPE(CMSDK_UART1) == INT_RCV_DATA_AVAILABLE) || (UART_INT_TYPE(CMSDK_UART1) == INT_CHAR_TIMEOUT_INDICATION)) {
//		CMSDK_UART1->IER &= ~CMSDK_UART_IER_RDAI_EN_Msk;	
//		ParamNumber=(CMSDK_UART1->FSR>>16) & 0x1f;
//		bytes_to_read = (fifo_count > sizeof(new_data)) ? sizeof(new_data) : fifo_count;
//		for(uint32_t i=0;i<ParamNumber;i++)
//		{
//			rev_data = CMSDK_UART1->RBR; //把接收到的数据取出来
//			UartPutc2(CMSDK_UART1,rev_data);//把接收到的数据再发回去
//		}
//	}
//	CMSDK_UART1->IER |= CMSDK_UART_IER_RDAI_EN_Msk;//重新启用中断
//    return;
//}

