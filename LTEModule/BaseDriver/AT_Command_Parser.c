#include "AT_Command_Parser.h"

/**************************************************************************
*
*************************************************************************/
typedef bool(*WaitEventHandler)(char *pch,eAT_Type at_type);

bool WatiATHandler(char *pch,eAT_Type at_type);                       //AT_CMD_AT        
bool WaitATEHandler(char *pch,eAT_Type at_type);                      //AT_CMD_ATE0
bool WaitCFUNHandler(char *pch,eAT_Type at_type);                     //AT_CMD_CFUN,
bool WaitCMEEHandler(char *pch,eAT_Type at_type);                     //AT_CMD_CMEE,
bool WaitCREGHandler(char *pch,eAT_Type at_type);                     //AT_CMD_CREG,
bool WaitCPINHandler(char *pch,eAT_Type at_type);                     //AT_CMD_CPIN,
bool WaitCTZUHandler(char *pch,eAT_Type at_type);                     //AT_CMD_CTZU,
bool WaitCMGFHandler(char *pch,eAT_Type at_type);                     //AT_CMD_CMGF,
bool WaitCSMSHandler(char *pch,eAT_Type at_type);                     //AT_CMD_CSMS,
bool WaitCNMIHandler(char *pch,eAT_Type at_type);                     //AT_CMD_CNMI,
bool WaitCPMSHandler(char *pch,eAT_Type at_type);                     //AT_CMD_CPMS,
bool WaitCGDCONTHandler(char *pch,eAT_Type at_type);                  //AT_CMD_CGDCONT,
bool WaitCSQHandler(char *pch,eAT_Type at_type);                      //AT_CMD_CSQ,
bool WaitZSNTHandler(char *pch,eAT_Type at_type);                     //AT_CMD_ZSNT,
bool WaitZSDTHandler(char *pch,eAT_Type at_type);                     //AT_CMD_ZSDT,
bool WaitZIPCALLHandler(char *pch,eAT_Type at_type);                  //AT_CMD_ZIPCALL,
bool WaitZIPOPENHandler(char *pch,eAT_Type at_type);                  //AT_CMD_ZIPOPEN,
bool WaitZIPCLOSEHandler(char *pch,eAT_Type at_type);                 //AT_CMD_ZIPCLOSE,
bool WaitZIPSENDFHandler(char *pch,eAT_Type at_type);                 //AT_CMD_ZIPSEND,
bool WaitZIPSTATHandler(char *pch,eAT_Type at_type);                  //AT_CMD_ZIPSTAT,

const WaitEventHandler ATWaitEventHandler[] =
{
  NULL, 
  WatiATHandler,
  WaitATEHandler,
  WaitCFUNHandler,
  WaitCMEEHandler,
  WaitCREGHandler,
  WaitCPINHandler,
  WaitCTZUHandler,
  WaitCMGFHandler,
  WaitCSMSHandler,
  WaitCNMIHandler,
  WaitCPMSHandler,
  WaitCGDCONTHandler,
  WaitCSQHandler,
  WaitZSNTHandler,
  WaitZSDTHandler,
  WaitZIPCALLHandler,
  WaitZIPOPENHandler,
  WaitZIPCLOSEHandler,
  WaitZIPSENDFHandler,
  WaitZIPSTATHandler,
  NULL,
  NULL,
};
extern sMERunMem MERunMem;

bool ATAnswerCheck(eAT_Command cmd,eAT_Type at_type,char * pch)
{
  if (cmd >= sizeof(ATWaitEventHandler))
    return FALSE;
  if (ATWaitEventHandler[cmd] == NULL)
    return FALSE;
  if (!strncmp(pch, "AT+", 3))
  {
    //GprsSetAtState(AT_STATE_BACK, FALSE);
    return TRUE;
  }
  return ATWaitEventHandler[cmd](pch,at_type);
}

