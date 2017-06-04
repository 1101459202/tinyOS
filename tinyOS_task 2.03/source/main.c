#include "tinyOS.h"
#include "ARMCm3.h"

	
tTask *currentTask;
tTask *nextTask;
tTask *taskTable[2];


void delay(int count);
void tTaskSched();//任务调度函数
void tSetSysTickPeriod (uint32_t ms);

void tSetSysTickPeriod (uint32_t ms)
{
	SysTick->LOAD = ms * SystemCoreClock / 1000 -1; //初始化重载
	NVIC_SetPriority(SysTick_IRQn,((1 << __NVIC_PRIO_BITS) - 1));
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |   //控制寄存器
								  SysTick_CTRL_TICKINT_Msk |
									SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Handler()
{
	tTaskSched();//任务调度函数
}
/************** 结构 *********/
typedef struct _BlockType_t
{
	unsigned long *stackPtr;//堆栈地址
}BlockType_t;

BlockType_t *blockPtr,block;

//缓存区保存r4-r7
unsigned long stackBuffer[1024];

int flag = 0;

tTask tTask1;
tTask tTask2;

tTaskStack task1Env[1024];
tTaskStack task2Env[1024];

void tTaskInit (tTask * task,void (*entry)(void *),void * param,tTaskStack * stack)
{
	*(--stack) = (unsigned long)(1 << 24);
	*(--stack) = (unsigned long)entry;//PC
	*(--stack) = (unsigned long)0x14;//LR
	*(--stack) = (unsigned long)0x12;//R12
	*(--stack) = (unsigned long)0x3;//R3
	*(--stack) = (unsigned long)0x2;//R2
	*(--stack) = (unsigned long)0x1;//R1
	*(--stack) = (unsigned long)param;//R0
	
	*(--stack) = (unsigned long)0x11;//R11
	*(--stack) = (unsigned long)0x10;//R10
	*(--stack) = (unsigned long)0x9;//R9
	*(--stack) = (unsigned long)0x8;//R8
	*(--stack) = (unsigned long)0x7;//R7
	*(--stack) = (unsigned long)0x6;//R6
	*(--stack) = (unsigned long)0x5;//R5
	*(--stack) = (unsigned long)0x4;//R4
	
	task->stack = stack;
}

int task1Flag;
int task2Flag;

void tTaskSched()//任务调度函数
{
	if(currentTask == taskTable[0])
	{
		nextTask = taskTable[1];
	}
	else
	{
		nextTask = taskTable[0];
	}
	
	tTaskSwitch();
}

void task1Entry (void * param)
{
	tSetSysTickPeriod (10);
	for(;;)
	{
		task1Flag = 0;
		delay(100);
		task1Flag = 1;
		delay(100);
		
//		tTaskSched();
	}
}

void task2Entry (void * param)
{
	for(;;)
	{
		task2Flag = 0;
		delay(100);
		task2Flag = 1;
		delay(100);
		
//		tTaskSched();
	}
}
int main()
{
	tTaskInit(&tTask1,task1Entry,(void *)0x11111111,&task1Env[1024]);
	tTaskInit(&tTask2,task2Entry,(void *)0x22222222,&task2Env[1024]);
	
	taskTable[0] = &tTask1;
	taskTable[1] = &tTask2;
	
	nextTask = &tTask1;
	
	tTaskRunFirst();
	
	return 0;
}

void delay(int count)
{
	while(--count > 0);
}
