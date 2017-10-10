#ifndef _LTE_INCLUDE_H
#define _LTE_INCLUDE_H


#include "os.h"
#include "stdio.h"
#include "bsp_chip_save.h"
/***********************************************************
*                     AT指令相关枚举和结构体申明
***********************************************************/
typedef enum {
  AT_CMD_NULL = 0,			    	        //AT模块无命令等待返回
  AT_CMD_AT,                                //"AT"命令用于测试ME模块是否与主机通信已连接
  AT_CMD_ATE0,                              //禁止回显
  AT_CMD_CFUN,                              //设置电话功能
  AT_CMD_CMEE,                              //设置设备上报错误
  AT_CMD_CREG,                            	//查询网络注册状态
  AT_CMD_CPIN,                            	//查询SIM卡PIN状态
  AT_CMD_CTZU,                              //使能自动获取网络时间
  AT_CMD_CMGF,                              //设置短信格式
  AT_CMD_CSMS,                              //选择短信服务
  AT_CMD_CNMI,                              //新消息指示
  AT_CMD_CPMS,                              //首选短消息存储器
  AT_CMD_CGDCONT,                           //定义PDP上下文
  AT_CMD_CSQ,                             	//查询信号强度命令
  AT_CMD_ZSNT,                              //网络选择模式配置
  AT_CMD_ZSDT,                              //使能sim卡热拔插
  AT_CMD_ZIPCALL,                           //打开或关闭PS CALL
  AT_CMD_ZIPOPEN,                           //建立TCP/UDP连接
  AT_CMD_ZIPCLOSE,                          //关闭TCP/UDP连接
  AT_CMD_ZIPSEND,                           //TCP/UDP数据发送
  AT_CMD_ZIPSTAT,                           //获取套接字状态
  AT_CMD_WAIT_CALL_READY,                 	//等待开机命令
  AT_CMD_DELAY,
}eAT_Command;

typedef enum{
  AT_NULL = 0,
  AT_SET,
  AT_QUERY,
}eAT_Type;

typedef enum {
  AT_STATE_SEND = 1,		                     //已发发送,没应答
  AT_STATE_BACK,				     //回应命令
  AT_STATE_TIMEOUT,                                  //响应超时
  AT_STATE_NULL,				     //没发送
  AT_STATE_ERR,				             //回复错误
  AT_STATE_OK,			                     //回复OK
  AT_STATE_CONNECT_OK,
  AT_STATE_CONNECTING,
  AT_STATE_CONNECT_CLOSE,
  AT_STATE_PPPCONNECT_OK,
  AT_STATE_PPPCONNECT_CLOSE,
}eAT_State;

typedef struct {
  eAT_Command	ATCommand;                        //等待的操作命令
  eAT_Type      ATType;                           //命令类型  0:无命令 1：设置  2：查询
  eAT_State	    ATState;                          //操作命令状态
  uint32_t      OutTime;                          //状态超时时间
  uint32_t      StartTime;                        //命令执行开始时间
  bool          ReturnFlag;                       //命令已回复标志
  uint32_t      Option;                           //命令返回的附加信息
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
  eME_State               ME_STATE;                         //ME模块运行状态
  uint8_t                 RunStep;                          //每个状态下的运行步数
  sAtCmdInfo              ATInfo;                           //GPRS模块应答内容
  uint32_t                ConnectNum;                       //连接次数
  bool                    METimeOutFlag;                    //是否空闲超时
  bool                    ParChangedFlag;                   //参数修改标志
  bool                    IsOpen;                           //打开标志
  bool                    IsBusy;                           //忙
  bool                    IsNewMessage;                     //有新短信
  bool                    IsTCPConnected;                   //TCP连接OK
  bool                    Enable;                           //使能
  uint8_t                 ATEvent;                          //从GPRS模块上传的突发事件
                                                            //0位 : 有新短信事件
                                                            //1位 : 有电话进入事件
  uint8_t                 SendState;                        //TCP 发送状态
                                                            // SENDSTATE_SENDOK	  发送成功
                                                            // SENDSTATE_SENDERR  发送出错
  uint32_t                 Rssi;                             //接收信号强度
  uint32_t                 Ber;                              //信道误码率
  uint8_t                 ipDisConnectedState;
  uint8_t                 ipConnectedState;
  uint8_t                 ipConnectFail;
  //sIpState                ipState[IP_NUM_MAX];              //IP地址
  uint8_t                 cur_connect_ip_no;                //当前待连接的IP号
}sMERunMem;

#endif // !_LTE_INCLUDE_H

