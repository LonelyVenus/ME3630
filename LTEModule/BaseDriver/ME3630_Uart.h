/***********************************************************************************************
************************************************************************************************
**
**文件名: ME3630_Uart.h
**作  者: mys
**日  期: 2017.3.2
**功  能: 提供MCU串口与ME3630的串口通信，主要完成消息的提取和发布相应处理消息。
**        
**版  本: V1.0
************************************************************************************************
***********************************************************************************************/
#ifndef  _ME3630_UART_H
#define  _ME3630_UART_H
/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   ME3630_MODULE
#define  ME3630_EXT
#else
#define  ME3630_EXT  extern
#endif

/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/
#include "bsp_uart.h"
#include "os.h"
#include "stm32f10x_gpio.h"
#include "AT_Command_Parser.h"
/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/
#define MEReciveBuffSize        512
#define TCPBuffSize        1024

#define ME_PWRKEY_GPIO                GPIOC
#define ME_PWRKEY_GPIO_PIN            GPIO_Pin_4
#define ME_PWRKEY_RCC                 RCC_APB2Periph_GPIOC  

#define ME_PWRKEY_ON()                  GPIO_SetBits(ME_PWRKEY_GPIO, ME_PWRKEY_GPIO_PIN)
#define ME_PWRKEY_OFF()                 GPIO_ResetBits(ME_PWRKEY_GPIO, ME_PWRKEY_GPIO_PIN)

#define ME_WAKEUP_IN_GPIO             GPIOA
#define ME_WAKEUP_IN_GPIO_PIN         GPIO_Pin_1
#define ME_WAKEUP_IN_RCC              RCC_APB2Periph_GPIOA

#define ME_RESET_GPIO                 GPIOC
#define ME_RESET_GPIO_PIN             GPIO_Pin_5
#define ME_RESET_RCC                  RCC_APB2Periph_GPIOC  
/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void ME3630Init();
uint32_t MEUartSend(uint8_t *p_data,uint32_t DataLen);
void MEUartRxTask(void *p_arg);


#endif /* _ME3630_UART_H */
