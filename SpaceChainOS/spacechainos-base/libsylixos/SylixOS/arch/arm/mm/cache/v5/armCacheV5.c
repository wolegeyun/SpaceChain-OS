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
** ��   ��   ��: armCacheV5.c
**
** ��   ��   ��: Han.Hui (����)
**
** �ļ���������: 2013 �� 12 �� 09 ��
**
** ��        ��: ARMv5 ��ϵ���� CACHE ����.
**
** BUG:
2015.08.21  ���� Invalidate ����������ַ�������.
2016.04.29  ���� data update ����.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
/*********************************************************************************************************
  ARM926 ��ϵ����
*********************************************************************************************************/
#if !defined(__SYLIXOS_ARM_ARCH_M__)
/*********************************************************************************************************
  �ü�֧��
*********************************************************************************************************/
#if LW_CFG_CACHE_EN > 0
#include "../armCacheCommon.h"
#include "../../mmu/armMmuCommon.h"
/*********************************************************************************************************
  ��������
*********************************************************************************************************/
extern VOID  armDCacheV5Disable(VOID);
extern VOID  armDCacheV5FlushAll(VOID);
extern VOID  armDCacheV5ClearAll(VOID);
/*********************************************************************************************************
  CACHE ����
*********************************************************************************************************/
#define ARMv5_CACHE_LINE_SIZE           32
#define ARMv5_CACHE_LOOP_OP_MAX_SIZE    (16 * LW_CFG_KB_SIZE)
/*********************************************************************************************************
** ��������: armCacheV5Enable
** ��������: ʹ�� CACHE 
** �䡡��  : cachetype      INSTRUCTION_CACHE / DATA_CACHE
** �䡡��  : ERROR or OK
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static INT  armCacheV5Enable (LW_CACHE_TYPE  cachetype)
{
    if (cachetype == INSTRUCTION_CACHE) {
        armICacheEnable();
    
    } else {
        armDCacheEnable();
    }
    
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: armCacheV5Disable
** ��������: ���� CACHE 
** �䡡��  : cachetype      INSTRUCTION_CACHE / DATA_CACHE
** �䡡��  : ERROR or OK
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static INT  armCacheV5Disable (LW_CACHE_TYPE  cachetype)
{
    if (cachetype == INSTRUCTION_CACHE) {
        armICacheDisable();
    
    } else {
        armDCacheV5Disable();
    }
     
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: armCacheV5Flush
** ��������: CACHE �����ݻ�д
** �䡡��  : cachetype     CACHE ����
**           pvAdrs        �����ַ
**           stBytes       ����
** �䡡��  : ERROR or OK
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static INT	armCacheV5Flush (LW_CACHE_TYPE  cachetype, PVOID  pvAdrs, size_t  stBytes)
{
    addr_t  ulEnd;
    
    if (cachetype == DATA_CACHE) {
        if (stBytes >= ARMv5_CACHE_LOOP_OP_MAX_SIZE) {
            armDCacheV5FlushAll();
            
        } else {
            ARM_CACHE_GET_END(pvAdrs, stBytes, ulEnd, ARMv5_CACHE_LINE_SIZE);
            armDCacheFlush(pvAdrs, (PVOID)ulEnd, ARMv5_CACHE_LINE_SIZE);/*  ���ֻ�д                    */
        }
    }
    
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: armCacheV5FlushPage
** ��������: CACHE �����ݻ�д
** �䡡��  : cachetype     CACHE ����
**           pvAdrs        �����ַ
**           pvPdrs        ������ַ
**           stBytes       ����
** �䡡��  : ERROR or OK
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static INT	armCacheV5FlushPage (LW_CACHE_TYPE  cachetype, PVOID  pvAdrs, PVOID  pvPdrs, size_t  stBytes)
{
    addr_t  ulEnd;
    
    if (cachetype == DATA_CACHE) {
        if (stBytes >= ARMv5_CACHE_LOOP_OP_MAX_SIZE) {
            armDCacheV5FlushAll();                                      /*  ȫ����д                    */
        
        } else {
            ARM_CACHE_GET_END(pvAdrs, stBytes, ulEnd, ARMv5_CACHE_LINE_SIZE);
            armDCacheFlush(pvAdrs, (PVOID)ulEnd, ARMv5_CACHE_LINE_SIZE);/*  ���ֻ�д                    */
        }
    }
    
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: armCacheV5Invalidate
** ��������: ָ�����͵� CACHE ʹ������Ч(���ʲ�����)
** �䡡��  : cachetype     CACHE ����
**           pvAdrs        �����ַ
**           stBytes       ����
** �䡡��  : ERROR or OK
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static INT	armCacheV5Invalidate (LW_CACHE_TYPE  cachetype, PVOID  pvAdrs, size_t  stBytes)
{
    addr_t  ulEnd;

    if (cachetype == INSTRUCTION_CACHE) {
        if (stBytes >= ARMv5_CACHE_LOOP_OP_MAX_SIZE) {
            armICacheInvalidateAll();                                   /*  ICACHE ȫ����Ч             */
        
        } else {
            ARM_CACHE_GET_END(pvAdrs, stBytes, ulEnd, ARMv5_CACHE_LINE_SIZE);
            armICacheInvalidate(pvAdrs, (PVOID)ulEnd, ARMv5_CACHE_LINE_SIZE);
        }
    } else {
        if (stBytes > 0) {                                              /*  ���� > 0                    */
            addr_t  ulStart = (addr_t)pvAdrs;
                    ulEnd   = ulStart + stBytes;
            
            if (ulStart & (ARMv5_CACHE_LINE_SIZE - 1)) {                /*  ��ʼ��ַ�� cache line ����  */
                ulStart &= ~(ARMv5_CACHE_LINE_SIZE - 1);
                armDCacheClear((PVOID)ulStart, (PVOID)ulStart, ARMv5_CACHE_LINE_SIZE);
                ulStart += ARMv5_CACHE_LINE_SIZE;
            }
            
            if (ulEnd & (ARMv5_CACHE_LINE_SIZE - 1)) {                  /*  ������ַ�� cache line ����  */
                ulEnd &= ~(ARMv5_CACHE_LINE_SIZE - 1);
                armDCacheClear((PVOID)ulEnd, (PVOID)ulEnd, ARMv5_CACHE_LINE_SIZE);
            }
                                                                        /*  ����Ч���벿��              */
            armDCacheInvalidate((PVOID)ulStart, (PVOID)ulEnd, ARMv5_CACHE_LINE_SIZE);
        } else {
            _DebugHandle(__ERRORMESSAGE_LEVEL, "stBytes == 0.\r\n");
        }
    }
    
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: armCacheV5InvalidatePage
** ��������: ָ�����͵� CACHE ʹ������Ч(���ʲ�����)
** �䡡��  : cachetype     CACHE ����
**           pvAdrs        �����ַ
**           pvPdrs        ������ַ
**           stBytes       ����
** �䡡��  : ERROR or OK
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static INT	armCacheV5InvalidatePage (LW_CACHE_TYPE cachetype, PVOID pvAdrs, PVOID pvPdrs, size_t stBytes)
{
    addr_t  ulEnd;
    
    if (cachetype == INSTRUCTION_CACHE) {
        if (stBytes >= ARMv5_CACHE_LOOP_OP_MAX_SIZE) {
            armICacheInvalidateAll();                                   /*  ICACHE ȫ����Ч             */
        
        } else {
            ARM_CACHE_GET_END(pvAdrs, stBytes, ulEnd, ARMv5_CACHE_LINE_SIZE);
            armICacheInvalidate(pvAdrs, (PVOID)ulEnd, ARMv5_CACHE_LINE_SIZE);
        }
    } else {
        if (stBytes > 0) {                                              /*  ���� > 0                    */
            addr_t  ulStart = (addr_t)pvAdrs;
                    ulEnd   = ulStart + stBytes;
                    
            if (ulStart & (ARMv5_CACHE_LINE_SIZE - 1)) {                /*  ��ʼ��ַ�� cache line ����  */
                ulStart &= ~(ARMv5_CACHE_LINE_SIZE - 1);
                armDCacheClear((PVOID)ulStart, (PVOID)ulStart, ARMv5_CACHE_LINE_SIZE);
                ulStart += ARMv5_CACHE_LINE_SIZE;
            }
            
            if (ulEnd & (ARMv5_CACHE_LINE_SIZE - 1)) {                  /*  ������ַ�� cache line ����  */
                ulEnd &= ~(ARMv5_CACHE_LINE_SIZE - 1);
                armDCacheClear((PVOID)ulEnd, (PVOID)ulEnd, ARMv5_CACHE_LINE_SIZE);
            }
                                                                        /*  ����Ч���벿��              */
            armDCacheInvalidate((PVOID)ulStart, (PVOID)ulEnd, ARMv5_CACHE_LINE_SIZE);
        } else {
            _DebugHandle(__ERRORMESSAGE_LEVEL, "stBytes == 0.\r\n");
        }
    }
    
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: armCacheV5Clear
** ��������: ָ�����͵� CACHE ʹ���ֻ�ȫ�����(��д�ڴ�)����Ч(���ʲ�����)
** �䡡��  : cachetype     CACHE ����
**           pvAdrs        �����ַ
**           stBytes       ����
** �䡡��  : ERROR or OK
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static INT	armCacheV5Clear (LW_CACHE_TYPE  cachetype, PVOID  pvAdrs, size_t  stBytes)
{
    addr_t  ulEnd;

    if (cachetype == INSTRUCTION_CACHE) {
        if (stBytes >= ARMv5_CACHE_LOOP_OP_MAX_SIZE) {
            armICacheInvalidateAll();                                   /*  ICACHE ȫ����Ч             */
            
        } else {
            ARM_CACHE_GET_END(pvAdrs, stBytes, ulEnd, ARMv5_CACHE_LINE_SIZE);
            armICacheInvalidate(pvAdrs, (PVOID)ulEnd, ARMv5_CACHE_LINE_SIZE);
        }
    } else {
        if (stBytes >= ARMv5_CACHE_LOOP_OP_MAX_SIZE) {
            armDCacheV5ClearAll();                                      /*  ȫ����д����Ч              */
        
        } else {
            ARM_CACHE_GET_END(pvAdrs, stBytes, ulEnd, ARMv5_CACHE_LINE_SIZE);
            armDCacheClear(pvAdrs, (PVOID)ulEnd, ARMv5_CACHE_LINE_SIZE);/*  ���ֻ�д����Ч              */
        }
    }
    
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: armCacheV5ClearPage
** ��������: ָ�����͵� CACHE ʹ���ֻ�ȫ�����(��д�ڴ�)����Ч(���ʲ�����)
** �䡡��  : cachetype     CACHE ����
**           pvAdrs        �����ַ
**           pvPdrs        ������ַ
**           stBytes       ����
** �䡡��  : ERROR or OK
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static INT	armCacheV5ClearPage (LW_CACHE_TYPE cachetype, PVOID pvAdrs, PVOID pvPdrs, size_t stBytes)
{
    addr_t  ulEnd;
    
    if (cachetype == INSTRUCTION_CACHE) {
        if (stBytes >= ARMv5_CACHE_LOOP_OP_MAX_SIZE) {
            armICacheInvalidateAll();                                   /*  ICACHE ȫ����Ч             */
            
        } else {
            ARM_CACHE_GET_END(pvAdrs, stBytes, ulEnd, ARMv5_CACHE_LINE_SIZE);
            armICacheInvalidate(pvAdrs, (PVOID)ulEnd, ARMv5_CACHE_LINE_SIZE);
        }
    } else {
        if (stBytes >= ARMv5_CACHE_LOOP_OP_MAX_SIZE) {
            armDCacheV5ClearAll();                                      /*  ȫ����д����Ч              */
        
        } else {
            ARM_CACHE_GET_END(pvAdrs, stBytes, ulEnd, ARMv5_CACHE_LINE_SIZE);
            armDCacheClear(pvAdrs, (PVOID)ulEnd, ARMv5_CACHE_LINE_SIZE);/*  ���ֻ�д����Ч              */
        }
    }
    
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: armCacheV5Lock
** ��������: ����ָ�����͵� CACHE 
** �䡡��  : cachetype     CACHE ����
**           pvAdrs        �����ַ
**           stBytes       ����
** �䡡��  : ERROR or OK
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static INT	armCacheV5Lock (LW_CACHE_TYPE  cachetype, PVOID  pvAdrs, size_t  stBytes)
{
    _ErrorHandle(ENOSYS);
    return  (PX_ERROR);
}
/*********************************************************************************************************
** ��������: armCacheV5Unlock
** ��������: ����ָ�����͵� CACHE 
** �䡡��  : cachetype     CACHE ����
**           pvAdrs        �����ַ
**           stBytes       ����
** �䡡��  : ERROR or OK
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static INT	armCacheV5Unlock (LW_CACHE_TYPE  cachetype, PVOID  pvAdrs, size_t  stBytes)
{
    _ErrorHandle(ENOSYS);
    return  (PX_ERROR);
}
/*********************************************************************************************************
** ��������: armCacheV5TextUpdate
** ��������: ���(��д�ڴ�) D CACHE ��Ч(���ʲ�����) I CACHE
** �䡡��  : pvAdrs                        �����ַ
**           stBytes                       ����
** �䡡��  : ERROR or OK
** ȫ�ֱ���: 
** ����ģ��: 
** ע  ��  : L2 cache Ϊͳһ CACHE ���� text update ����Ҫ���� L2 cache.
*********************************************************************************************************/
static INT	armCacheV5TextUpdate (PVOID  pvAdrs, size_t  stBytes)
{
    addr_t  ulEnd;
    
    if (stBytes >= ARMv5_CACHE_LOOP_OP_MAX_SIZE) {
        armDCacheV5FlushAll();                                          /*  DCACHE ȫ����д             */
        armICacheInvalidateAll();                                       /*  ICACHE ȫ����Ч             */
        
    } else {
        ARM_CACHE_GET_END(pvAdrs, stBytes, ulEnd, ARMv5_CACHE_LINE_SIZE);
        armDCacheFlush(pvAdrs, (PVOID)ulEnd, ARMv5_CACHE_LINE_SIZE);    /*  ���ֻ�д                    */
        armICacheInvalidate(pvAdrs, (PVOID)ulEnd, ARMv5_CACHE_LINE_SIZE);
    }
    
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: armCacheV5DataUpdate
** ��������: ��д D CACHE (����д CPU ������ CACHE)
** �䡡��  : pvAdrs                        �����ַ
**           stBytes                       ����
**           bInv                          �Ƿ�Ϊ��д��Ч
** �䡡��  : ERROR or OK
** ȫ�ֱ���: 
** ����ģ��: 
** ע  ��  : L2 cache Ϊͳһ CACHE ���� data update ����Ҫ���� L2 cache.
*********************************************************************************************************/
static INT	armCacheV5DataUpdate (PVOID  pvAdrs, size_t  stBytes, BOOL  bInv)
{
    addr_t  ulEnd;
    
    if (bInv == LW_FALSE) {
        if (stBytes >= ARMv5_CACHE_LOOP_OP_MAX_SIZE) {
            armDCacheV5FlushAll();                                      /*  ȫ����д                    */
        
        } else {
            ARM_CACHE_GET_END(pvAdrs, stBytes, ulEnd, ARMv5_CACHE_LINE_SIZE);
            armDCacheFlush(pvAdrs, (PVOID)ulEnd, ARMv5_CACHE_LINE_SIZE);/*  ���ֻ�д                    */
        }
    
    } else {
        if (stBytes >= ARMv5_CACHE_LOOP_OP_MAX_SIZE) {
            armDCacheV5ClearAll();                                      /*  ȫ����д                    */
        
        } else {
            ARM_CACHE_GET_END(pvAdrs, stBytes, ulEnd, ARMv5_CACHE_LINE_SIZE);
            armDCacheClear(pvAdrs, (PVOID)ulEnd, ARMv5_CACHE_LINE_SIZE);/*  ���ֻ�д                    */
        }
    }
    
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: archCacheV5Init
** ��������: ��ʼ�� CACHE 
** �䡡��  : pcacheop       CACHE ����������
**           uiInstruction  ָ�� CACHE ����
**           uiData         ���� CACHE ����
**           pcMachineName  ��������
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  armCacheV5Init (LW_CACHE_OP *pcacheop, 
                      CACHE_MODE   uiInstruction, 
                      CACHE_MODE   uiData, 
                      CPCHAR       pcMachineName)
{
    pcacheop->CACHEOP_ulOption = 0ul;

    pcacheop->CACHEOP_iILoc = CACHE_LOCATION_VIVT;
    pcacheop->CACHEOP_iDLoc = CACHE_LOCATION_VIVT;
    
    pcacheop->CACHEOP_iICacheLine = ARMv5_CACHE_LINE_SIZE;
    pcacheop->CACHEOP_iDCacheLine = ARMv5_CACHE_LINE_SIZE;
    
    pcacheop->CACHEOP_iICacheWaySize = (4 * LW_CFG_KB_SIZE);
    pcacheop->CACHEOP_iDCacheWaySize = (4 * LW_CFG_KB_SIZE);
    
    pcacheop->CACHEOP_pfuncEnable  = armCacheV5Enable;
    pcacheop->CACHEOP_pfuncDisable = armCacheV5Disable;
    
    pcacheop->CACHEOP_pfuncLock    = armCacheV5Lock;                    /*  ��ʱ��֧����������          */
    pcacheop->CACHEOP_pfuncUnlock  = armCacheV5Unlock;
    
    pcacheop->CACHEOP_pfuncFlush          = armCacheV5Flush;
    pcacheop->CACHEOP_pfuncFlushPage      = armCacheV5FlushPage;
    pcacheop->CACHEOP_pfuncInvalidate     = armCacheV5Invalidate;
    pcacheop->CACHEOP_pfuncInvalidatePage = armCacheV5InvalidatePage;
    pcacheop->CACHEOP_pfuncClear          = armCacheV5Clear;
    pcacheop->CACHEOP_pfuncClearPage      = armCacheV5ClearPage;
    pcacheop->CACHEOP_pfuncTextUpdate     = armCacheV5TextUpdate;
    pcacheop->CACHEOP_pfuncDataUpdate     = armCacheV5DataUpdate;
    
#if LW_CFG_VMM_EN > 0
    pcacheop->CACHEOP_pfuncDmaMalloc      = API_VmmDmaAlloc;
    pcacheop->CACHEOP_pfuncDmaMallocAlign = API_VmmDmaAllocAlign;
    pcacheop->CACHEOP_pfuncDmaFree        = API_VmmDmaFree;
#endif                                                                  /*  LW_CFG_VMM_EN > 0           */
}
/*********************************************************************************************************
** ��������: archCacheV5Reset
** ��������: ��λ CACHE 
** �䡡��  : pcMachineName  ��������
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  armCacheV5Reset (CPCHAR  pcMachineName)
{
    armICacheInvalidateAll();
    armDCacheV5Disable();
    armICacheDisable();
}

#endif                                                                  /*  LW_CFG_CACHE_EN > 0         */
#endif                                                                  /*  !__SYLIXOS_ARM_ARCH_M__     */
/*********************************************************************************************************
  END
*********************************************************************************************************/