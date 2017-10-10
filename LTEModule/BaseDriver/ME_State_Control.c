/***********************************************************************************************
************************************************************************************************
**
**文件名: ME_State_Control.c
**作  者: mys
**日  期: 2017.3.28
**功  能: 使用状态机完成ME3630模块的控制。
**
**版  本: V1.0
************************************************************************************************
***********************************************************************************************/
/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include "ME_State_Control.h"
/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void MESetCommand(eAT_Command cmd,eAT_Type at_type,uint32_t outtime);                  //设置发送的命令但不向ME模块发送数据
void MESendCommand(uint8_t *p_char,eAT_Command cmd,eAT_Type at_type,uint32_t outtime); //完成向ME3630模块发送AT命令
void MERunStepInc();
void MESetRunStep(uint8_t runstep);
void MESetAtState(eAT_State state, bool retflag, uint32_t option);
void SetMERunState(eME_State me_state);
void SetMEErr();
/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

OS_TCB   AppTaskMERunTCB; 

sMERunMem MERunMem;

#define ME_TICK         100                       //设置ME模块的tick为 100*OS_CFG_TICK_RATE_HZ 100ms

/*
************************************************************************************************************************
*                                                 MEIint()
*
* Description: 完成ME运行的初始化
*
* Arguments  : none
* Returns    : none
************************************************************************************************************************
*/
void MEIint()
{
  OS_ERR        err;
  memset(&MERunMem,0,sizeof(MERunMem));
  SetMERunState(ME_OPEN);
  OSTaskSemPost ((OS_TCB *)&AppTaskMERunTCB,
                 (OS_OPT)OS_OPT_POST_NONE,
                 (OS_ERR *)&err);
}
/*
************************************************************************************************************************
*                                                 MEOpenPower()
*
* Description: 完成ME3630的电源开机控制流程
*
* Arguments  : none
* Returns    : none
************************************************************************************************************************
*/
void MEOpenPower()                         //执行打开ME3630模块电源功能
{
  static uint8_t TryNum = 0;
  if(MERunMem.RunStep == 2)
  {
    if(MERunMem.ATInfo.ATState == AT_STATE_OK)
    {
      TryNum = 0;
      MERunStepInc();
    }    
    else if(MERunMem.ATInfo.ATState == AT_STATE_TIMEOUT)
    {
      if(TryNum >= 2)
      {
        SetMEErr();     //连续开机两次不成功，退出启动程序，进入异常处理。
        TryNum = 0;
        return;
      }
      else
      {
        TryNum++;
        MESetRunStep(0);
      }  
    }
  }
  switch(MERunMem.RunStep){
  case 0:
    ME_PWRKEY_OFF();                  //拉低power_on管脚电平
    MESetCommand(AT_CMD_NULL,AT_NULL,ME_TICK*2);           //延时200ms
    MERunStepInc();
    break;
  case 1:
    ME_PWRKEY_ON();
    MESetCommand(AT_CMD_NULL,AT_NULL,ME_TICK*200); //延时20S等待模块开机
    MERunStepInc();
    break;
  case 2:
    MESendCommand("AT\r\n",AT_CMD_AT,AT_QUERY,ME_TICK*50);  
    break;
  case 3:
    SetMERunState(ME_INIT);
    break;
  default:
    break;
  }
}

