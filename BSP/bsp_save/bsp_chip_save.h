/***********************************************************************************************
************************************************************************************************
**
**文件名: bsp_chip_save.H
**作  者: mys
**日  期: 2017.02.26
**功  能: 提供片上FLASH的读写存储功能
**        完成系统参数的存储;
**版  本: V1.0
************************************************************************************************
***********************************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_CHIP_SAVE_H
#define __BSP_CHIP_SAVE_H

/* Includes ------------------------------------------------------------------*/
#include "cpu.h"
#include <os.h>
#include "stdio.h"
#include "stm32f10x_flash.h"
/* Define ------------------------------------------------------------------*/
#define CHIP_SAVE_ENABLE  1

/* 
* 定义存储器存储空间分配
 */
#define CHIP_SAVE_START_PAGE  124             //片内存储开始页地址
#define CHIP_SAVE_END_PAGE    128             //片内存储结束页地址(结束页不可读写)



#define RUN_PAR_SAVE_ADDR     123             //系统运行参数保存地址
#define RUN_PAR_SET_FLAG      0x5b5b          //数据有效标志

/* 
* 定义处理器具体型号
 */
#define STM32F10xxE
/* 
* 定义存储器页的个数,大小及表示页大小所需的位数
 */
#if defined STM32F10X_HD                               //大容量
  #if defined STM32F10xxC                              //256K
    #define FLASH_PAGE_NUMS         128
  #elif defined STM32F10xxD                            //384K
    #define FLASH_PAGE_NUMS         192
  #elif defined STM32F10xxE                            //512K
    #define FLASH_PAGE_NUMS         256
  #else
    #error "请选择大容量芯片的具体型号！"
  #endif
  #define FLASH_PAGE_SIZE           2048
  #define FLASH_PAGE_SIZE_BITS      11
#elif defined STM32F10X_CL                             // 互联型
  #if defined STM32F10xx8                              // 64K 
    #define FLASH_PAGE_NUMS         32 
  #elif defined STM32F10xxB                            // 128K 
    #define FLASH_PAGE_NUMS         64 
  #elif defined STM32F10xxC                            // 256K 
    #define FLASH_PAGE_NUMS         128
  #else
    #error "请选择互联性芯片的具体型号！"
  #endif
  #define FLASH_PAGE_SIZE           2048
  #define FLASH_PAGE_SIZE_BITS      11
#elif defined (STM32F10X_MD) || defined (STM32F10X_LD) //小容量、中容量
  #define FLASH_PAGE_NUMS           128                //默认16K
  #define FLASH_PAGE_SIZE           1024
  #define FLASH_PAGE_SIZE_BITS      10
#else
  #error "请选择STM32芯片类别！"
#endif

/* 定义内部FLASH的起始地址和结束地址 */
#define FLASH_START_ADDR   ((uint32_t)0x08000000)
#define FLASH_END_ADDR     ((uint32_t)(FLASH_START_ADDR + (FLASH_PAGE_NUMS << FLASH_PAGE_SIZE_BITS) - 1))



/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void OnChipInit(void);
inline const void * OnChipGet(uint32_t page_addr);
void OnChipSavePage(uint32_t page_addr,void * pdata);
void OnChipSave(uint32_t page_addr,const void * pdata,uint32_t data_size);

typedef struct{
  uint16_t       SetFlag;              //存储参数更改标志
  char           ip[15];              //需要连接的服务器IP地址
  uint16_t       port;               //连接服务器的端口号
}sRunPar;
#endif /* __BSP_CHIP_SAVE_H */
