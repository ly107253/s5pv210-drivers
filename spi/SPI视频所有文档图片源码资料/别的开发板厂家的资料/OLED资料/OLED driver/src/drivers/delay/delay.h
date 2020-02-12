/******************** (C) COPYRIGHT 2011 安联德嵌入式开发 ********************
 * 文件名       ：delay.h
 * 描述         ：模拟延时函数头文件
 *
 * 实验平台     ：安联德开发板
 * 库版本       ：
 * 嵌入系统     ：
 *
 * 作者         ：安联德嵌入式开发
 * 淘宝店       ：http://aldsz.taobao.com
 * 技术支持论坛 ：
**********************************************************************************/


#ifndef __DELAY_H__
#define __DELAY_H__

#include "common.h"

void  delay(void);          //延时约500ms
void  delayms(uint32  ms);

#endif