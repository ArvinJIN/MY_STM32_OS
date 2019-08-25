    .syntax unified
    
    .extern pTCBCur
    .extern pTCBPrioHighRdy

    .equ    NVIC_INT_CTRL,0xE000ED04
    .equ    NVIC_PENDSVSET,0x10000000
    .equ    NVIC_SYSPRI14,0xE000ED22
    .equ    NVIC_PENDSV_PRI,0xFF

    .text
    .thumb
    .thumb_func
    .align  3
    
    .globl OS_CPU_SR_Save
    .type OS_CPU_SR_Save,%function
OS_CPU_SR_Save:
    MRS     R0, PRIMASK
    CPSID   I
    BX      LR

    .globl OS_CPU_SR_Restore
    .type OS_CPU_SR_Restore,%function
OS_CPU_SR_Restore:
    MSR     PRIMASK, R0
    BX      LR



    .globl 	OSStartHighRdy
    .type   OSStartHighRdy,%function
OSStartHighRdy:
    LDR		R0,=NVIC_SYSPRI14
	LDR		R1,=NVIC_PENDSV_PRI
	STRB	R1,[R0]
	
	MOVS	R0,#0
	MSR		PSP,R0
	
	LDR		R0,=CPU_ExceptStkBase
	LDR		R1,[R0]
	MSR		MSP,R1
	
	LDR		R0,=NVIC_INT_CTRL
	LDR		R1,=NVIC_PENDSVSET
	STR		R1,[R0]
	
	CPSIE	I


    .globl  PendSV_Handler
    .type   PendSV_Handler,%function
PendSV_Handler:
    CPSID	I							
	MRS 	R0, PSP						
	CBZ 	R0, PendSV_Handler_NoSave	
	SUB 	R0, R0, #0x20
	STM 	R0, {R4-R11}
	
	LDR 	R1, =pTCBCur			
	LDR 	R1, [R1] 				
	STR 	R0, [R1] 			

PendSV_Handler_NoSave:
	LDR 	R0, =pTCBCur 			
	LDR 	R1, =pTCBPrioHighRdy			
	LDR 	R2, [R1] 				
	STR 	R2, [R0]				
	LDR 	R0, [R2] 				
	LDM 	R0!, {R4-R11}
	
	MSR 	PSP, R0 				
	ORR 	LR, LR, #0x04 		
	CPSIE 	I 					
	BX	LR 						

    
    .globl  OSCtxSw
    .type   OSCtxSw,%function

OSCtxSw:
    LDR		R0,=NVIC_INT_CTRL
	LDR		R1,=NVIC_PENDSVSET
	STR		R1,[R0]
	BX		LR

    .align 2
    .end


