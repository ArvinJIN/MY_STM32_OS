#include "myos.h"
#include "define.h"

OS_TCB TCB_Task[OS_MAX_TASKS];
OS_CPU_SR cpu_sr;


uint8_t OS_PrioCur;//记录当前运行的任务优先级
uint8_t OS_PrioHighRdy;
volatile uint32_t OSRdyTbl; //任务就绪表
OS_TCB *pTCBCur;   // 当前运行任务的TCB
OS_TCB *pTCBPrioHighRdy; //记录最高优先级任务的TCB




__inline void OSSetPrioRdy(u8 prio){
    OSRdyTbl|=0x01<<prio;
}


__inline void OSDelPrioRdy(u8 prio)
{
	OSRdyTbl&=~(0x01<<prio);
}

//
__inline void OSGetHighRdy(void)				
{									
	u8	OS_NEXT_Prio;	
	for(OS_NEXT_Prio=0; (OS_NEXT_Prio<OS_MAX_TASKS)&&(!(OSRdyTbl&(0x01<<OS_NEXT_Prio))); OS_NEXT_Prio++);
	OS_PrioHighRdy=OS_NEXT_Prio;	
}


/*---------------------------------------
-* Description：创建一个新的任务,初始化任务为待恢复状态
-* Arguments:  	task		任务函数名
-* 				stk			任务堆栈
-* 				prio		任务优先级
-*---------------------------------------*/
void OSTaskCreate(void (*task)(void),unsigned int *stk,unsigned char prio){
unsigned int * p_stk;
	p_stk=stk;
	p_stk=(unsigned int *) ((unsigned int)(p_stk)&0xFFFFFFF8u);
	//以下寄存器顺序和PendSV退出时寄存器恢复顺序一致
	*(--p_stk)=(unsigned int)0x01000000uL;//xPSR状态寄存器、第24位THUMB模式必须置位一 
	*(--p_stk)=(unsigned int)task;//entry point//函数入口
	*(--p_stk)=(unsigned int)Task_End ;//R14(LR);
	*(--p_stk)=(unsigned int)0x12121212uL;//R12
	*(--p_stk)=(unsigned int)0x03030303uL;//R3
	*(--p_stk)=(unsigned int)0x02020202uL;//R2
	*(--p_stk)=(unsigned int)0x01010101uL;//R1
	*(--p_stk)=(unsigned int)0x00000000uL;//R0
	//PendSV发生时未自动保存的内核寄存器：R4~R11
	*(--p_stk)=(unsigned int)0x11111111uL;//R11
	*(--p_stk)=(unsigned int)0x10101010uL;//R10
	*(--p_stk)=(unsigned int)0x09090909uL;//R9
	*(--p_stk)=(unsigned int)0x08080808uL;//R8
	*(--p_stk)=(unsigned int)0x07070707uL;//R7
	*(--p_stk)=(unsigned int)0x06060606uL;//R6
	*(--p_stk)=(unsigned int)0x05050505uL;//R5
	*(--p_stk)=(unsigned int)0x04040404uL;//R4
	
	TCB_Task[prio].StkPtr =p_stk;//将该任务控制块中应当指向栈顶的指针，指向了该任务的新栈顶
	TCB_Task[prio].DLy =0;
	OSSetPrioRdy(prio);//设置 优先级
}

/*---------------------------------------
-* File: Task_End
-* Description：任务运行结束时进入此函数，理论上每个子任务都是一个死循环，不会结束
-* Arguments:  	void				
-*---------------------------------------*/

void Task_End(void)
{
	while(1){ 
    };
}


void OS_TaskIdle(void *p_arg){
    p_arg = p_arg;
    for(;;){

    }
}


void OSStart(){
    SysTickInit();
    CPU_ExceptStkBase=CPU_ExceptStk+TASK_STACK_SIZE-1;//Cortex-M3栈向下增长
	OSTaskCreate(OS_TaskIdle,&IDLE_STK[TASK_STACK_SIZE-1],OS_MAX_TASKS-1);//空闲任务
	OSGetHighRdy();//获得最高级的就绪任务
	OS_PrioCur= OS_PrioHighRdy;
	//p_TCB_Cur=&TCB_Task[OS_PrioCur];
	pTCBPrioHighRdy=&TCB_Task[OS_PrioHighRdy];
    OSStartHighRdy();
}

/*---------------------------------------
-* File: SysTickInit
-* Description：系统(滴答)时钟初始化
-* Arguments:  	void
-*---------------------------------------*/
void SysTickInit(void){
    #define NVIC_STCSR ((volatile unsigned long *)(0xE000E010))
    #define NVIC_RELOAD ((volatile unsigned long *)(0xE000E014))
    #define NVIC_CURRVAL ((volatile unsigned long *)(0xE000E018))
    #define NVIC_CALVAL ((volatile unsigned long *)(0xE000E01C))

    *NVIC_STCSR = 0; // 除能 SYSTICK
    *NVIC_RELOAD = 499999; // 基于50MHz主频的100Hz装载值 
    *NVIC_CURRVAL = 0; // 清除当前值
    *NVIC_STCSR = 0x7; // 使能SYSTICK，使能中断，使用内核时钟 
    return;

}

/*---------------------------------------
-* File: SysTick_Handler
-* Description：系统(滴答)时钟中断,每1000/System_Ticks ms中断一次
-* Arguments:  	void
-*---------------------------------------*/
void SysTick_Handler(void)
{
	unsigned int cpu_sr;
	unsigned char i=0;
	//对于Cortex-M3,任务切换是由pendsv中断实现的，而pendsv中断的优先级最低
	//即使中断嵌套发生了任务调度也没事，真正的任务切换也不会发生，只是触发了pendsv中断了而已
	for(;i<OS_MAX_TASKS;i++)
    {
		OS_ENTER_CRITICAL();
		if(TCB_Task[i].DLy)
		{
			TCB_Task[i].DLy-=1000/System_Ticks;
			if(TCB_Task[i].DLy==0)			//延时时钟到达
			{
				OSSetPrioRdy(i);            //任务重新就绪
			}
		}
		OS_EXIT_CRITICAL();
	}
	
	OS_Sched();//都是由pendsv中断进行调度
}


void OS_Sched(){
    OS_CPU_SR cpu_sr;
    OS_ENTER_CRITICAL();
    OSGetHighRdy();
    if(OS_PrioHighRdy!=OS_PrioCur){
        pTCBPrioHighRdy = &TCB_Task[OS_PrioHighRdy];
        OS_PrioCur = OS_PrioHighRdy;
        OSCtxSw();
    }
    OS_EXIT_CRITICAL();
}


static u16 fac_ms=0;							//ms延时倍乘数,在ucos下,代表每个节拍的ms数

/*---------------------------------------
-* File: OSTimeDly
-* Description：系统延时
-* Arguments:  	ticks		延时时间
-*---------------------------------------*/
void OSTimeDly(unsigned int ticks)
{
	if(ticks> 0)
	{
		unsigned int cpu_sr;
		OS_ENTER_CRITICAL();                                  //进入临界区
		OSDelPrioRdy(OS_PrioCur);                             //将任务挂起
		TCB_Task[OS_PrioCur].DLy= ticks;                      //设置TCB中任务延时节拍数
		OS_EXIT_CRITICAL();                                   //退出临界区
		OS_Sched();                                           //任务调度
		//return ;
	}	  	    
}


