/***************************************************************************
 *                                                                         									     *
 * Copyright (c) 2008 Nuvoton Technolog. All rights reserved.              					     *
 *                                                                         									     *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wblib.h"
#include "demo.h"

#define MEM_SIZE			0x100000
#define NON_CACHE_BIT		0x80000000

void DemoAPI_Cache(BOOL bIsCacheOn)
{
	UINT32 tmp;
	char *pUINT8Buf0, *pUINT8Buf1;
	volatile unsigned int  u32Btime, u32Etime;
	
	pUINT8Buf0 = (char*)0x800000;//(char*)malloc(MEM_SIZE);
	pUINT8Buf1 = (char*)0x1000000;//(char*)malloc(MEM_SIZE);
	
	//pUINT8Buf0 = (UINT8*)MEM_BUF1;
	if(bIsCacheOn)
	{
		//sysSetMMUMappingMethod(MMU_INVERSE_MAPPING);
		sysEnableCache(CACHE_WRITE_BACK);
		DBG_PRINTF("Enable Cache\n");
	}
	else
	{
		sysDisableCache(); 	
		sysFlushCache(I_D_CACHE);		
		DBG_PRINTF("Disable Cache\n");
	}
	for(tmp=0;tmp<MEM_SIZE; tmp = tmp+1)
	{
		*pUINT8Buf0++ = tmp*123;
	}	
	sysSetTimerReferenceClock(TIMER0, sysGetExternalClock()); 		//External Crystal
	sysStartTimer(TIMER0, 100, PERIODIC_MODE);			/* 100 ticks/per sec ==> 1tick/10ms */
	
	sysSetLocalInterrupt(ENABLE_IRQ);	
	/* Copy time */
	u32Btime = sysGetTicks(TIMER0);	
	memcpy((UINT8*) ((UINT32)pUINT8Buf0 | NON_CACHE_BIT), 
			(UINT8*)((UINT32)pUINT8Buf1 | NON_CACHE_BIT), 
			MEM_SIZE);
	u32Etime = sysGetTicks(TIMER0);
	DBG_PRINTF("Copy 0x%x bytes memory takes %d ms\n",MEM_SIZE, (u32Etime-u32Btime)*10);
	
	/* Compare time */
	u32Btime = sysGetTicks(TIMER0);
#if 1	
	if(memcmp((UINT8*) ((UINT32)pUINT8Buf0 | NON_CACHE_BIT), 
				(UINT8*)((UINT32)pUINT8Buf1 | NON_CACHE_BIT), 
				MEM_SIZE)!=0)
	{
		DBG_PRINTF("Compare error\n");
		while(1);
	}
#else
	for(tmp=0; tmp<MEM_SIZE;tmp=tmp+1)
	{
		if(*pUINT8Buf0++ !=*pUINT8Buf1++)
		{
			DBG_PRINTF("Compare error\n");
			DBG_PRINTF("Src Addr 0x%x\n",(UINT32)pUINT8Buf0);
			DBG_PRINTF("Dst Addr 0x%x\n",(UINT32)pUINT8Buf1);
		}	
	}
#endif	
	u32Etime = sysGetTicks(TIMER0);
	DBG_PRINTF("Compare 0x%x bytes memory takes %d ms\n", MEM_SIZE, (u32Etime-u32Btime)*10);
	//free(pUINT8Buf0);
	//free(pUINT8Buf1);
}
