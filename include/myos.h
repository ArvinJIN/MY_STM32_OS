
#ifndef MYOS_H
#define MYOS_H

#include "define.h"
#include "memory.h"



typedef unsigned int OS_STK;    // Each stack entry is 32-bit wide(OS Stack)
#define System_Ticks 100u       //每1000/System_Ticks ms进入一次中断

/***************************************************************/

#define OS_MAX_TASKS 16         //最大任务数
#define TASK_STATE_CREATING 0
#define TASK_STATE_RUNNING 1
#define TASK_STATE_PAUSING 2
#define TASK_STACK_SIZE 64      //堆栈大小



u32 CPU_ExceptStk[TASK_STACK_SIZE]; //主任务堆栈
u32 *CPU_ExceptStkBase; //指向数组最后一个元素
u32 IDLE_STK[TASK_STACK_SIZE]; //空闲任务堆栈

OS_CPU_SR cpu_sr;
#define  OS_ENTER_CRITICAL()  {cpu_sr = OS_CPU_SR_Save();}
#define  OS_EXIT_CRITICAL()   {OS_CPU_SR_Restore(cpu_sr);}




typedef struct os_tcb{
    OS_STK *StkPtr; // 任务栈顶
    u32 DLy;    // 任务延时
}OS_TCB; // TCB 任务控制块


extern OS_CPU_SR  OS_CPU_SR_Save(void);
extern void       OS_CPU_SR_Restore(OS_CPU_SR cpu_sr);
extern void OSCtxSw(void);             // 切换任务上下文
extern void OSStartHighRdy(void);


void OS_TaskSuspend(unsigned char prio);
void OS_TaskResume(u8 prio);
void OSTaskCreate(void (*task)(void),unsigned int *stk,unsigned char prio);
void OSSetPrioRdy(u8 prio);
void OSDelPrioRdy(u8 prio);
void OSGetHighRdy(void);
void OSStart();
void OS_Sched();
void OSTimeDly(unsigned int ticks);
void Task_End(void);

#endif