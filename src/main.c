#include "include.h"
#include <stdlib.h>
#include <string.h>

extern OS_TCB TCB_Task[OS_MAX_TASKS];
extern OS_STK TASK_IDLE_STK[TASK_STACK_SIZE];
extern OS_TCB *OSTCBCur;
extern OS_TCB *OSTCBNext;
extern u8 OSTaskNext;
extern u32 TaskTickLeft;
extern u32 TimeMS;
extern u32 TaskTimeSlice;

OS_STK Task1Stk[TASK_STACK_SIZE];
OS_STK Task2Stk[TASK_STACK_SIZE];
OS_STK Task3Stk[TASK_STACK_SIZE];


void Task1(void *p_arg);
void Task2(void *p_arg);
void Task3(void *p_arg);

OS_MEM *MemPtr1;
OS_MEM *MemPtr2;
OS_MEM *MemPtr3;

u8 MemPart1[10][1024];
u8 MemPart2[50][256];
u8 MemPart3[100][64];

int main(void){
    Stm32_Clock_Init();
    delay_init(72);
    LED_Init();
    LCD_Init();
    LCD_ShowString(60,200,300,16,16,"Hello World");    
    OS_MemInit();
    MY_NVIC_PriorityGroupConfig(2);

    OSTaskCreate(Task1,&Task1Stk[TASK_STACK_SIZE-1], 2);
    OSTaskCreate(Task2,&Task2Stk[TASK_STACK_SIZE-1], 1);
    OSTaskCreate(Task3,&Task3Stk[TASK_STACK_SIZE-1], 3);

    MemPtr1 = OSMemCreate(MemPart1,10,1024);
    MemPtr2 = OSMemCreate(MemPart2,50,256);
    MemPtr3 = OSMemCreate(MemPart3,100,64);

    char *str;
    str = OSMemGet(MemPtr3);
    strcpy(str,"Test for function OSMemGet");
    LCD_ShowString(60,400,300,16,16,str); 
    OSMemPut(MemPtr3,str);

    OSStart();
    return 0;
}



void Task1(void *p_arg){
    LCD_ShowString(60,260,300,16,16,"Task1");
    int i=0;
    while(1){
        i++;
        LCD_ShowNum(60,280,i,16,16);
        OSTimeDly(1000);
        LED0=0;
        OSTimeDly(1000);
        LED0=1;
        //LED0TURN();
        
    }
}

void Task2(void *p_arg){
    LCD_ShowString(60,300,300,16,16,"Task2 ");
    int i = 0;
    while(1){
        i++;
        LCD_ShowNum(60,320, i, 16, 16);
        OSTimeDly(500);
        LED1=1;
        OSTimeDly(500);
        LED1=0;
       
    }
}

void Task3(void *p_arg){
    LCD_ShowString(60,340,300,16,16,"Task3 ");
    int i = 0;
    while(1){
        i++;
        LCD_ShowNum(60, 360, i, 16, 16);
        OSTimeDly(300);
    }
}