/*
************************************************************************************************************************
*                                                 MEOpenInit()
*
* Description: 完成ME3630的开机初始化流程
*
* Arguments  : none
* Returns    : none
************************************************************************************************************************
*/
void MEOpenInit()
{ 
  static uint8_t TryNum = 0;
  if(MERunMem.ATInfo.ATState == AT_STATE_OK)
  {
    if(MERunMem.RunStep == 1)
      MESetRunStep(3);
    else if(MERunMem.RunStep == 2)
      MESetRunStep(1);
    else
      MERunStepInc();
  }
  else if(MERunMem.ATInfo.ATState == AT_STATE_ERR)
  {
    if(MERunMem.RunStep == 1)
      MERunStepInc();
    else if(MERunMem.RunStep == 5 && TryNum <= 10)
      TryNum++;
    else
    {
      SetMEErr();
      return;
    }
  }
  else if (MERunMem.ATInfo.ATState == AT_STATE_TIMEOUT)
  {
    SetMEErr();
    return;
  }
  switch(MERunMem.RunStep){
  case 0:
    MESendCommand("ATE0\r\n",AT_CMD_ATE0,AT_SET,ME_TICK*50);
    break;
  case 1:
    MESendCommand("AT+CFUN?\r\n",AT_CMD_CFUN,AT_QUERY,ME_TICK*50);
    break;
  case 2:
    MESendCommand("AT+CFUN=1\r\n",AT_CMD_CFUN,AT_SET,ME_TICK*50);
    break;
  case 3:
    MESendCommand("AT+CPIN?\r\n",AT_CMD_CPIN,AT_QUERY,ME_TICK*50);
    break;
  case 4:
    MESendCommand("AT+ZSDT=1\r\n", AT_CMD_ZSDT, AT_SET, ME_TICK * 50);
    break;
  case 5:
    MESendCommand("AT+CREG?\r\n",AT_CMD_CREG,AT_QUERY,ME_TICK*50);
    break;
  case 6:
    MESendCommand("AT+CSQ\r\n",AT_CMD_CSQ,AT_QUERY,ME_TICK*50);
    break;
  default:
    SetMERunState(ME_CONNECTING);
    break;
  }
}

/*
************************************************************************************************************************
*                                                 MEConnecting()
*
* Description: 完成ME3630的TCP连接
*
* Arguments  : none
* Returns    : none
************************************************************************************************************************
*/
void MEConnecting()
{
  const sRunPar *RunPar;
  uint8_t command[64];
  if(MERunMem.ATInfo.ATState == AT_STATE_OK)
    MERunStepInc();
  switch(MERunMem.RunStep){
  case 0:
    MESendCommand("AT+ZIPCALL=1\r\n",AT_CMD_ZIPCALL,AT_SET,ME_TICK*50);                //打开PS连接
    break;
  case 1:
    MESendCommand("AT+ZIPCALL?\r\n", AT_CMD_ZIPCALL, AT_QUERY, ME_TICK * 50);
    break;
  case 2:
    RunPar = (const sRunPar *)OnChipGet(RUN_PAR_SAVE_ADDR);
    sprintf((char *)command,"AT+ZIPOPEN=1,0,%s,%d\r\n",RunPar->ip,RunPar->port);
    MESendCommand(command,AT_CMD_ZIPOPEN,AT_SET,ME_TICK*300);
    break;
  case 3:
    SetMERunState(ME_CONNECTED);
    break;
  default:
    break;
  }
}
/*
************************************************************************************************************************
*                                                 MEException()
*
* Description: 完成ME3630连接过程中的异常处理
*
* Arguments  : none
* Returns    : none
************************************************************************************************************************
*/
void MEException()
{
  
}


void MESetCommand(eAT_Command cmd,eAT_Type at_type,uint32_t outtime)
{
  OS_ERR GetTimeErr;
  MERunMem.ATInfo.ATCommand = cmd;
  MERunMem.ATInfo.ATType = at_type;
  if(cmd == AT_CMD_NULL)
    MERunMem.ATInfo.ATState = AT_STATE_NULL;
  else
    MERunMem.ATInfo.ATState = AT_STATE_SEND;
  MERunMem.ATInfo.ReturnFlag = FALSE;
  MERunMem.ATInfo.OutTime = outtime;
  MERunMem.ATInfo.StartTime = OSTimeGet(&GetTimeErr);
}

