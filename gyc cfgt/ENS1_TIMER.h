#ifndef ENS1_TIMER_H
#define ENS1_TIMER_H
//#include "CMSDK_CM0.h"
#include "my_header.h"
typedef enum {TOTAL_TIME_MODE = 0 , TRIGGER_TIME_MODE=1}TIME_COUNT_MODE; //选择为总时间或有效运行时间
extern void TIMER0_Init(uint32_t Int_Period);
extern void TIMER1_Init(uint32_t Int_Period);
//extern volatile uint32_t CHANNEL_TIME_COUNT[4]; 
//extern volatile uint32_t TRIGGER_TIME_COUNT[4];
//extern volatile uint32_t TOTAL_TIME_THRESHOLD_VALUE[4];
//extern volatile uint32_t TRIGGER_TIME_THRESHOLD_VALUE[4];
//extern volatile uint8_t  TRIGGER_TIME_COUNT_FLAG;
//extern void CMSDK_timer_EnableIRQ(CMSDK_TIMER_TypeDef *CMSDK_TIMER);
//extern void CMSDK_timer_DisableIRQ(CMSDK_TIMER_TypeDef *CMSDK_TIMER);
//extern void CMSDK_timer_StartTimer(CMSDK_TIMER_TypeDef *CMSDK_TIMER);
//extern void CMSDK_timer_StopTimer(CMSDK_TIMER_TypeDef *CMSDK_TIMER);
//extern uint32_t CMSDK_timer_GetValue(CMSDK_TIMER_TypeDef *CMSDK_TIMER);
//extern void CMSDK_timer_SetValue(CMSDK_TIMER_TypeDef *CMSDK_TIMER, uint32_t value);
//extern uint32_t CMSDK_timer_GetReload(CMSDK_TIMER_TypeDef *CMSDK_TIMER);
//extern void CMSDK_timer_SetReload(CMSDK_TIMER_TypeDef *CMSDK_TIMER, uint32_t value);
//extern void CMSDK_timer_ClearIRQ(CMSDK_TIMER_TypeDef *CMSDK_TIMER);
//extern uint32_t  CMSDK_timer_StatusIRQ(CMSDK_TIMER_TypeDef *CMSDK_TIMER);
extern void CMSDK_timer_Init(CMSDK_TIMER_TypeDef *CMSDK_TIMER, uint32_t reload, bool irq_en);
//extern void CMSDK_timer_Init_ExtClock(CMSDK_TIMER_TypeDef *CMSDK_TIMER, uint32_t reload,uint32_t irq_en);
//extern void CMSDK_timer_Init_ExtEnable(CMSDK_TIMER_TypeDef *CMSDK_TIMER, uint32_t reload,uint32_t irq_en);
//extern void CMSDK_dualtimer_start(CMSDK_DUALTIMER_TypeDef *CMSDK_DUALTIMERx);
//extern void CMSDK_dualtimer_stop(CMSDK_DUALTIMER_TypeDef *CMSDK_DUALTIMERx);
//extern void CMSDK_dualtimer_irq_clear(CMSDK_DUALTIMER_TypeDef *CMSDK_DUALTIMERx);
//extern void CMSDK_dualtimer_setup_freerunning(CMSDK_DUALTIMER_TypeDef *CMSDK_DUALTIMERx,
//													unsigned int cycle, unsigned int prescale, 
//															unsigned int interrupt, unsigned int size);
//extern void CMSDK_dualtimer_setup_periodic(CMSDK_DUALTIMER_TypeDef *CMSDK_DUALTIMERx,
//												unsigned int cycle, unsigned int prescale, 
//														unsigned int interrupt, unsigned int size);
//extern void CMSDK_dualtimer_setup_oneshot(CMSDK_DUALTIMER_TypeDef *CMSDK_DUALTIMERx,
//													unsigned int cycle, unsigned int prescale,
//															unsigned int interrupt, unsigned int size);
//extern void CMSDK_RTC_Init_Calender(uint16_t prescaler, uint8_t data_mode, uint8_t hour_mode, uint32_t init_time, uint32_t init_date);
//extern void CMSDK_RTC_Config_Alarm(uint16_t prescaler, uint8_t data_mode, uint8_t hour_mode, uint32_t init_time, uint32_t init_date, uint32_t alarm_time, uint32_t alarm_date)	;
//extern void CMSDK_RTC_Config_PeriodWake(uint8_t clock_sel, uint16_t prescaler, uint16_t period_time);		
													
#endif

