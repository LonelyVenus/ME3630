#ifndef _LTE_INCLUDE_H
#define _LTE_INCLUDE_H


#include "os.h"
#include "stdio.h"
#include "bsp_chip_save.h"
/***********************************************************
*                     ATָ�����ö�ٺͽṹ������
***********************************************************/
typedef enum {
  AT_CMD_NULL = 0,			    	        //ATģ��������ȴ�����
  AT_CMD_AT,                                //"AT"�������ڲ���MEģ���Ƿ�������ͨ��������
  AT_CMD_ATE0,                              //��ֹ����
  AT_CMD_CFUN,                              //���õ绰����
  AT_CMD_CMEE,                              //�����豸�ϱ�����
  AT_CMD_CREG,                            	//��ѯ����ע��״̬
  AT_CMD_CPIN,                            	//��ѯSIM��PIN״̬
  AT_CMD_CTZU,                              //ʹ���Զ���ȡ����ʱ��
  AT_CMD_CMGF,                              //���ö��Ÿ�ʽ
  AT_CMD_CSMS,                              //ѡ����ŷ���
  AT_CMD_CNMI,                              //����Ϣָʾ
  AT_CMD_CPMS,                              //��ѡ����Ϣ�洢��
  AT_CMD_CGDCONT,                           //����PDP������
  AT_CMD_CSQ,                             	//��ѯ�ź�ǿ������
  AT_CMD_ZSNT,                              //����ѡ��ģʽ����
  AT_CMD_ZSDT,                              //ʹ��sim���Ȱβ�
  AT_CMD_ZIPCALL,                           //�򿪻�ر�PS CALL
  AT_CMD_ZIPOPEN,                           //����TCP/UDP����
  AT_CMD_ZIPCLOSE,                          //�ر�TCP/UDP����
  AT_CMD_ZIPSEND,                           //TCP/UDP���ݷ���
  AT_CMD_ZIPSTAT,                           //��ȡ�׽���״̬
  AT_CMD_WAIT_CALL_READY,                 	//�ȴ���������
  AT_CMD_DELAY,
}eAT_Command;

typedef enum{
  AT_NULL = 0,
  AT_SET,
  AT_QUERY,
}eAT_Type;

typedef enum {
  AT_STATE_SEND = 1,		                     //�ѷ�����,ûӦ��
  AT_STATE_BACK,				     //��Ӧ����
  AT_STATE_TIMEOUT,                                  //��Ӧ��ʱ
  AT_STATE_NULL,				     //û����
  AT_STATE_ERR,				             //�ظ�����
  AT_STATE_OK,			                     //�ظ�OK
  AT_STATE_CONNECT_OK,
  AT_STATE_CONNECTING,
  AT_STATE_CONNECT_CLOSE,
  AT_STATE_PPPCONNECT_OK,
  AT_STATE_PPPCONNECT_CLOSE,
}eAT_State;

typedef struct {
  eAT_Command	ATCommand;                        //�ȴ��Ĳ�������
  eAT_Type      ATType;                           //��������  0:������ 1������  2����ѯ
  eAT_State	    ATState;                          //��������״̬
  uint32_t      OutTime;                          //״̬��ʱʱ��
  uint32_t      StartTime;                        //����ִ�п�ʼʱ��
  bool          ReturnFlag;                       //�����ѻظ���־
  uint32_t      Option;                           //����صĸ�����Ϣ
}sAtCmdInfo;

/*****************************************************************
*                          
*****************************************************************/
typedef enum {
  ME_CLOSED = 0,
  ME_OPEN,
  ME_INIT,
  ME_CONNECTING,
  ME_CONNECTED,
  ME_SENDDATA,
  ME_EXCEPTION,
}eME_State;
typedef struct {
  eME_State               ME_STATE;                         //MEģ������״̬
  uint8_t                 RunStep;                          //ÿ��״̬�µ����в���
  sAtCmdInfo              ATInfo;                           //GPRSģ��Ӧ������
  uint32_t                ConnectNum;                       //���Ӵ���
  bool                    METimeOutFlag;                    //�Ƿ���г�ʱ
  bool                    ParChangedFlag;                   //�����޸ı�־
  bool                    IsOpen;                           //�򿪱�־
  bool                    IsBusy;                           //æ
  bool                    IsNewMessage;                     //���¶���
  bool                    IsTCPConnected;                   //TCP����OK
  bool                    Enable;                           //ʹ��
  uint8_t                 ATEvent;                          //��GPRSģ���ϴ���ͻ���¼�
                                                            //0λ : ���¶����¼�
                                                            //1λ : �е绰�����¼�
  uint8_t                 SendState;                        //TCP ����״̬
                                                            // SENDSTATE_SENDOK	  ���ͳɹ�
                                                            // SENDSTATE_SENDERR  ���ͳ���
  uint32_t                 Rssi;                             //�����ź�ǿ��
  uint32_t                 Ber;                              //�ŵ�������
  uint8_t                 ipDisConnectedState;
  uint8_t                 ipConnectedState;
  uint8_t                 ipConnectFail;
  //sIpState                ipState[IP_NUM_MAX];              //IP��ַ
  uint8_t                 cur_connect_ip_no;                //��ǰ�����ӵ�IP��
}sMERunMem;

#endif // !_LTE_INCLUDE_H

