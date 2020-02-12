/******************************************************************************

          版权所有 (C), 2011-2012, 安联德科技(http://aldsz.taobao.com/)

 ******************************************************************************
  文 件 名   : oled.c
  版 本 号   : v1.1
  作    者   : Andy
  生成日期   : 2012-12-1
  最近修改   : 
  功能描述   : LED128x64OLED显示屏底层驱动
  修改历史   :
  1.日    期   : 2012-11-10
    作    者   : Andy
    修改内容   : 创建文件
    、
  2.日    期   : 2012-12-1
    作    者   : Andy
    修改内容   : 修复F6x8字符数组的bug，
                 该bug会导致LED_P6x8Char，LED_P6x8Str显示错误

******************************************************************************/

/*----------------------------------------------------------------------------*
 * 包含头文件                                                                 *
 *----------------------------------------------------------------------------*/
#include "oled.h"

/*----------------------------------------------------------------------------*
 * 宏定义                                                                     *
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*
 * 全局变量                                                                   *
 *----------------------------------------------------------------------------*/
const UCHAR8 F6x8[][6] =
{
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   //sp0
    { 0x00, 0x00, 0x00, 0x2f, 0x00, 0x00 },   // !1
    { 0x00, 0x00, 0x07, 0x00, 0x07, 0x00 },   // "2
    { 0x00, 0x14, 0x7f, 0x14, 0x7f, 0x14 },   // #3
    { 0x00, 0x24, 0x2a, 0x7f, 0x2a, 0x12 },   // $4
    { 0x00, 0x62, 0x64, 0x08, 0x13, 0x23 },   // %5
    { 0x00, 0x36, 0x49, 0x55, 0x22, 0x50 },   // &6
    { 0x00, 0x00, 0x05, 0x03, 0x00, 0x00 },   // '7
    { 0x00, 0x00, 0x1c, 0x22, 0x41, 0x00 },   // (8
    { 0x00, 0x00, 0x41, 0x22, 0x1c, 0x00 },   // )9
    { 0x00, 0x14, 0x08, 0x3E, 0x08, 0x14 },   // *10
    { 0x00, 0x08, 0x08, 0x3E, 0x08, 0x08 },   // +11
    { 0x00, 0x00, 0x00, 0xA0, 0x60, 0x00 },   // ,12
    { 0x00, 0x08, 0x08, 0x08, 0x08, 0x08 },   // -13
    { 0x00, 0x00, 0x60, 0x60, 0x00, 0x00 },   // .14
    { 0x00, 0x20, 0x10, 0x08, 0x04, 0x02 },   // /15
    { 0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E },   // 016
    { 0x00, 0x00, 0x42, 0x7F, 0x40, 0x00 },   // 117
    { 0x00, 0x42, 0x61, 0x51, 0x49, 0x46 },   // 218
    { 0x00, 0x21, 0x41, 0x45, 0x4B, 0x31 },   // 319
    { 0x00, 0x18, 0x14, 0x12, 0x7F, 0x10 },   // 420
    { 0x00, 0x27, 0x45, 0x45, 0x45, 0x39 },   // 521
    { 0x00, 0x3C, 0x4A, 0x49, 0x49, 0x30 },   // 622
    { 0x00, 0x01, 0x71, 0x09, 0x05, 0x03 },   // 723
    { 0x00, 0x36, 0x49, 0x49, 0x49, 0x36 },   // 824
    { 0x00, 0x06, 0x49, 0x49, 0x29, 0x1E },   // 925
    { 0x00, 0x00, 0x36, 0x36, 0x00, 0x00 },   // :26
    { 0x00, 0x00, 0x56, 0x36, 0x00, 0x00 },   // ;27
    { 0x00, 0x08, 0x14, 0x22, 0x41, 0x00 },   // <28
    { 0x00, 0x14, 0x14, 0x14, 0x14, 0x14 },   // =29
    { 0x00, 0x00, 0x41, 0x22, 0x14, 0x08 },   // >30
    { 0x00, 0x02, 0x01, 0x51, 0x09, 0x06 },   // ?31
    { 0x00, 0x32, 0x49, 0x59, 0x51, 0x3E },   // @32
    { 0x00, 0x7C, 0x12, 0x11, 0x12, 0x7C },   // A33
    { 0x00, 0x7F, 0x49, 0x49, 0x49, 0x36 },   // B34
    { 0x00, 0x3E, 0x41, 0x41, 0x41, 0x22 },   // C35
    { 0x00, 0x7F, 0x41, 0x41, 0x22, 0x1C },   // D36
    { 0x00, 0x7F, 0x49, 0x49, 0x49, 0x41 },   // E37
    { 0x00, 0x7F, 0x09, 0x09, 0x09, 0x01 },   // F38
    { 0x00, 0x3E, 0x41, 0x49, 0x49, 0x7A },   // G39
    { 0x00, 0x7F, 0x08, 0x08, 0x08, 0x7F },   // H40
    { 0x00, 0x00, 0x41, 0x7F, 0x41, 0x00 },   // I41
    { 0x00, 0x20, 0x40, 0x41, 0x3F, 0x01 },   // J42
    { 0x00, 0x7F, 0x08, 0x14, 0x22, 0x41 },   // K43
    { 0x00, 0x7F, 0x40, 0x40, 0x40, 0x40 },   // L44
    { 0x00, 0x7F, 0x02, 0x0C, 0x02, 0x7F },   // M45
    { 0x00, 0x7F, 0x04, 0x08, 0x10, 0x7F },   // N46
    { 0x00, 0x3E, 0x41, 0x41, 0x41, 0x3E },   // O47
    { 0x00, 0x7F, 0x09, 0x09, 0x09, 0x06 },   // P48
    { 0x00, 0x3E, 0x41, 0x51, 0x21, 0x5E },   // Q49
    { 0x00, 0x7F, 0x09, 0x19, 0x29, 0x46 },   // R50
    { 0x00, 0x46, 0x49, 0x49, 0x49, 0x31 },   // S51
    { 0x00, 0x01, 0x01, 0x7F, 0x01, 0x01 },   // T52
    { 0x00, 0x3F, 0x40, 0x40, 0x40, 0x3F },   // U53
    { 0x00, 0x1F, 0x20, 0x40, 0x20, 0x1F },   // V54
    { 0x00, 0x3F, 0x40, 0x38, 0x40, 0x3F },   // W55
    { 0x00, 0x63, 0x14, 0x08, 0x14, 0x63 },   // X56
    { 0x00, 0x07, 0x08, 0x70, 0x08, 0x07 },   // Y57
    { 0x00, 0x61, 0x51, 0x49, 0x45, 0x43 },   // Z58
    { 0x00, 0x00, 0x7F, 0x41, 0x41, 0x00 },   // [59
    { 0x00, 0x02, 0x04, 0x08, 0x10, 0x20 },   // \60
    { 0x00, 0x00, 0x41, 0x41, 0x7F, 0x00 },   // ]61
    { 0x00, 0x04, 0x02, 0x01, 0x02, 0x04 },   // ^62
    { 0x00, 0x40, 0x40, 0x40, 0x40, 0x40 },   // _63
    { 0x00, 0x00, 0x01, 0x02, 0x04, 0x00 },   // '64
    { 0x00, 0x20, 0x54, 0x54, 0x54, 0x78 },   // a65
    { 0x00, 0x7F, 0x48, 0x44, 0x44, 0x38 },   // b66
    { 0x00, 0x38, 0x44, 0x44, 0x44, 0x20 },   // c67
    { 0x00, 0x38, 0x44, 0x44, 0x48, 0x7F },   // d68
    { 0x00, 0x38, 0x54, 0x54, 0x54, 0x18 },   // e69
    { 0x00, 0x08, 0x7E, 0x09, 0x01, 0x02 },   // f70
    { 0x00, 0x18, 0xA4, 0xA4, 0xA4, 0x7C },   // g71
    { 0x00, 0x7F, 0x08, 0x04, 0x04, 0x78 },   // h72
    { 0x00, 0x00, 0x44, 0x7D, 0x40, 0x00 },   // i73
    { 0x00, 0x40, 0x80, 0x84, 0x7D, 0x00 },   // j74
    { 0x00, 0x7F, 0x10, 0x28, 0x44, 0x00 },   // k75
    { 0x00, 0x00, 0x41, 0x7F, 0x40, 0x00 },   // l76
    { 0x00, 0x7C, 0x04, 0x18, 0x04, 0x78 },   // m77
    { 0x00, 0x7C, 0x08, 0x04, 0x04, 0x78 },   // n78
    { 0x00, 0x38, 0x44, 0x44, 0x44, 0x38 },   // o79
    { 0x00, 0xFC, 0x24, 0x24, 0x24, 0x18 },   // p80
    { 0x00, 0x18, 0x24, 0x24, 0x18, 0xFC },   // q81
    { 0x00, 0x7C, 0x08, 0x04, 0x04, 0x08 },   // r82
    { 0x00, 0x48, 0x54, 0x54, 0x54, 0x20 },   // s83
    { 0x00, 0x04, 0x3F, 0x44, 0x40, 0x20 },   // t84
    { 0x00, 0x3C, 0x40, 0x40, 0x20, 0x7C },   // u85
    { 0x00, 0x1C, 0x20, 0x40, 0x20, 0x1C },   // v86
    { 0x00, 0x3C, 0x40, 0x30, 0x40, 0x3C },   // w87
    { 0x00, 0x44, 0x28, 0x10, 0x28, 0x44 },   // x88
    { 0x00, 0x1C, 0xA0, 0xA0, 0xA0, 0x7C },   // y89
    { 0x00, 0x44, 0x64, 0x54, 0x4C, 0x44 },   // z90
    { 0x14, 0x14, 0x14, 0x14, 0x14, 0x14 }    // horiz lines91
};

