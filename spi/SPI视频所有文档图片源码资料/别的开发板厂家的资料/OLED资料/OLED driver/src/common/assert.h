/******************** (C) COPYRIGHT 2011 ������Ƕ��ʽ���� ********************
 * �ļ���       ��assert.h
 * ����         �����Ժ궨��ͷ�ļ�
 *
 * ʵ��ƽ̨     �������¿�����
 * ��汾       ��
 * Ƕ��ϵͳ     ��
 * ��    ע     ��assert_failed() �� assert.c �ﶨ��
 *
 * ����         ��
 * �Ա���       ��http://aldsz.taobao.com
 * ����֧����̳ ��
**********************************************************************************/


#ifndef _ASSERT_H_
#define _ASSERT_H_

/********************************************************************/

void assert_failed(char *, int);

#ifdef DEBUG
#define ASSERT(expr) \
    if (!(expr)) \
        assert_failed(__FILE__, __LINE__)
#else
#define ASSERT(expr)
#endif

/********************************************************************/
#endif /* _ASSERT_H_ */

