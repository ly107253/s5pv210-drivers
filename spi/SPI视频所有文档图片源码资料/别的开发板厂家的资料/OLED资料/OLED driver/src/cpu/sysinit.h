/******************** (C) COPYRIGHT 2011 ������Ƕ��ʽ���� ********************
 * �ļ���       ��sysinit.h
 * ����         ��ϵͳʱ��������غ�����ͷ�ļ�
 * ��ע         ��
 *
 * ʵ��ƽ̨     �������¿�����
 * ��汾       ��
 * Ƕ��ϵͳ     ��
 *
 * ����         ��
 * �Ա���       ��http://aldsz.taobao.com
 * ����֧����̳ ��
**********************************************************************************/

#ifndef    _SYSINIT_H_
#define    _SYSINIT_H_
typedef 	unsigned 	long  int	u32;
/********************************************************************/
extern u32 core_clk_khz;
extern u32 core_clk_mhz;
extern u32 bus_clk_khz;



// function prototypes
void sysinit (void);
void trace_clk_init(void);
void fb_clk_init(void);
void enable_abort_button(void);



/********************************************************************/

#endif  //_SYSINIT_H_