//中文字符串原字
const UCHAR8 F14x16_Idx[] = 
{
    "安联德科技屏幕测试"
};

//中文字符串编码
const UCHAR8 F14x16[] = 
{  
    0x09,0x92,0x46,0x00,0x47,0x51,0x49,0xC5,0xF1,0xC5,0x49,0x51,0x47,0x00,
    0x1E,0x01,0x00,0x04,0x04,0x02,0x01,0x00,0x1F,0x00,0x01,0x02,0x04,0x00,/*"深",0*/
    0x40,0x5C,0x44,0x44,0xC4,0x74,0x47,0x44,0x44,0xC4,0x44,0x5C,0x40,0x00,
    0x00,0x10,0x11,0x11,0x12,0x0A,0x04,0x06,0x09,0x08,0x10,0x10,0x00,0x00,/*"安",1*/
    0x01,0xFF,0x49,0x49,0xFF,0x41,0x49,0x4A,0xF8,0x4A,0x49,0x49,0x40,0x00,
    0x04,0x07,0x02,0x02,0x1F,0x12,0x08,0x06,0x01,0x06,0x08,0x10,0x10,0x00,/*"联",2*/
    0x98,0xC6,0x31,0x82,0xBA,0xAA,0xBE,0xAB,0xBA,0xAA,0xAA,0xBA,0x82,0x00,
    0x00,0x1F,0x08,0x06,0x00,0x0E,0x10,0x11,0x16,0x10,0x18,0x02,0x0C,0x00,/*"德",3*/
    0x00,0x00,0x48,0xFC,0x44,0x42,0x00,0x88,0x30,0x00,0xFE,0x02,0x00,0x00,
    0x00,0x0C,0x02,0x7F,0x01,0x01,0x08,0x08,0x05,0x04,0xFF,0x04,0x02,0x00,/*"科",4*/
    0x00,0x00,0x20,0xFE,0x20,0x90,0x20,0x20,0x20,0xFE,0x20,0xA0,0x10,0x00,
    0x00,0x02,0x41,0x7F,0x81,0x80,0x41,0x47,0x29,0x18,0x27,0x41,0x40,0x00,/*"技",5*/
    0x00,0x00,0xFE,0x14,0x14,0x34,0x54,0x14,0x94,0x74,0x14,0x9C,0x00,0x00,
    0x00,0x30,0x0F,0x88,0x48,0x39,0x0F,0x09,0x08,0x7F,0x09,0x04,0x04,0x00,/*"屏",6*/
    0x00,0x04,0x04,0xF4,0x54,0xCE,0x54,0x54,0x4E,0x4C,0xF4,0x04,0x04,0x00,
    0x00,0x08,0x0A,0x3E,0x0A,0x09,0x7E,0x0A,0x0A,0x7A,0x0E,0x0A,0x09,0x00,/*"幕",7*/
    0x00,0x20,0x04,0x80,0xFC,0x04,0xF4,0x04,0xFC,0x00,0xF0,0xFC,0x00,0x00,
    0x00,0x00,0x7F,0x40,0x27,0x18,0x07,0x10,0x67,0x00,0x4F,0xFF,0x00,0x00,/*"测",8*/
    0x00,0x80,0x88,0x98,0x20,0x20,0x20,0x20,0xA0,0xFE,0x24,0x2C,0x10,0x00,
    0x00,0x00,0x1F,0x60,0x10,0x28,0x21,0x1F,0x10,0x13,0x1C,0x20,0x70,0x00,/*"试",9*/
};

