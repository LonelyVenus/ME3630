/***********************************************************************************************
************************************************************************************************
**
**文件名: bsp_chip_save.c
**作  者: mys
**日  期: 2017.02.26
**功  能: 提供片上FLASH的读写存储功能
**        完成系统参数的存储;
**版  本: V1.0
************************************************************************************************
***********************************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "bsp_chip_save.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
FLASH_Status Flash_Write(uint32_t Address, const uint8_t *Data, uint32_t data_len);
/* Private functions ---------------------------------------------------------*/
static sRunPar  tempRunPar;

const sRunPar DefaultRunPar = {
  RUN_PAR_SET_FLAG,
  "112.123.252.200",
  6000,
};
#if CHIP_SAVE_ENABLE

//全局变量
//函数体声明
void OnChipSave(uint32_t page_addr,const void * pdata,uint32_t data_size)
{
  CPU_SR_ALLOC();
  CPU_CRITICAL_ENTER();
  FLASH_Unlock();
  FLASH_ErasePage(FLASH_START_ADDR + page_addr * FLASH_PAGE_SIZE);
  Flash_Write(FLASH_START_ADDR + page_addr * FLASH_PAGE_SIZE,pdata,data_size);
  FLASH_Lock();
  CPU_CRITICAL_EXIT();
}
#pragma inline 
const void * OnChipGet(uint32_t page_addr)
{
  return (void *)(FLASH_START_ADDR + page_addr * FLASH_PAGE_SIZE);        //取得存储信息的地址
}

void OnChipInit(void)
{
  const sRunPar * prun_par_save = (const sRunPar *)OnChipGet(RUN_PAR_SAVE_ADDR);
  
  if(prun_par_save->SetFlag != RUN_PAR_SET_FLAG)
  {
    sRunPar * pnew_par = &tempRunPar;
    if(pnew_par)
    {
      * pnew_par = DefaultRunPar;
      OnChipSave(RUN_PAR_SAVE_ADDR,pnew_par,sizeof(sRunPar));
    }
  }
}


void OnChipSavePage(uint32_t page_addr,void * pdata)
{
  if(page_addr < CHIP_SAVE_START_PAGE || page_addr >= CHIP_SAVE_END_PAGE)
    return;
  FLASH_Unlock();
  FLASH_ErasePage(page_addr);
  Flash_Write(FLASH_START_ADDR + page_addr * FLASH_PAGE_SIZE,pdata,FLASH_PAGE_SIZE);
  FLASH_Lock();
}

FLASH_Status Flash_Write(uint32_t Address, const uint8_t *Data, uint32_t data_len)
{
  uint32_t num;
  uint16_t InputData;
  FLASH_Status Status;
  if (data_len % 2 != 0)  
    num = data_len / 2 + 1;
  else 
    num = data_len / 2;                         //每次可编程16位
  for (uint32_t i = 0;i < num;i++)
  {
    InputData = *Data;
    Data++;
    InputData += *Data << 8;
    Data++;
    Status = FLASH_ProgramHalfWord(Address, InputData);
    if (Status != FLASH_COMPLETE)
      return Status;
    Address += 2;
  }
  return Status;
}

#endif
