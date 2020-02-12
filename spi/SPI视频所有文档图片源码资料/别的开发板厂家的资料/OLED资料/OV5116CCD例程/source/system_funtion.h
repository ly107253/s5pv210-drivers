/******************************************************************************

          版权所有 (C), 2011-2012, 安联德科技(http://aldsz.taobao.com/)

 ******************************************************************************
  文 件 名   : system_funtion.h
  版 本 号   : v1.0
  作    者   : Andy
  生成日期   : 2012-11-12
  最近修改   : 
  功能描述   : system_funtion.c 的头文件

  修改历史   :
  1.日    期   : 2012-11-12
    作    者   : Andy
    修改内容   : 创建文件

******************************************************************************/
#ifndef  __SYSTEM_FUNTION_H__
#define  __SYSTEM_FUNTION_H__

/*----------------------------------------------------------------------------*
 * 包含头文件                                                                 *
 *----------------------------------------------------------------------------*/
#include "common.h"

/*----------------------------------------------------------------------------*
 * 宏定义                                                                     *
 *----------------------------------------------------------------------------*/
#define SYSTEM_BUS_CLOCK        64000000    //总线频率
#define SYSTEM_OSC_CLOCK        16000000    //晶振频率
#define SYSTEM_BAUD                 9600    //波特率

/*----------------------------------------------------------------------------*
 * 全局变量                                                                   *
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*
 * 内部函数原型                                                               *
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*
 * 外部函数原型                                                               *
 *----------------------------------------------------------------------------*/
extern VOID     SYSTEM_PLL_Init(VOID);
extern VOID     SYSTEM_ATD_Init(VOID);
extern UCHAR8   SYSTEM_ATD_Convert(VOID);
extern VOID     SYSTEM_DFLASH_Init(void);
extern VOID     SYSTEM_DFLASH_Erase(USHORT16 usAddress);
extern VOID     SYSTEM_DFLASH_Write(USHORT16 usAddress, USHORT16 *pusDataBuf);
extern VOID     SYSTEM_DFLASH_Read(USHORT16 usAddress, USHORT16 *pusDataBuf); 
extern VOID     SYSTEM_TIME_Delay(USHORT16 usTime);

#endif