//======================================================
// 128X64OLED液晶底层驱动[8X16]字体库
// 设计者: Andy
// 描  述: [8X16]西文字符的字模数据 (纵向取模,字节倒序)
// !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
//======================================================
const UCHAR8 F8X16[]=
{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// 0
    0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x33,0x30,0x00,0x00,0x00,//!1
    0x00,0x10,0x0C,0x06,0x10,0x0C,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//"2
    0x40,0xC0,0x78,0x40,0xC0,0x78,0x40,0x00,0x04,0x3F,0x04,0x04,0x3F,0x04,0x04,0x00,//#3
    0x00,0x70,0x88,0xFC,0x08,0x30,0x00,0x00,0x00,0x18,0x20,0xFF,0x21,0x1E,0x00,0x00,//$4
    0xF0,0x08,0xF0,0x00,0xE0,0x18,0x00,0x00,0x00,0x21,0x1C,0x03,0x1E,0x21,0x1E,0x00,//%5
    0x00,0xF0,0x08,0x88,0x70,0x00,0x00,0x00,0x1E,0x21,0x23,0x24,0x19,0x27,0x21,0x10,//&6
    0x10,0x16,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//'7
    0x00,0x00,0x00,0xE0,0x18,0x04,0x02,0x00,0x00,0x00,0x00,0x07,0x18,0x20,0x40,0x00,//(8
    0x00,0x02,0x04,0x18,0xE0,0x00,0x00,0x00,0x00,0x40,0x20,0x18,0x07,0x00,0x00,0x00,//)9
    0x40,0x40,0x80,0xF0,0x80,0x40,0x40,0x00,0x02,0x02,0x01,0x0F,0x01,0x02,0x02,0x00,//*10
    0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x1F,0x01,0x01,0x01,0x00,//+11
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xB0,0x70,0x00,0x00,0x00,0x00,0x00,//,12
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,//-13
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,0x00,0x00,//.14
    0x00,0x00,0x00,0x00,0x80,0x60,0x18,0x04,0x00,0x60,0x18,0x06,0x01,0x00,0x00,0x00,///15
    0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x0F,0x10,0x20,0x20,0x10,0x0F,0x00,//016
    0x00,0x10,0x10,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//117
    0x00,0x70,0x08,0x08,0x08,0x88,0x70,0x00,0x00,0x30,0x28,0x24,0x22,0x21,0x30,0x00,//218
    0x00,0x30,0x08,0x88,0x88,0x48,0x30,0x00,0x00,0x18,0x20,0x20,0x20,0x11,0x0E,0x00,//319
    0x00,0x00,0xC0,0x20,0x10,0xF8,0x00,0x00,0x00,0x07,0x04,0x24,0x24,0x3F,0x24,0x00,//420
    0x00,0xF8,0x08,0x88,0x88,0x08,0x08,0x00,0x00,0x19,0x21,0x20,0x20,0x11,0x0E,0x00,//521
    0x00,0xE0,0x10,0x88,0x88,0x18,0x00,0x00,0x00,0x0F,0x11,0x20,0x20,0x11,0x0E,0x00,//622
    0x00,0x38,0x08,0x08,0xC8,0x38,0x08,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,//723
    0x00,0x70,0x88,0x08,0x08,0x88,0x70,0x00,0x00,0x1C,0x22,0x21,0x21,0x22,0x1C,0x00,//824
    0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x00,0x31,0x22,0x22,0x11,0x0F,0x00,//925
    0x00,0x00,0x00,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,//:26
    0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x60,0x00,0x00,0x00,0x00,//;27
    0x00,0x00,0x80,0x40,0x20,0x10,0x08,0x00,0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x00,//<28
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x00,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x00,//=29
    0x00,0x08,0x10,0x20,0x40,0x80,0x00,0x00,0x00,0x20,0x10,0x08,0x04,0x02,0x01,0x00,//>30
    0x00,0x70,0x48,0x08,0x08,0x08,0xF0,0x00,0x00,0x00,0x00,0x30,0x36,0x01,0x00,0x00,//?31
    0xC0,0x30,0xC8,0x28,0xE8,0x10,0xE0,0x00,0x07,0x18,0x27,0x24,0x23,0x14,0x0B,0x00,//@32
    0x00,0x00,0xC0,0x38,0xE0,0x00,0x00,0x00,0x20,0x3C,0x23,0x02,0x02,0x27,0x38,0x20,//A33
    0x08,0xF8,0x88,0x88,0x88,0x70,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x11,0x0E,0x00,//B34
    0xC0,0x30,0x08,0x08,0x08,0x08,0x38,0x00,0x07,0x18,0x20,0x20,0x20,0x10,0x08,0x00,//C35
    0x08,0xF8,0x08,0x08,0x08,0x10,0xE0,0x00,0x20,0x3F,0x20,0x20,0x20,0x10,0x0F,0x00,//D36
    0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x20,0x23,0x20,0x18,0x00,//E37
    0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x00,0x03,0x00,0x00,0x00,//F38
    0xC0,0x30,0x08,0x08,0x08,0x38,0x00,0x00,0x07,0x18,0x20,0x20,0x22,0x1E,0x02,0x00,//G39
    0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x20,0x3F,0x21,0x01,0x01,0x21,0x3F,0x20,//H40
    0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//I41
    0x00,0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0xC0,0x80,0x80,0x80,0x7F,0x00,0x00,0x00,//J42
    0x08,0xF8,0x88,0xC0,0x28,0x18,0x08,0x00,0x20,0x3F,0x20,0x01,0x26,0x38,0x20,0x00,//K43
    0x08,0xF8,0x08,0x00,0x00,0x00,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x20,0x30,0x00,//L44
    0x08,0xF8,0xF8,0x00,0xF8,0xF8,0x08,0x00,0x20,0x3F,0x00,0x3F,0x00,0x3F,0x20,0x00,//M45
    0x08,0xF8,0x30,0xC0,0x00,0x08,0xF8,0x08,0x20,0x3F,0x20,0x00,0x07,0x18,0x3F,0x00,//N46
    0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,0x0F,0x10,0x20,0x20,0x20,0x10,0x0F,0x00,//O47
    0x08,0xF8,0x08,0x08,0x08,0x08,0xF0,0x00,0x20,0x3F,0x21,0x01,0x01,0x01,0x00,0x00,//P48
    0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,0x0F,0x18,0x24,0x24,0x38,0x50,0x4F,0x00,//Q49
    0x08,0xF8,0x88,0x88,0x88,0x88,0x70,0x00,0x20,0x3F,0x20,0x00,0x03,0x0C,0x30,0x20,//R50
    0x00,0x70,0x88,0x08,0x08,0x08,0x38,0x00,0x00,0x38,0x20,0x21,0x21,0x22,0x1C,0x00,//S51
    0x18,0x08,0x08,0xF8,0x08,0x08,0x18,0x00,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00,//T52
    0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00,//U53
    0x08,0x78,0x88,0x00,0x00,0xC8,0x38,0x08,0x00,0x00,0x07,0x38,0x0E,0x01,0x00,0x00,//V54
    0xF8,0x08,0x00,0xF8,0x00,0x08,0xF8,0x00,0x03,0x3C,0x07,0x00,0x07,0x3C,0x03,0x00,//W55
    0x08,0x18,0x68,0x80,0x80,0x68,0x18,0x08,0x20,0x30,0x2C,0x03,0x03,0x2C,0x30,0x20,//X56
    0x08,0x38,0xC8,0x00,0xC8,0x38,0x08,0x00,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00,//Y57
    0x10,0x08,0x08,0x08,0xC8,0x38,0x08,0x00,0x20,0x38,0x26,0x21,0x20,0x20,0x18,0x00,//Z58
    0x00,0x00,0x00,0xFE,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x7F,0x40,0x40,0x40,0x00,//[59
    0x00,0x0C,0x30,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x06,0x38,0xC0,0x00,//\60
    0x00,0x02,0x02,0x02,0xFE,0x00,0x00,0x00,0x00,0x40,0x40,0x40,0x7F,0x00,0x00,0x00,//]61
    0x00,0x00,0x04,0x02,0x02,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//^62
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,//_63
    0x00,0x02,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//`64
    0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x19,0x24,0x22,0x22,0x22,0x3F,0x20,//a65
    0x08,0xF8,0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x3F,0x11,0x20,0x20,0x11,0x0E,0x00,//b66
    0x00,0x00,0x00,0x80,0x80,0x80,0x00,0x00,0x00,0x0E,0x11,0x20,0x20,0x20,0x11,0x00,//c67
    0x00,0x00,0x00,0x80,0x80,0x88,0xF8,0x00,0x00,0x0E,0x11,0x20,0x20,0x10,0x3F,0x20,//d68
    0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x22,0x22,0x22,0x22,0x13,0x00,//e69
    0x00,0x80,0x80,0xF0,0x88,0x88,0x88,0x18,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//f70
    0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x6B,0x94,0x94,0x94,0x93,0x60,0x00,//g71
    0x08,0xF8,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20,//h72
    0x00,0x80,0x98,0x98,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//i73
    0x00,0x00,0x00,0x80,0x98,0x98,0x00,0x00,0x00,0xC0,0x80,0x80,0x80,0x7F,0x00,0x00,//j74
    0x08,0xF8,0x00,0x00,0x80,0x80,0x80,0x00,0x20,0x3F,0x24,0x02,0x2D,0x30,0x20,0x00,//k75
    0x00,0x08,0x08,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//l76
    0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x20,0x3F,0x20,0x00,0x3F,0x20,0x00,0x3F,//m77
    0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20,//n78
    0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00,//o79
    0x80,0x80,0x00,0x80,0x80,0x00,0x00,0x00,0x80,0xFF,0xA1,0x20,0x20,0x11,0x0E,0x00,//p80
    0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x0E,0x11,0x20,0x20,0xA0,0xFF,0x80,//q81
    0x80,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x20,0x20,0x3F,0x21,0x20,0x00,0x01,0x00,//r82
    0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x33,0x24,0x24,0x24,0x24,0x19,0x00,//s83
    0x00,0x80,0x80,0xE0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x1F,0x20,0x20,0x00,0x00,//t84
    0x80,0x80,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x1F,0x20,0x20,0x20,0x10,0x3F,0x20,//u85
    0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x00,0x01,0x0E,0x30,0x08,0x06,0x01,0x00,//v86
    0x80,0x80,0x00,0x80,0x00,0x80,0x80,0x80,0x0F,0x30,0x0C,0x03,0x0C,0x30,0x0F,0x00,//w87
    0x00,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x31,0x2E,0x0E,0x31,0x20,0x00,//x88
    0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x80,0x81,0x8E,0x70,0x18,0x06,0x01,0x00,//y89
    0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x21,0x30,0x2C,0x22,0x21,0x30,0x00,//z90
    0x00,0x00,0x00,0x00,0x80,0x7C,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x3F,0x40,0x40,//{91
    0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,//|92
    0x00,0x02,0x02,0x7C,0x80,0x00,0x00,0x00,0x00,0x40,0x40,0x3F,0x00,0x00,0x00,0x00,//}93
    0x00,0x06,0x01,0x01,0x02,0x02,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//~94
};

