/***********************************************************************************************
************************************************************************************************
**
**�ļ���: ME_State_Control.c
**��  ��: mys
**��  ��: 2017.3.28
**��  ��: ʹ��״̬�����ME3630ģ��Ŀ��ơ�
**
**��  ��: V1.0
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
void MESetCommand(eAT_Command cmd,eAT_Type at_type,uint32_t outtime);                  //���÷��͵��������MEģ�鷢������
void MESendCommand(uint8_t *p_char,eAT_Command cmd,eAT_Type at_type,uint32_t outtime); //�����ME3630ģ�鷢��AT����
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

#define ME_TICK         100                       //����MEģ���tickΪ 100*OS_CFG_TICK_RATE_HZ 100ms

/*
************************************************************************************************************************
*                                                 MEIint()
*
* Description: ���ME���еĳ�ʼ��
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
* Description: ���ME3630�ĵ�Դ������������
*
* Arguments  : none
* Returns    : none
************************************************************************************************************************
*/
void MEOpenPower()                         //ִ�д�ME3630ģ���Դ����
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
        SetMEErr();     //�����������β��ɹ����˳��������򣬽����쳣����
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
    ME_PWRKEY_OFF();                  //����power_on�ܽŵ�ƽ
    MESetCommand(AT_CMD_NULL,AT_NULL,ME_TICK*2);           //��ʱ200ms
    MERunStepInc();
    break;
  case 1:
    ME_PWRKEY_ON();
    MESetCommand(AT_CMD_NULL,AT_NULL,ME_TICK*200); //��ʱ20S�ȴ�ģ�鿪��
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
* Description: ���ME3630�Ŀ�����ʼ������
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
* Description: ���ME3630��TCP����
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
    MESendCommand("AT+ZIPCALL=1\r\n",AT_CMD_ZIPCALL,AT_SET,ME_TICK*50);                //��PS����
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
* Description: ���ME3630���ӹ����е��쳣����
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
*��  ��:  ME����״̬С�����1
*��  ��:
*����ֵ:
*��  ע:
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
*��  ��:  ME����״̬С��������
*��  ��:
*����ֵ:
*��  ע:
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
*��  ��:  ���õ�ǰATָ��Ļ�Ӧ״̬
*��  ��:  state ��Ӧ״̬
*��  ��:  retflag�Ƿ���������¼�
*����ֵ:
*��  ע:
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
*��  ��:  ����ME������״̬
*��  ��:  eME_State me_state ��ME״̬
*����ֵ:  none
*��  ע:
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
* Description: ���ME3630��ʱ��״̬���ƣ�
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
* Description: ME�������ӹ��̳����쳣ʱ����
*
* Arguments  : none
* Returns    : none
************************************************************************************************************************
*/
void SetMEErr()                                  //MEģ�����г��ִ��󱨸�
{
  SetMERunState(ME_EXCEPTION);
}

/*
************************************************************************************************************************
*                                                 MERun()
*
* Description: ME����״̬���ƣ����ݲ�ͬ��״̬���벻ͬ�Ĵ�����
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

