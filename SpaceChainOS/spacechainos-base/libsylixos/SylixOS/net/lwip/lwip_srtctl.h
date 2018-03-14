/*********************************************************************************************************
**
**                                    �й�������Դ��֯
**
**                                   Ƕ��ʽʵʱ����ϵͳ
**
**                                SylixOS(TM)  LW : long wing
**
**                               Copyright All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: lwip_srtctl.h
**
** ��   ��   ��: Han.Hui (����)
**
** �ļ���������: 2017 �� 12 �� 08 ��
**
** ��        ��: ioctl Դ·�ɱ�֧��.
*********************************************************************************************************/

#ifndef __LWIP_SRTCTL_H
#define __LWIP_SRTCTL_H

/*********************************************************************************************************
  �ü�����
*********************************************************************************************************/
#if LW_CFG_NET_EN > 0 && LW_CFG_NET_ROUTER > 0 && LW_CFG_NET_BALANCING > 0

INT  __srtIoctlInet(INT  iFamily, INT  iCmd, PVOID  pvArg);

#endif                                                                  /*  LW_CFG_NET_EN > 0           */
                                                                        /*  LW_CFG_NET_ROUTER > 0       */
                                                                        /*  LW_CFG_NET_BALANCING > 0    */
#endif                                                                  /*  __LWIP_SRTCTL_H             */
/*********************************************************************************************************
  END
*********************************************************************************************************/