/*----------------------------------------------------------------------------*
 * 内部函数原型                                                               *
 *----------------------------------------------------------------------------*/
VOID LED_WrDat(UCHAR8 ucData);
VOID LED_WrCmd(UCHAR8 ucCmd);
VOID LED_SetPos(UCHAR8 ucIdxX, UCHAR8 ucIdxY);
VOID SetStartColumn(UCHAR8 ucData);
VOID SetAddressingMode(UCHAR8 ucData);
VOID SetColumnAddress(UCHAR8 a, UCHAR8 b);
VOID SetPageAddress(UCHAR8 a, UCHAR8 b);
VOID SetStartLine(UCHAR8 d);
VOID SetContrastControl(UCHAR8 d);
VOID SetChargePump(UCHAR8 d);
VOID SetSegmentRemap(UCHAR8 d);
VOID SetEntireDisplay(UCHAR8 d);
VOID SetInverseDisplay(UCHAR8 d);
VOID SetMultiplexRatio(UCHAR8 d);
VOID SetDisplayOnOff(UCHAR8 d);
VOID SetStartPage(UCHAR8 d);
VOID SetCommonRemap(UCHAR8 d);
VOID SetDisplayOffset(UCHAR8 d);
VOID SetDisplayClock(UCHAR8 d);
VOID SetPrechargePeriod(UCHAR8 d);
VOID SetCommonConfig(UCHAR8 d);
VOID SetVCOMH(UCHAR8 d);
VOID SetNop(VOID);

