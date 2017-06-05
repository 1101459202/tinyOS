#include "tinyOs.h"
#include "ARMCM3.h"

#define NVIC_INIT_CTRL	0xE000ED04 //�жϿ���״̬�Ĵ���
#define NVIC_PENDSVSET	0x10000000 //���ù��� NMI
#define NVIC_SYSPRI2		0xE000ED20 //ϵͳ������ 12-15 ���ȼ��Ĵ���
#define NVIC_PENDSV_PRI	0x00FF0000 //PendSV���ȼ�

#define MEM32(addr)	*(volatile unsigned long *)(addr)
#define MEM8(addr)  *(volatile unsigned int *)(addr)
	
__asm void PendSV_Handler(void)
{
	IMPORT currentTask
	IMPORT nextTask
	
	MRS R0,PSP
	CBZ R0,PendSV_nosave
	
	STMDB R0!,{R4-R11}
	LDR R1,=currentTask
	LDR R1,[R1]
	STR R0,[R1]
	
PendSV_nosave
	LDR R0,=currentTask
	LDR R1,=nextTask
	LDR R2,[R1]
	STR R2,[R0]
	
	LDR R0,[R2]
	
	LDMIA R0!,{R4-R11}
	
	MSR PSP,R0
	ORR LR,LR,#0x04
	BX LR
}

void tTaskRunFirst(void)
{
	__set_PSP(0);
	
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	MEM32(NVIC_INIT_CTRL) = NVIC_PENDSVSET;
}

void tTaskSwitch(void)//����penSV�쳣
{
	MEM32(NVIC_INIT_CTRL) = NVIC_PENDSVSET;
}