void ATHandle(uint8_t *p_ATBuff,uint16_t len)
{
  ATAnswerCheck(MERunMem.ATInfo.ATCommand,MERunMem.ATInfo.ATType,(char *)p_ATBuff);
}
/**************************************************************************
*                        AT_CMD_AT
*************************************************************************/
bool WatiATHandler(char *pch,eAT_Type at_type)
{
  if(!strncmp(pch,"OK",2))
    MESetAtState(AT_STATE_OK,TRUE,0);
  else if(!strncmp(pch,"+CME",4))
    MESetAtState(AT_STATE_ERR,TRUE,0);
  return TRUE;
}
/**************************************************************************
*                        AT_CMD_ATE0
*************************************************************************/
bool WaitATEHandler(char *pch,eAT_Type at_type)                      
{
  if(!strncmp(pch,"OK",2))
    MESetAtState(AT_STATE_OK,TRUE,0);
  else if(!strncmp(pch,"+CME",4))
    MESetAtState(AT_STATE_ERR,TRUE,0);
  return TRUE;
}
/**************************************************************************
*                        //AT_CMD_CFUN,
*************************************************************************/
bool WaitCFUNHandler(char *pch,eAT_Type at_type)                     
{
  static uint8_t cnt = 0;
  if(at_type == AT_QUERY)
  {
    if(!strncmp(pch,"+CFUN: 1",8))
      cnt = 1;
    else if(!strncmp(pch,"OK",2) && (cnt == 1))
    {
      MESetAtState(AT_STATE_OK,TRUE,0);
      cnt = 0;
    }
    else if(!strncmp(pch,"+CME ERROR:",11))
      MESetAtState(AT_STATE_ERR,TRUE,0);
  }
  else if(at_type == AT_SET)
  {
    if(!strncmp(pch,"OK",2))
      MESetAtState(AT_STATE_OK,TRUE,0);
    else if(!strncmp(pch,"+CME ERROR:",11))
      MESetAtState(AT_STATE_ERR,TRUE,0);
  }
  return TRUE;
}
/**************************************************************************
*                        //AT_CMD_CMEE,
*************************************************************************/
bool WaitCMEEHandler(char *pch,eAT_Type at_type)                     
{
  return TRUE;
}
/**************************************************************************
*                        //AT_CMD_CREG,
*************************************************************************/
bool WaitCREGHandler(char *pch,eAT_Type at_type)                     
{
  static uint8_t cnt = 0;
  static bool log_on = FALSE;
  if(at_type == AT_QUERY)
  {
    if(!strncmp(pch,"+CREG:",6))
    {
      cnt = 1;
      if(*(pch + 9) == '1' || *(pch + 9) == '5')
        log_on = TRUE;
      else 
        log_on = FALSE;
    }
    else if(!strncmp(pch,"OK",2) && (cnt == 1))
    {
      cnt = 0;
      if(log_on)
        MESetAtState(AT_STATE_OK,TRUE,0);
      else
        MESetAtState(AT_STATE_ERR,TRUE,0);
    } 
    else if(!strncmp(pch,"+CME ERROR:",11))
      MESetAtState(AT_STATE_ERR,TRUE,0);
  }
  else if(at_type == AT_SET)
  {
    if(!strncmp(pch,"OK",2))
      MESetAtState(AT_STATE_OK,TRUE,0);
    else if(!strncmp(pch,"+CME ERROR:",11))
      MESetAtState(AT_STATE_ERR,TRUE,0);
  }
  return TRUE;
}
/**************************************************************************
*                        //AT_CMD_CPIN,
*************************************************************************/
bool WaitCPINHandler(char *pch,eAT_Type at_type)                     
{
  
  static uint8_t code = 0;
  /*
  <code> ��
  1:READY آ��Ҫ�����κ�����
  2:SIM PIN ��Ҫ����PIN��
  3:SIM PUK PIN���������
  4:PH-SIM PIN SIM��������
  5:SIM PIN2 PIN2������
  6:SIM PUK2 PIN2���������
  7:PH-NET PIN ��������
  */
  static uint8_t cnt = 0;
  if(at_type == AT_QUERY)
  {
    if(!strncmp(pch,"+CPIN:",6))
    {
      cnt = 1;
      if(!strncmp(pch+7,"READY",5))
        code = 1;
      else if(!strncmp(pch+7,"SIM PIN",7))
        code =2;
      else if(!strncmp(pch+7,"SIM PUK PIN",11))
        code =3;
      else if(!strncmp(pch+7,"PH-SIM PIN SIM",14))
        code =4;
      else if(!strncmp(pch+7,"SIM PIN2 PIN2",13))
        code =5;
      else if(!strncmp(pch+7,"SIM PUK2 PIN2",13))
        code =6;
      else if(!strncmp(pch+7,"PH-NET PIN",10))
        code =7;
    }
    else if((cnt == 1) && !strncmp(pch,"OK",2))
    {
      cnt = 0;
      if(code == 1)
        MESetAtState(AT_STATE_OK,TRUE,code);
      else
        MESetAtState(AT_STATE_ERR,TRUE,code);
    }
    else if(!strncmp(pch,"+CME ERROR:",11))
      MESetAtState(AT_STATE_ERR,TRUE,0);
  }
  else if(at_type == AT_SET)
  {
    if(!strncmp(pch,"OK",2))
      MESetAtState(AT_STATE_OK,TRUE,0);
    else if(!strncmp(pch,"+CME ERROR:",11))
      MESetAtState(AT_STATE_ERR,TRUE,0);
  }
  return TRUE;
}
/**************************************************************************
*                        //AT_CMD_CTZU,
*************************************************************************/
bool WaitCTZUHandler(char *pch,eAT_Type at_type)                     
{
  return TRUE;
}
/**************************************************************************
*                        //AT_CMD_CMGF,
*************************************************************************/
bool WaitCMGFHandler(char *pch,eAT_Type at_type)                     
{
  return TRUE;
}
/**************************************************************************
*                        //AT_CMD_CSMS,
*************************************************************************/
bool WaitCSMSHandler(char *pch,eAT_Type at_type)                     
{
  return TRUE;
}
/**************************************************************************
*                        //AT_CMD_CNMI,
*************************************************************************/
bool WaitCNMIHandler(char *pch,eAT_Type at_type)                    
{
  return TRUE;
}
/**************************************************************************
*                        //AT_CMD_CPMS,
*************************************************************************/
bool WaitCPMSHandler(char *pch,eAT_Type at_type)                     
{
  return TRUE;
}
/**************************************************************************
*                        //AT_CMD_CGDCONT,
*************************************************************************/
bool WaitCGDCONTHandler(char *pch,eAT_Type at_type)                  
{
  return TRUE;
}
/**************************************************************************
*                        //AT_CMD_CSQ,
*************************************************************************/
bool WaitCSQHandler(char *pch,eAT_Type at_type)                      
{
  static uint8_t cnt = 0;
  if(!strncmp(pch,"+CSQ:",5))
  {
    sscanf(pch + 6,"%u,%u",&MERunMem.Rssi,&MERunMem.Ber);
    cnt = 1;
  }
  else if (!strncmp(pch, "OK", 2) && (cnt == 1))
  {
    cnt = 0;
    MESetAtState(AT_STATE_OK, TRUE,0);
  } 
  else if(!strncmp(pch,"+CME ERROR:",11))
    MESetAtState(AT_STATE_ERR,TRUE,0);
  return TRUE;
}
/**************************************************************************
*                        //AT_CMD_ZSNT,
*************************************************************************/
bool WaitZSNTHandler(char *pch,eAT_Type at_type)                     
{
  return TRUE;
}
/**************************************************************************
*                        //AT_CMD_ZSDT,
*************************************************************************/
bool WaitZSDTHandler(char *pch,eAT_Type at_type)                     
{
  if (at_type == AT_SET)
  {
    if(!strncmp(pch, "OK", 2))
      MESetAtState(AT_STATE_OK, TRUE,0);
    else 
      MESetAtState(AT_STATE_ERR, TRUE,0);
  }
  else if (at_type == AT_QUERY)
  {
    return TRUE;
  }
  return TRUE;
}
/**************************************************************************
*                        //AT_CMD_ZIPCALL,
*************************************************************************/
bool WaitZIPCALLHandler(char *pch,eAT_Type at_type)                  
{
  /*
  <State>: IP call ״̬
  0�� �Ͽ�
  1�� ����
  2�� ��������
  3�� ���ڶϿ�
  */
  static uint32_t state = 0;
  static uint8_t cnt = 0;
  
  if(at_type == AT_QUERY)
  {
    if (!strncmp(pch, "+ZIPCALL:", 9))
    {
      cnt = 1;
      sscanf(pch + 10, "%u", &state);
    }
    else if ((cnt == 1) && !strncmp(pch, "OK", 2))
    {
      cnt = 0;
      switch (state) {
      case 1:
        MESetAtState(AT_STATE_OK, TRUE,state);
        break;
      default:
        MESetAtState(AT_STATE_ERR, TRUE,state);
        break;
      }
    }
  }
  else if (at_type == AT_SET)
  {
    if (!strncmp(pch, "OK", 2))
      cnt = 1;
    else if (!strncmp(pch, "+ZIPCALL:", 9) && (cnt == 1))
    {
      cnt = 0;
      sscanf(pch + 10, "%u", &state);
      switch (state)
      {
      case 1:
        MESetAtState(AT_STATE_OK, TRUE,state);
        break;
      default:
        MESetAtState(AT_STATE_ERR, TRUE,state);
        break;
      }
    }
  }
  return TRUE;
}
/**************************************************************************
*                        //AT_CMD_ZIPOPEN,
*************************************************************************/
bool WaitZIPOPENHandler(char *pch,eAT_Type at_type)                  
{
  /*
  <Status>: �׽������ӵ�״̬
  0���׽������ӹر�
  1���׽������Ӵ򿪣������������ͽ��������
  2���׽������Ӵ򿪣����������������ݣ������ͻ���������
  3���׽����������ڴ�
  4���׽����������ڹر�
  */
  static uint32_t status = 0;
  static uint8_t cnt = 0;
  
  if (at_type == AT_SET)
  {
    if (!strncmp(pch, "+ZIPSTAT:", 9))
    {
      cnt = 1;
      sscanf((char *)(pch + 12), "%u", &status);
    }
    else if ( (cnt==1)&& !strncmp(pch, "OK", 2))
    {
      cnt = 0;
      switch (status)
      {
      case 1:
        MESetAtState(AT_STATE_OK, TRUE,status);
        break;
      default:
        MESetAtState(AT_STATE_ERR, TRUE,status);
        break;
      }
    }
  }
  else if (at_type == AT_QUERY)
  {

  }
  return TRUE;
}
/**************************************************************************
*                        //AT_CMD_ZIPCLOSE,
*************************************************************************/
bool WaitZIPCLOSEHandler(char *pch,eAT_Type at_type)                 
{
  return TRUE;
}
/**************************************************************************
*                        //AT_CMD_ZIPSEND,
*************************************************************************/
bool WaitZIPSENDFHandler(char *pch,eAT_Type at_type)                 
{
  return TRUE;
}
/**************************************************************************
*                        //AT_CMD_ZIPSTAT,
*************************************************************************/
bool WaitZIPSTATHandler(char *pch,eAT_Type at_type)                  
{
  return TRUE;
}