/*----------------------------------------------------------------------------*
 * 外部函数原型                                                               *
 *----------------------------------------------------------------------------*/

/*****************************************************************************
 函 数 名  : LED_WrDat
 功能描述  : 向OLED写数据
 输入参数  : UCHAR8 ucData  
 输出参数  : NONE
 返 回 值  : NONE
*****************************************************************************/
VOID LED_WrDat(UCHAR8 ucData)
{
    UCHAR8 i = 8;
    LED_DC   = 1;
    LED_SCL  = 0;
    
    while (i--)
    {
        if (ucData & 0x80)
        {
            LED_SDA = 1;
        }
        else
        {
            LED_SDA = 0;
        }
        
        LED_SCL = 1; 
        asm("nop"); 
        LED_SCL = 0;    
        ucData <<= 1;    
    }

    return;
}

/*****************************************************************************
 函 数 名  : LED_WrCmd
 功能描述  : 向OLED写命令
 输入参数  : UCHAR8 ucCmd  
 输出参数  : NONE
 返 回 值  : NONE
*****************************************************************************/
VOID LED_WrCmd(UCHAR8 ucCmd)
{
    UCHAR8 i = 8;
    LED_DC   = 0;
    LED_SCL  = 0;
    while (i--)
    {
        if (ucCmd & 0x80)
        {
            LED_SDA = 1;
        }
        else
        {
            LED_SDA = 0;
        }
        LED_SCL = 1;
        asm("nop");           
        LED_SCL = 0;    
        ucCmd <<= 1;   
    }  

    return;
}


/*****************************************************************************
 函 数 名  : LED_SetPos
 功能描述  : 设置坐标
 输入参数  : UCHAR8 ucIdxX  
             UCHAR8 ucIdxY  
 输出参数  : NONE
 返 回 值  : NONE
*****************************************************************************/
VOID LED_SetPos(UCHAR8 ucIdxX, UCHAR8 ucIdxY)
{ 
    LED_WrCmd(0xb0 + ucIdxY);
    LED_WrCmd(((ucIdxX & 0xf0) >> 4) | 0x10);
    LED_WrCmd((ucIdxX & 0x0f) | 0x00); 
} 

/*****************************************************************************
 函 数 名  : LED_Fill
 功能描述  : 对全屏写入同一个字符函数 
             如 LED_Fill(0x01);    对于某一位0为不亮 1为亮
            ------------------------------------------------------
                                共128列
                  1           1                1
              共  0           0                0
              8   0           0                0
              组  0       ……0     …………   0
              字  0           0                0
              符  0           0                0
              即  0           0                0
              8   0           0                0
              页  1           1                1
              64  0           0                0
              行  .           .                .
                  .           .    …………    .
                  .           .                . 
                  .           .                .
                  0           0                0
            ------------------------------------------------------
 输入参数  : UCHAR8 ucData  
 输出参数  : NONE
 返 回 值  : NONE
*****************************************************************************/
VOID LED_Fill(UCHAR8 ucData)
{
    UCHAR8 ucPage,ucColumn;
    
    for(ucPage = 0; ucPage < 8; ucPage++)
    {
        LED_WrCmd(0xb0 + ucPage);  //0xb0+0~7表示页0~7
        LED_WrCmd(0x00);           //0x00+0~16表示将128列分成16组其地址在某组中的第几列
        LED_WrCmd(0x10);           //0x10+0~16表示将128列分成16组其地址所在第几组
        for(ucColumn = 0; ucColumn < 128; ucColumn++)
        {
            LED_WrDat(ucData);
        }
    }
} 

VOID SetStartColumn(UCHAR8 ucData)
{
    LED_WrCmd(0x00+ucData % 16);   // Set Lower Column Start Address for Page Addressing Mode
                                   // Default => 0x00
    LED_WrCmd(0x10+ucData / 16);   // Set Higher Column Start Address for Page Addressing Mode
                                   // Default => 0x10
}

VOID SetAddressingMode(UCHAR8 ucData)
{
    LED_WrCmd(0x20);        // Set Memory Addressing Mode
    LED_WrCmd(ucData);      // Default => 0x02
                            // 0x00 => Horizontal Addressing Mode
                            // 0x01 => Vertical Addressing Mode
                            // 0x02 => Page Addressing Mode
}

VOID SetColumnAddress(UCHAR8 a, UCHAR8 b)
{
    LED_WrCmd(0x21);        // Set Column Address
    LED_WrCmd(a);           // Default => 0x00 (Column Start Address)
    LED_WrCmd(b);           // Default => 0x7F (Column End Address)
}

