#include "memory.h"
#include "define.h"
#include "myos.h"



void OS_MemClear(u8 *pdest, u32 size){
	while(size > 0){
		*pdest++ = 0;
		size--;
	}
}

/*
***************************************************************************
初始化内存分区管理，在OSInit()调用
***************************************************************************
*/
void OS_MemInit(void){
	OS_MEM *pmem;
	u16 i;
	OS_MemClear((u8 *)(&OS_MemTbl[0]), sizeof(OS_MemTbl));
	pmem = &OS_MemTbl[0];
	for(i=0;i<(OS_MAX_MEM_PART-1);i++){
		pmem->OSMemFreeList = (void *)&OS_MemTbl[i+1];
		pmem++;
	}
	pmem->OSMemFreeList = (void *)0;
	OS_MemFreeList = &OS_MemTbl[0];
}





OS_MEM  *OSMemCreate (void *addr, u32 nblks, u32 blksize){
	OS_MEM *pmem;
	u8 *pblk;
	void **plink;
	u32 i;
	OS_CPU_SR cpu_sr = 0;

	OS_ENTER_CRITICAL();
	pmem = OS_MemFreeList;  //从MCB链表中去除一个空闲的MCB.
	if(OS_MemFreeList != (OS_MEM *)0){
		OS_MemFreeList = (OS_MEM *)OS_MemFreeList->OSMemFreeList;
	}
	OS_EXIT_CRITICAL();
	if(pmem == (OS_MEM *)0){
		return ((OS_MEM *)0);
	}
	plink = (void **)addr;
	pblk = (u8 *)((u32)addr + blksize);
	for(i =0 ; i < (nblks -1); i++){
		*plink = (void *)pblk;
		plink = (void **)pblk;
		pblk = (u8 *)((u32)pblk + blksize);
	}
	*plink = (void *)0;
	pmem->OSMemAddr = addr;
	pmem->OSMemFreeList = addr;
	pmem->OSMemNFree = nblks;
	pmem->OSMemNBlks = nblks;
	pmem->OSMemBlkSize = blksize;
	return (pmem);
}




void *OSMemGet(OS_MEM *pmem){
	void *pblk;
	OS_CPU_SR cpu_sr = 0;

	OS_ENTER_CRITICAL();
	if(pmem->OSMemNFree > 0){
		pblk = pmem->OSMemFreeList;
		pmem->OSMemFreeList = *(void **)pblk;
		pmem->OSMemNFree--;
		OS_EXIT_CRITICAL();
		return (pblk);
	}
	OS_EXIT_CRITICAL();
	return (void *)0;

}


u8 OSMemPut(OS_MEM *pmem, void *pblk){
	OS_CPU_SR cpu_sr = 0;

	OS_ENTER_CRITICAL();
	if(pmem->OSMemNFree >= pmem->OSMemNBlks){
		return -1;
	*(void **)pblk = pmem->OSMemFreeList;
	pmem->OSMemFreeList = pblk;
	pmem->OSMemNFree++;
	OS_EXIT_CRITICAL();
	return 0;
	}
}


u8 OSMemQuery(OS_MEM *pmem, OS_MEM_DATA *p_mem_data){
	OS_CPU_SR  cpu_sr = 0;

	OS_ENTER_CRITICAL();
	p_mem_data->OSAddr = pmem->OSMemAddr;
	p_mem_data->OSFreeList = pmem->OSMemFreeList;
	p_mem_data->OSBlkSize = pmem->OSMemBlkSize;
	p_mem_data->OSNBlks = pmem->OSMemNBlks;
	p_mem_data->OSNFree = pmem->OSMemNFree;
	OS_EXIT_CRITICAL();
	p_mem_data->OSNUsed = p_mem_data->OSNBlks - p_mem_data->OSNFree;
	return 0;
}