void MESendCommand(uint8_t *p_char,eAT_Command cmd,eAT_Type at_type,uint32_t outtime)
{
  MEUartSend(p_char,strlen((char *)p_char));
  MESetCommand(cmd,at_type,outtime);
}
/*******************************************************************************
*功  能:  ME运行状态小步骤加1
*参  数:
*返回值:
*备  注:
*******************************************************************************/
void MERunStepInc()
{
  OS_ERR GetTimeErr;
  MERunMem.ATInfo.ATCommand = AT_CMD_NULL;
  MERunMem.ATInfo.ATState = AT_STATE_NULL;
  MERunMem.ATInfo.ReturnFlag = FALSE;
  MERunMem.ATInfo.StartTime = OSTimeGet(&GetTimeErr);
  MERunMem.RunStep++;
}
/*******************************************************************************
*功  能:  ME运行状态小步骤设置
*参  数:
*返回值:
*备  注:
*******************************************************************************/
void MESetRunStep(uint8_t runstep)
{
  OS_ERR GetTimeErr;
  MERunMem.ATInfo.ATCommand = AT_CMD_NULL;
  MERunMem.ATInfo.ATState = AT_STATE_NULL;
  MERunMem.ATInfo.ReturnFlag = FALSE;
  MERunMem.ATInfo.StartTime = OSTimeGet(&GetTimeErr);
  MERunMem.RunStep = runstep;
}
/*******************************************************************************
*功  能:  设置当前AT指令的回应状态
*参  数:  state 回应状态
*参  数:  retflag是否产生运行事件
*返回值:
*备  注:
*******************************************************************************/
void MESetAtState(eAT_State state,bool retflag,uint32_t option)
{
  OS_ERR        err;
  MERunMem.ATInfo.ATState = state;
  MERunMem.ATInfo.Option = option;
  MERunMem.ATInfo.ReturnFlag = retflag;
  if(retflag)
  {
    OSTaskSemPost ((OS_TCB *)&AppTaskMERunTCB,
                   (OS_OPT)OS_OPT_POST_NONE,
                   (OS_ERR *)&err);
  }
}
/*******************************************************************************
*功  能:  设置ME的运行状态
*参  数:  eME_State me_state ：ME状态
*返回值:  none
*备  注:
*******************************************************************************/
void SetMERunState(eME_State me_state)
{
  OS_ERR        err;
  CPU_SR_ALLOC();
  CPU_CRITICAL_ENTER();
  MERunMem.ME_STATE = me_state;
  MERunMem.ATInfo.ATCommand = AT_CMD_NULL;
  MESetAtState(AT_STATE_NULL,FALSE,0);
  CPU_CRITICAL_EXIT();
  MESetRunStep(0);
  OSTaskSemPost ((OS_TCB *)&AppTaskMERunTCB,
                 (OS_OPT)OS_OPT_POST_NONE,
                 (OS_ERR *)&err);
}
/*
************************************************************************************************************************
*                                                 MEOpen()
*
* Description: 完成ME3630的时间状态控制，
*
* Arguments  : none
* Returns    : none
************************************************************************************************************************
*/
void METimeTask()
{
  OS_ERR TimeGetErr,err;
  while(1)
  {
    OSTimeDlyHMSM(0, 0, 0, 100, 
                  OS_OPT_TIME_HMSM_STRICT, 
                  &err);
    if(!MERunMem.ATInfo.ReturnFlag)
    {
      if( OSTimeGet(&TimeGetErr) - MERunMem.ATInfo.StartTime > MERunMem.ATInfo.OutTime)
      {
        if(MERunMem.ATInfo.ATState < AT_STATE_TIMEOUT)
          MERunMem.ATInfo.ATState = AT_STATE_TIMEOUT;
        MERunMem.ATInfo.ReturnFlag = TRUE;
        OSTaskSemPost ((OS_TCB *)&AppTaskMERunTCB,
                      (OS_OPT)OS_OPT_POST_NONE,
                       (OS_ERR *)&err);
      }
    }
  }
  
}
/*
************************************************************************************************************************
*                                                 SetMEErr()
*
* Description: ME启动连接过程出现异常时调用
*
* Arguments  : none
* Returns    : none
************************************************************************************************************************
*/
void SetMEErr()                                  //ME模块运行出现错误报告
{
  SetMERunState(ME_EXCEPTION);
}

/*
************************************************************************************************************************
*                                                 MERun()
*
* Description: ME运行状态控制，根据不同的状态进入不同的处理函数
*
* Arguments  : none
* Returns    : none
************************************************************************************************************************
*/
void MERunTask()
{
  CPU_TS        TS;
  OS_ERR        err;
  while(1)
  {
    OSTaskSemPend ((OS_TICK)0,
                   (OS_OPT)OS_OPT_PEND_BLOCKING,
                   (CPU_TS *)&TS,
                   (OS_ERR *)&err);
    switch(MERunMem.ME_STATE){
    case ME_CLOSED:
      break;
    case ME_OPEN:
      MEOpenPower();
      break;
    case ME_INIT:
      MEOpenInit();
      break;
    case ME_CONNECTING:
      MEConnecting();
      break;
    case ME_CONNECTED:
      break;
    case ME_SENDDATA:
      break;
    case ME_EXCEPTION:
      MEException();
      break;
    default:
      break;
    }
  }
  
}