VOID SetPageAddress(UCHAR8 a, UCHAR8 b)
{
    LED_WrCmd(0x22);        // Set Page Address
    LED_WrCmd(a);           // Default => 0x00 (Page Start Address)
    LED_WrCmd(b);           // Default => 0x07 (Page End Address)
}

VOID SetStartLine(UCHAR8 ucData)
{
    LED_WrCmd(0x40|ucData); // Set Display Start Line
                            // Default => 0x40 (0x00)
}

VOID SetContrastControl(UCHAR8 ucData)
{
    LED_WrCmd(0x81);        // Set Contrast Control
    LED_WrCmd(ucData);      // Default => 0x7F
}

VOID SetChargePump(UCHAR8 ucData)
{
    LED_WrCmd(0x8D);        // Set Charge Pump
    LED_WrCmd(0x10|ucData); // Default => 0x10
                            // 0x10 (0x00) => Disable Charge Pump
                            // 0x14 (0x04) => Enable Charge Pump
}

VOID SetSegmentRemap(UCHAR8 ucData)
{
    LED_WrCmd(0xA0|ucData); // Set Segment Re-Map
                            // Default => 0xA0
                            // 0xA0 (0x00) => Column Address 0 Mapped to SEG0
                            // 0xA1 (0x01) => Column Address 0 Mapped to SEG127
}

VOID SetEntireDisplay(UCHAR8 ucData)
{
    LED_WrCmd(0xA4|ucData); // Set Entire Display On / Off
                            // Default => 0xA4
                            // 0xA4 (0x00) => Normal Display
                            // 0xA5 (0x01) => Entire Display On
}

VOID SetInverseDisplay(UCHAR8 ucData)
{
    LED_WrCmd(0xA6|ucData); // Set Inverse Display On/Off
                            // Default => 0xA6
                            // 0xA6 (0x00) => Normal Display
                            // 0xA7 (0x01) => Inverse Display On
}

VOID SetMultiplexRatio(UCHAR8 ucData)
{
    LED_WrCmd(0xA8);        // Set Multiplex Ratio
    LED_WrCmd(ucData);      // Default => 0x3F (1/64 Duty)
}

VOID SetDisplayOnOff(UCHAR8 ucData)
{
    LED_WrCmd(0xAE|ucData); // Set Display On/Off
                            // Default => 0xAE
                            // 0xAE (0x00) => Display Off
                            // 0xAF (0x01) => Display On
}

VOID SetStartPage(UCHAR8 ucData)
{
    LED_WrCmd(0xB0|ucData); // Set Page Start Address for Page Addressing Mode
                            // Default => 0xB0 (0x00)
}

VOID SetCommonRemap(UCHAR8 ucData)
{
    LED_WrCmd(0xC0|ucData); // Set COM Output Scan Direction
                            // Default => 0xC0
                            // 0xC0 (0x00) => Scan from COM0 to 63
                            // 0xC8 (0x08) => Scan from COM63 to 0
}

VOID SetDisplayOffset(UCHAR8 ucData)
{
    LED_WrCmd(0xD3);        // Set Display Offset
    LED_WrCmd(ucData);      // Default => 0x00
}

VOID SetDisplayClock(UCHAR8 ucData)
{
    LED_WrCmd(0xD5);        // Set Display Clock Divide Ratio / Oscillator Frequency
    LED_WrCmd(ucData);      // Default => 0x80
                            // D[3:0] => Display Clock Divider
                            // D[7:4] => Oscillator Frequency
}

VOID SetPrechargePeriod(UCHAR8 ucData)
{
    LED_WrCmd(0xD9);        // Set Pre-Charge Period
    LED_WrCmd(ucData);      // Default => 0x22 (2 Display Clocks [Phase 2] / 2 Display Clocks [Phase 1])
                            // D[3:0] => Phase 1 Period in 1~15 Display Clocks
                            // D[7:4] => Phase 2 Period in 1~15 Display Clocks
}

VOID SetCommonConfig(UCHAR8 ucData)
{
    LED_WrCmd(0xDA);        // Set COM Pins Hardware Configuration
    LED_WrCmd(0x02|ucData); // Default => 0x12 (0x10)
                            // Alternative COM Pin Configuration
                            // Disable COM Left/Right Re-Map
}

VOID SetVCOMH(UCHAR8 ucData)
{
    LED_WrCmd(0xDB);        // Set VCOMH Deselect Level
    LED_WrCmd(ucData);      // Default => 0x20 (0.77*VCC)
}

VOID SetNop(VOID)
{
    LED_WrCmd(0xE3);        // Command for No Operation
}

/*****************************************************************************
 函 数 名  : LED_Init
 功能描述  : OLED初始化
 输入参数  : VOID  
 输出参数  : NONE
 返 回 值  : NONE
*****************************************************************************/
VOID LED_Init(VOID)        
{
    UCHAR8 i;
    LED_PORT=0X0F;
    LED_SCL=1;      
    LED_RST=0;
    
    for(i=0;i<100;i++)
    {
        asm("nop");            //从上电到下面开始初始化要有足够的时间，即等待RC复位完毕      
    }
    
    LED_RST=1;

    SetDisplayOnOff(0x00);     // Display Off (0x00/0x01)
    SetDisplayClock(0x80);     // Set Clock as 100 Frames/Sec
    SetMultiplexRatio(0x3F);   // 1/64 Duty (0x0F~0x3F)
    SetDisplayOffset(0x00);    // Shift Mapping RAM Counter (0x00~0x3F)
    SetStartLine(0x00);        // Set Mapping RAM Display Start Line (0x00~0x3F)
    SetChargePump(0x04);       // Enable Embedded DC/DC Converter (0x00/0x04)
    SetAddressingMode(0x02);   // Set Page Addressing Mode (0x00/0x01/0x02)
    SetSegmentRemap(0x01);     // Set SEG/Column Mapping     0x00左右反置 0x01正常
    SetCommonRemap(0x08);      // Set COM/Row Scan Direction 0x00上下反置 0x08正常
    SetCommonConfig(0x10);     // Set Sequential Configuration (0x00/0x10)
    SetContrastControl(0xCF);  // Set SEG Output Current
    SetPrechargePeriod(0xF1);  // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    SetVCOMH(0x40);            // Set VCOM Deselect Level
    SetEntireDisplay(0x00);    // Disable Entire Display On (0x00/0x01)
    SetInverseDisplay(0x00);   // Disable Inverse Display On (0x00/0x01)  
    SetDisplayOnOff(0x01);     // Display On (0x00/0x01)
    LED_Fill(0x00);            // 初始清屏
    LED_SetPos(0,0);

    return;
} 
 
