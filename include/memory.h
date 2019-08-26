#ifndef MEMORY_H
#define MEMORY_H

#include "define.h"

#define OS_MAX_MEM_PART 5

/* MEMORY CONTROL BLOCK */
typedef struct os_mem{
    void *OSMemAddr;    // 指向内存分区的起始地址
    void *OSMemFreeList;//指向下一个空闲块
    u32 OSMemBlkSize;   //每个块的大小
    u32 OSMemNBlks;     //该分区里块的总数
    u32 OSMemNFree;     //该分区内当前空闲内存块数量
}OS_MEM;


typedef struct os_mem_data{
    void *OSAddr;       //指向内存分区的开始地址
    void *OSFreeList;   //指向空闲内存块链表的开始
    u32 OSBlkSize;      //每一个内存块的大小
    u32 OSNBlks;        //该分区内存块的总数
    u32 OSNFree;        //该分区中当前空闲内存块的数量
    u32 OSNUsed;        //该分区中已经使用了的内存块的数量
}OS_MEM_DATA;

OS_MEM *OS_MemFreeList;
OS_MEM OS_MemTbl[OS_MAX_MEM_PART];

void OS_MemInit(void);
void OS_MemClear(u8 *pdest, u32 size);
OS_MEM *OSMemCreate(void *addr, u32 nblks, u32 blksize);
void *OSMemGet(OS_MEM *pmem);
u8 OSMemPut(OS_MEM *pmem, void *pblk);
u8 OSMemQuery(OS_MEM *pmem, OS_MEM_DATA *p_mem_data);

#endif