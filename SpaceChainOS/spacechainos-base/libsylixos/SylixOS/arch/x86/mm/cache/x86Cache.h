/*********************************************************************************************************
**
**                                    �й�������Դ��֯
**
**                                   Ƕ��ʽʵʱ����ϵͳ
**
**                                       SylixOS(TM)
**
**                               Copyright  All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: x86Cache.h
**
** ��   ��   ��: Jiao.JinXing (������)
**
** �ļ���������: 2016 �� 07 �� 18 ��
**
** ��        ��: x86 ��ϵ���� CACHE ����.
*********************************************************************************************************/

#ifndef __ARCH_X86CACHE_H
#define __ARCH_X86CACHE_H

VOID  x86CacheInit(LW_CACHE_OP *pcacheop,
                   CACHE_MODE   uiInstruction,
                   CACHE_MODE   uiData,
                   CPCHAR       pcMachineName);
                      
VOID  x86CacheReset(CPCHAR  pcMachineName);

#endif                                                                  /*  __ARCH_X86CACHE_H           */
/*********************************************************************************************************
  END
*********************************************************************************************************/