/*****************************************************************************
 函 数 名  : LED_P6x8Char
 功能描述  : 显示一个6x8标准ASCII字符
 输入参数  : UCHAR8 ucIdxX  显示的横坐标0~122
             UCHAR8 ucIdxY  页范围0～7
             UCHAR8 ucData  显示的字符
 输出参数  : NONE
 返 回 值  : NONE
*****************************************************************************/
VOID LED_P6x8Char(UCHAR8 ucIdxX, UCHAR8 ucIdxY, UCHAR8 ucData)
{
    UCHAR8 i, ucDataTmp;     
       
    ucDataTmp = ucData-32;
    if(ucIdxX > 122)
    {
        ucIdxX = 0;
        ucIdxY++;
    }
    
    LED_SetPos(ucIdxX, ucIdxY);
    
    for(i = 0; i < 6; i++)
    {     
        LED_WrDat(F6x8[ucDataTmp][i]);  
    }
}

/*****************************************************************************
 函 数 名  : LED_P6x8Str
 功能描述  : 写入一组6x8标准ASCII字符串
 输入参数  : UCHAR8 ucIdxX       显示的横坐标0~122
             UCHAR8 ucIdxY       页范围0～7
             UCHAR8 ucDataStr[]  显示的字符串
 输出参数  : NONE
 返 回 值  : NONE
*****************************************************************************/
VOID LED_P6x8Str(UCHAR8 ucIdxX, UCHAR8 ucIdxY, UCHAR8 ucDataStr[])
{
    UCHAR8 i, j, ucDataTmp; 

    for (j = 0; ucDataStr[j] != '\0'; j++)
    {    
        ucDataTmp = ucDataStr[j] - 32;
        if(ucIdxX > 122)
        {
            ucIdxX = 0;
            ucIdxY++;
        }
        
        LED_SetPos(ucIdxX,ucIdxY); 
        
        for(i = 0; i < 6; i++)
        {     
            LED_WrDat(F6x8[ucDataTmp][i]);  
        }
        ucIdxX += 6;
    }

    return;
}

/*****************************************************************************
 函 数 名  : LED_P8x16Str
 功能描述  : 写入一组8x16标准ASCII字符串
 输入参数  : UCHAR8 ucIdxX       为显示的横坐标0~120
             UCHAR8 ucIdxY       为页范围0～3
             UCHAR8 ucDataStr[]  要显示的字符串
 输出参数  : NONE
 返 回 值  : NONE
*****************************************************************************/
VOID LED_P8x16Str(UCHAR8 ucIdxX, UCHAR8 ucIdxY, UCHAR8 ucDataStr[])
{
    UCHAR8 i, j, ucDataTmp;

    ucIdxY <<= 1;
    
    for (j = 0; ucDataStr[j] != '\0'; j++)
    {    
        ucDataTmp = ucDataStr[j] - 32;
        if(ucIdxX > 120)
        {
            ucIdxX = 0;
            ucIdxY += 2;
        }
        LED_SetPos(ucIdxX, ucIdxY);   
        
        for(i = 0; i < 8; i++) 
        {
            LED_WrDat(F8X16[(ucDataTmp << 4) + i]);
        }
        
        LED_SetPos(ucIdxX, ucIdxY + 1);   
        
        for(i = 0; i < 8; i++) 
        {
            LED_WrDat(F8X16[(ucDataTmp << 4) + i + 8]);
        }
        ucIdxX += 8;
        
    }

    return;
}

/*****************************************************************************
 函 数 名  : LED_P14x16Str
 功能描述  : 写入一组14x16的中文字符串（字符串表格中需含有此字）
 输入参数  : UCHAR8 ucIdxX       为显示的横坐标0~114
             UCHAR8 ucIdxY       为页范围0～3
             UCHAR8 ucDataStr[]  要显示的中文字符串
 输出参数  : NONE
 返 回 值  : NONE
*****************************************************************************/
VOID LED_P14x16Str(UCHAR8 ucIdxX,UCHAR8 ucIdxY,UCHAR8 ucDataStr[])
{
    UCHAR8 i, j;
    USHORT16 usAddress; 
    
    ucIdxY <<= 1;
    
    j = 0;
    while(ucDataStr[j] != '\0')
    {
        i = 0;
        usAddress = 1;
        while(F14x16_Idx[i] > 127)
        {
            if(F14x16_Idx[i] == ucDataStr[j])
            {
                if(F14x16_Idx[i + 1] == ucDataStr[j + 1])
                {
                    usAddress = i * 14;
                    break;
                }
            }
            i += 2;            
        }
        
        if(ucIdxX > 114)
        {
            ucIdxX = 0;
            ucIdxY += 2;
        }
        
        if(usAddress != 1)// 显示汉字                   
        {
            LED_SetPos(ucIdxX, ucIdxY);
            for(i = 0; i < 14; i++)               
            {
                LED_WrDat(F14x16[usAddress]);   
                usAddress++;
            }
            
            LED_SetPos(ucIdxX,ucIdxY + 1); 
            for(i = 0;i < 14;i++)          
            {
                LED_WrDat(F14x16[usAddress]);
                usAddress++;
            }
            
            j += 2;        
        }
        else              //显示空白字符            
        {
            LED_SetPos(ucIdxX,ucIdxY);
            for(i = 0;i < 14;i++)
            {
                LED_WrDat(0);
            }
            
            LED_SetPos(ucIdxX,ucIdxY + 1);
            for(i = 0;i < 14;i++)
            {           
                LED_WrDat(0);   
            }
            
            j++;
        }
        
        ucIdxX += 14;
    }

    return;
}

