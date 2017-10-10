/***********************************************************************************************
************************************************************************************************
**
**文件名: ME3630_Uart.c
**作  者: mys
**日  期: 2017.3.2
**功  能: 提供MCU串口与ME3630的串口通信，主要完成消息的提取和发布相应处理消息。
**
**版  本: V1.0
************************************************************************************************
***********************************************************************************************/
/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#define ME3630_MODULE
#include "ME3630_Uart.h"
/*
*********************************************************************************************************
*                                            UART PARAMETER
*********************************************************************************************************
*/
#define ME_UART_COM       COM2
sUart_Paramer Uart2_Paramer = {NULL,115200};
extern OS_SEM   Uart2_Sem;

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/
uint32_t MEReciveDataLen;
uint32_t TCPDataLen;
uint8_t MEReciveBuff[MEReciveBuffSize];
uint8_t TCPDataBuff[TCPBuffSize];
/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void ATReturnParser();
/*
************************************************************************************************************************
*                                                 ME3630 Init
*
* Description: 完成ME3630串口的打开；电源控制脚（pwrkey）的初始化，模块唤醒控制脚(wakeup_in)初始化.
*
* Arguments  : none
* Returns    : none
************************************************************************************************************************
*/
void ME3630Init()
{
  /***************PWRKEY GPIO INIT*******/
  GPIO_InitTypeDef  PWRKEY_GPIO_INIT;
  RCC_APB2PeriphClockCmd(ME_PWRKEY_RCC, ENABLE);
  PWRKEY_GPIO_INIT.GPIO_Pin   = ME_PWRKEY_GPIO_PIN;
  PWRKEY_GPIO_INIT.GPIO_Speed = GPIO_Speed_50MHz;
  PWRKEY_GPIO_INIT.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_Init(ME_PWRKEY_GPIO, &PWRKEY_GPIO_INIT);
  /***************WAKEUP_IN GPIO INIT*******/
  GPIO_InitTypeDef  WAKEUP_IN_GPIO_INIT;
  RCC_APB2PeriphClockCmd(ME_WAKEUP_IN_RCC, ENABLE);
  WAKEUP_IN_GPIO_INIT.GPIO_Pin   = ME_WAKEUP_IN_GPIO_PIN;
  WAKEUP_IN_GPIO_INIT.GPIO_Speed = GPIO_Speed_50MHz;
  WAKEUP_IN_GPIO_INIT.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_Init(ME_WAKEUP_IN_GPIO, &WAKEUP_IN_GPIO_INIT);
  
  UartOpen(ME_UART_COM,&Uart2_Paramer);
}
/*
************************************************************************************************************************
*                                                 通过串口向ME3630发送数据
*
* Description: 被调用于向ME3630串口发送数据.
*
* Arguments  : uint8_t  *p_data  发送数据指针
*              uint32_t DataLen   待发送数据长度
* Returns    : 失败返回0
************************************************************************************************************************
*/
uint32_t MEUartSend(uint8_t *p_data,uint32_t DataLen)
{
  return UartWrite(ME_UART_COM,p_data,DataLen);
}
/*
************************************************************************************************************************
*                                                  ME3630 Data Recive task
*
* Description: 用于处理从ME3630串口读取到的数据的任务.
*
* Arguments  : p_arg    无用指针

* Returns    : none
************************************************************************************************************************
*/
void MEUartRxTask(void *p_arg)
{
  OS_ERR        err;
  CPU_TS        Uart2_Ts;
  while(DEF_ON)
  {
     OSTaskSemPend ((OS_TICK)0,
                   (OS_OPT)OS_OPT_PEND_BLOCKING,
                   (CPU_TS *)&Uart2_Ts,
                   (OS_ERR *)&err);
    if (!UartIsRecive(ME_UART_COM))
    {
      ATReturnParser();
    }
  }
}

/*******************************************************************************
*功  能:  从数据池中读取AT指令的回应
*参  数:  none
*返回值:  true    :读取到符合AT返回命令格式的回复返回
*        fasle    :未读取到符合AT返回命令格式的回复返回
*备  注:
*******************************************************************************/
const char * p_ziprecv = "+ZIPRECV:";
void ATReturnParser()
{
  uint32_t len = 0;
  uint8_t TempReadBuff[8];
  uint8_t TempReadLen=0;
  
  do {
    len = UartRead(ME_UART_COM, &TempReadBuff[TempReadLen], 1);
    TempReadLen++;

    /***********************如果读取到<CR><LF>*******************************/
    if (TempReadBuff[TempReadLen-1] == 0x0a && TempReadBuff[TempReadLen - 2] == 0x0d)
    {
      TempReadLen = 0;
      do
      {
        len = UartRead(ME_UART_COM, &MEReciveBuff[MEReciveDataLen], 1);
        
        /*************************如果读取到<CR>**********************************/
        if (MEReciveDataLen == 0 && MEReciveBuff[MEReciveDataLen] == 0x0d)
        {
            TempReadBuff[TempReadLen] = 0x0d;
            TempReadLen++;
            break;   
        }
        /**********************判断是否是TCP数据上报**************************/
        if (MEReciveDataLen == 9)
        {
          if ((strncmp((char*)MEReciveBuff, p_ziprecv, 9) == 0)) // 接收到的字符串是"+ZIPRECV:"
          {
            MEReciveDataLen = 0;
            uint8_t AddrCnt = 0;              // TCP/UDP上报的数据地址域是否结束
            uint32_t TcpUpDataLen = 0;                // TCP/UDP上报的数据的总长度
            uint8_t TCPDataHead[36];
            uint8_t HeadLen = 0;
            bool TCPHeadEnd = FALSE;
            do
            {
              if (!TCPHeadEnd)
              {
                len = UartRead(ME_UART_COM, &TCPDataHead[HeadLen], 1);
                HeadLen++;
                if (AddrCnt == 3 && TCPDataHead[HeadLen] != ',')
                {
                  sscanf((char *)&TCPDataHead[HeadLen], "%u", &TcpUpDataLen);
                }
                if (TCPDataHead[HeadLen] == ',')
                {
                  AddrCnt++;
                  if (AddrCnt == 4)
                  {
                    /**********Add tcp/ip data head handle***********/

                    /************************************************/
                    TCPHeadEnd = TRUE;
                  }
                }
              }
              else
              {
                len = UartRead(ME_UART_COM, &TCPDataBuff[TCPDataLen], 1);
                if (TCPDataLen >= TcpUpDataLen - 1)
                  break;
                else
                  TCPDataLen++;
              }  
            }while (len != 0);
          }
        }
        /***********************读取到指令尾部<CR><LF>*****************************/
        if (MEReciveBuff[MEReciveDataLen] == 0x0a && MEReciveBuff[MEReciveDataLen-1] == 0x0d)
        {
          MEReciveDataLen -= 1;
          //UartWrite(COM4,MEReciveBuff,MEReciveDataLen);
          ATHandle(MEReciveBuff,MEReciveDataLen);
          MEReciveDataLen = 0;  //重置AT回应的存储
          break;
        }
        MEReciveDataLen++;
      } while (len != 0);
      if (MEReciveDataLen != 0)
      {
        /**************Add exception handle****************/

        /**************************************************/
        MEReciveDataLen = 0;
      }
    }
    /***************************************************************************/ 
  } while (len != 0);
}


