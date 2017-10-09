/***********************************************************************************************
************************************************************************************************
**
**文件名: lib_fifo.h
**作  者: mys
**日  期: 2017.09.08
**功  能: 循环先入先出队列(带数据读写)
**备  注: 数据池使用字节的数组
**备  注: 带写回调函数
************************************************************************************************
***********************************************************************************************/

#ifndef __LIB_FIFO_H
#define __LIB_FIFOCH
#include <os.h>
#include "string.h"
#include "stm32f10x_type.h"


typedef void (*CFifoWriteCallBack) (void);

//链表管理结构体
typedef struct{
  CPU_INT16U readItem;              //读数据位置
  CPU_INT16U writeItem;             //写入位置
  CPU_INT16U maxNum;                //最大数据数量
  volatile CPU_INT16U dataNum;      //当前数据数量
  CPU_INT08U * pbuff;                 //数据池
  CFifoWriteCallBack  callBack;
}scFifo;


/*******************************************************************************
*功  能:  使用指定的参数创建队列
*参  数:  psFifo -- 空的队列对象
*参  数:  pbuff  -- 队列使用的数据池
*参  数:  buff_size -- 队列的大小
*返回值:
*备  注:
*******************************************************************************/
void CFifoCreate(scFifo * psFifo,CPU_INT08U * pbuff,CPU_INT16U buff_size,CFifoWriteCallBack callBack);

/*******************************************************************************
*功  能:  从指定队列中读出一个数据
*参  数:  psFifo -- 队列对象
*参  数:  pch  -- 读出的数据
*返回值:  读出数据的长度
*备  注:  当队列对象不存在或数据存储位置不存在或队列为空时返回0
*******************************************************************************/
CPU_INT16U CFifoRead(scFifo * psFifo,CPU_INT08U * pch);

/*******************************************************************************
*功  能:  从指定队列中读出一个数据
*参  数:  psFifo -- 队列对象
*参  数:  pch  -- 读出的数据
*参  数:  len  -- 读出数据的长度
*返回值:  读出数据的长度
*备  注:  当队列对象不存在或数据存储位置不存在或队列为空时返回0
*******************************************************************************/
CPU_INT16U CFifoReads(scFifo * psFifo,CPU_INT08U * pch, CPU_INT16U len);

/*******************************************************************************
*功  能:  将数据写入指定队列
*参  数:  psFifo -- 队列对象
*参  数:  ch  -- 写入的数据
*返回值:  写入数据数量
*备  注:  当队列对象不存在或写入的数据不存在或队列为满时返回0
*******************************************************************************/
CPU_INT16U CFifoWriteNoWait(scFifo * psFifo,CPU_INT08U ch);

/*******************************************************************************
*功  能:  无等待的数据写入
*参  数:  psFifo -- 队列对象
*参  数:  pch  -- 写入的数据
*参  数:  len  -- 写入数据的数量
*返回值:  写入数据数量
*备  注:  当队列对象不存在或写入的数据不存在或队列为满时返回0
*******************************************************************************/
CPU_INT16U CFifoWritesNoWait(scFifo * psFifo,CPU_INT08U* pch,CPU_INT16U len);

/*******************************************************************************
*功  能:  将数据写入指定队列
*参  数:  psFifo -- 队列对象
*参  数:  ch  -- 写入的数据
*返回值:  是否写入成功
*备  注:  当队列对象不存在或写入的数据不存在返回false,队列满时等待
*******************************************************************************/
CPU_INT16U CFifoWrite(scFifo * psFifo,CPU_INT08U ch);

/*******************************************************************************
*功  能:  将数据写入指定队列
*参  数:  psFifo -- 队列对象
*参  数:  pch  -- 写入的数据
*参  数:  len   -- 写入数据的长度
*返回值:  是否写入成功
*备  注:  当队列对象不存在或写入的数据不存在返回false,队列满时等待
*******************************************************************************/
CPU_INT16U CFifoWrites(scFifo * psFifo,const CPU_INT08U* pch,CPU_INT16U len);

/*******************************************************************************
*功  能:  判断队列是否为空
*参  数:  psFifo -- 队列对象
*返回值:  是否为空
*备  注:  当对象存在且为空时返回true否则返回false
*******************************************************************************/
bool CFifoIsEmpty(scFifo * psFifo);

/*******************************************************************************
*功  能:  清空队列
*参  数:  psFifo -- 队列对象
*返回值:
*备  注:
*******************************************************************************/
void CFifoEmpty(scFifo * psFifo);

/*******************************************************************************
*功  能:  判断队列是否已满
*参  数:  psFifo -- 队列对象
*返回值:  是否已满
*备  注:  当对象存在且已满时返回true否则返回false
*******************************************************************************/
bool CFifoIsFill(scFifo * psFifo);

#endif