/*****************************************************************************
 函 数 名  : LED_PXx16MixStr
 功能描述  : 输出14x16汉字和字符混合字符串 （字符串表格中需含有此字）
 输入参数  : UCHAR8 ucIdxX       为显示的横坐标0~114
             UCHAR8 ucIdxY       为页范围0～3
             UCHAR8 ucDataStr[]  要显示的中文字符串
 输出参数  : NONE
 返 回 值  : NONE
*****************************************************************************/
VOID LED_PXx16MixStr(UCHAR8 ucIdxX, UCHAR8 ucIdxY, UCHAR8 ucDataStr[])
{
    UCHAR8 ucTmpStr[3];
    UCHAR8 i = 0;    
    
    while(ucDataStr[i] != '\0')
    {
        if(ucDataStr[i] > 127)
        {
            ucTmpStr[0] = ucDataStr[i];
            ucTmpStr[1] = ucDataStr[i + 1];
            ucTmpStr[2] = '\0';          //汉字为两个字节
            LED_P14x16Str(ucIdxX, ucIdxY, ucTmpStr);  //显示汉字
            ucIdxX += 14;
            i += 2;
        }
        else
        {
            ucTmpStr[0] = ucDataStr[i];    
            ucTmpStr[1] = '\0';          //字母占一个字节
            LED_P8x16Str(ucIdxX, ucIdxY, ucTmpStr);  //显示字母
            ucIdxX += 8;
            i += 1;
        }
    }

    return;
} 

/*****************************************************************************
 函 数 名  : LED_PrintChar
 功能描述  : 将一个Char型数转换成3位数进行显示
 输入参数  : UCHAR8 ucIdxX    ucIdxX的范围为0～122
             UCHAR8 ucIdxY    ucIdxY的范围为0～7
             CHAR8 cData      cData为需要转化显示的数值 -128~127
 输出参数  : none
 返 回 值  : none
*****************************************************************************/
VOID LED_PrintChar(UCHAR8 ucIdxX, UCHAR8 ucIdxY, CHAR8 cData)
{
    USHORT16 i, j, k, usData;

    if(cData < 0)
    {
        LED_P6x8Char(ucIdxX, ucIdxY, '-');
        usData = (USHORT16)(-cData);  
    }
    else
    {
        LED_P6x8Char(ucIdxX, ucIdxY, '+');
        usData = (USHORT16)cData;
    }
    i = usData / 100;
    j = (usData % 100) / 10;
    k = usData % 10;
    
    LED_P6x8Char(ucIdxX+6, ucIdxY, i+48);
    LED_P6x8Char(ucIdxX+12, ucIdxY, j+48);
    LED_P6x8Char(ucIdxX+18, ucIdxY, k+48);   

    return;
}

/*****************************************************************************
 函 数 名  : LED_PrintShort
 功能描述  : 将一个Short型数转换成5位数进行显示
 输入参数  : UCHAR8 ucIdxX ucIdxX的范围为0～120
             UCHAR8 ucIdxY ucIdxY为页的范围0～7
             SHORT16 sData  sData为需要转化显示的数值
 输出参数  : none
 返 回 值  : none
*****************************************************************************/
VOID LED_PrintShort(UCHAR8 ucIdxX, UCHAR8 ucIdxY, SHORT16 sData)
{
    USHORT16 i, j, k, l, m, usData;  
    if(sData < 0)
    {
        LED_P6x8Char(ucIdxX,ucIdxY,'-');
        usData = (USHORT16)(-sData);  
    }
    else
    {
        LED_P6x8Char(ucIdxX,ucIdxY,'+');
        usData = (USHORT16)sData;
    }
    
    l = usData / 10000;
    m = (usData % 10000) /1000;
    i = (usData % 1000) / 100;
    j = (usData % 100) / 10;
    k = usData % 10;
    LED_P6x8Char(ucIdxX+6, ucIdxY, l+48);
    LED_P6x8Char(ucIdxX+12, ucIdxY, m+48);
    LED_P6x8Char(ucIdxX+18, ucIdxY, i+48);
    LED_P6x8Char(ucIdxX+24, ucIdxY, j+48);
    LED_P6x8Char(ucIdxX+30, ucIdxY, k+48);  

    return;
}

/*****************************************************************************
 函 数 名  : LED_PrintImage
 功能描述  : 将图像显示出来
 输入参数  : UCHAR8 *pucTable     二维图像数组的地址
             USHORT16 usRowNum    二维图像的行数1~64
             USHORT16 usColumnNum 二维图像的列数1~128
 输出参数  : none
 返 回 值  : none
*****************************************************************************/
VOID LED_PrintImage(UCHAR8 *pucTable, USHORT16 usRowNum, USHORT16 usColumnNum)
{
    UCHAR8 ucData;
    USHORT16 i,j,k,m,n;
    USHORT16 usRowTmp;

    m = usRowNum >> 3;   //计算图片行数以8位为一组完整的组数
    n = usRowNum % 8;    //计算分完组后剩下的行数
    
    for(i = 0; i < m; i++) //完整组行扫描
    {
        LED_SetPos(0,(UCHAR8)i);
        usRowTmp = i << 3;    //计算当前所在行的下标                  
        for(j = 0; j < usColumnNum; j++) //列扫描        
        {
            ucData = 0;
            for(k = 0; k < 8; k++) //在i组中对这8行扫描
            {
                ucData = ucData >> 1;
                if((pucTable + (usRowTmp + k) * usColumnNum)[j] == LED_IMAGE_WHITE)
                {
                    ucData = ucData | 0x80;
                }
                
            }
            LED_WrDat(ucData);
        }
    }
    
    LED_SetPos(0,(UCHAR8)i); //设置剩下的行显示的起始坐标
    usRowTmp = i << 3;       //计算当前所在行的下标                  
    for(j = 0; j < usColumnNum; j++) //列扫描        
    {
        ucData = 0;
        for(k = 0; k < n; k++) //对剩下的行扫描
        {
            ucData = ucData >> 1;
            if((pucTable + (usRowTmp + k) * usColumnNum)[j] == LED_IMAGE_WHITE)
            {
                ucData = ucData | 0x80;
            }
            
        }
        ucData = ucData >> (8 - n);
        LED_WrDat(ucData);
    }

    return;
}

